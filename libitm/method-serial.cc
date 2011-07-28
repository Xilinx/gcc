/* Copyright (C) 2008, 2009 Free Software Foundation, Inc.
   Contributed by Richard Henderson <rth@redhat.com>.

   This file is part of the GNU Transactional Memory Library (libitm).

   Libitm is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   Libitm is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include "libitm_i.h"

// Avoid a dependency on libstdc++ for the pure virtuals in abi_dispatch.
extern "C" void HIDDEN
__cxa_pure_virtual ()
{
  abort ();
}

using namespace GTM;

namespace {

class serialirr_dispatch : public abi_dispatch
{
 public:
  serialirr_dispatch() : abi_dispatch(false, true, true, false) { }

 protected:
  // Transactional loads and stores simply access memory directly.
  // These methods are static to avoid indirect calls, and will be used by the
  // virtual ABI dispatch methods or by static direct-access methods created
  // below.
  template <typename V> static V load(const V* addr, ls_modifier mod)
  {
    return *addr;
  }
  template <typename V> static void store(V* addr, const V value,
      ls_modifier mod)
  {
    *addr = value;
  }

 public:
  static void memtransfer_static(void *dst, const void* src, size_t size,
      bool may_overlap, ls_modifier dst_mod, ls_modifier src_mod)
  {
    if (!may_overlap)
      ::memcpy(dst, src, size);
    else
      ::memmove(dst, src, size);
  }

  static void memset_static(void *dst, int c, size_t size, ls_modifier mod)
  {
    ::memset(dst, c, size);
  }

  CREATE_DISPATCH_METHODS(virtual, )
  CREATE_DISPATCH_METHODS_MEM()

  virtual bool trycommit() { return true; }
  virtual void rollback(gtm_transaction_cp *cp) { abort(); }
  virtual void reinit() { }
  virtual void fini() { }

  virtual abi_dispatch* closed_nesting_alternative()
  {
    // For nested transactions with an instrumented code path, we can do
    // undo logging.
    return GTM::dispatch_serial();
  }
};

class serial_dispatch : public abi_dispatch
{
protected:
  static void log(const void *addr, size_t len)
  {
    // TODO Ensure that this gets inlined: Use internal log interface and LTO.
    GTM_LB(addr, len);
  }

  template <typename V> static V load(const V* addr, ls_modifier mod)
  {
    return *addr;
  }
  template <typename V> static void store(V* addr, const V value,
      ls_modifier mod)
  {
    if (mod != WaW)
      log(addr, sizeof(V));
    *addr = value;
  }

public:
  static void memtransfer_static(void *dst, const void* src, size_t size,
      bool may_overlap, ls_modifier dst_mod, ls_modifier src_mod)
  {
    if (dst_mod != WaW && dst_mod != NONTXNAL)
      log(dst, size);
    if (!may_overlap)
      ::memcpy(dst, src, size);
    else
      ::memmove(dst, src, size);
  }

  static void memset_static(void *dst, int c, size_t size, ls_modifier mod)
  {
    if (mod != WaW)
      log(dst, size);
    ::memset(dst, c, size);
  }

  virtual bool trycommit() { return true; }
  // Local undo will handle this.
  // trydropreference() need not be changed either.
  virtual void rollback(gtm_transaction_cp *cp) { }
  virtual void reinit() { }
  virtual void fini() { }

  CREATE_DISPATCH_METHODS(virtual, )
  CREATE_DISPATCH_METHODS_MEM()

  serial_dispatch() : abi_dispatch(false, true, false, true) { }
};

} // anon namespace

static const serialirr_dispatch o_serialirr_dispatch;
static const serial_dispatch o_serial_dispatch;

abi_dispatch *
GTM::dispatch_serialirr ()
{
  return const_cast<serialirr_dispatch *>(&o_serialirr_dispatch);
}

abi_dispatch *
GTM::dispatch_serial ()
{
  return const_cast<serial_dispatch *>(&o_serial_dispatch);
}

// Put the transaction into serial-irrevocable mode.

void
GTM::gtm_transaction::serialirr_mode ()
{
  struct abi_dispatch *disp = abi_disp ();
  bool need_restart = true;

  if (this->state & STATE_SERIAL)
    {
      if (this->state & STATE_IRREVOCABLE)
	return;

      // Given that we're already serial, the trycommit better work.
      bool ok = disp->trycommit ();
      assert (ok);
      disp->fini ();
      need_restart = false;
    }
  else if (serial_lock.write_upgrade ())
    {
      this->state |= STATE_SERIAL;
      if (disp->trycommit ())
	{
	  disp->fini ();
	  need_restart = false;
	}
    }

  if (need_restart)
    restart (RESTART_SERIAL_IRR);
  else
    {
      this->state |= (STATE_SERIAL | STATE_IRREVOCABLE);
      set_abi_disp (dispatch_serial ());
    }
}

void ITM_REGPARM
_ITM_changeTransactionMode (_ITM_transactionState state)
{
  assert (state == modeSerialIrrevocable);
  gtm_tx()->serialirr_mode ();
}
