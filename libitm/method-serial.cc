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

namespace GTM HIDDEN {

gtm_cacheline_mask gtm_dispatch::mask_sink;

const gtm_cacheline *
gtm_dispatch::read_lock(const gtm_cacheline *addr, lock_type)
{
  return addr;
}

gtm_dispatch::mask_pair
gtm_dispatch::write_lock(gtm_cacheline *addr, lock_type)
{
  return mask_pair (addr, &mask_sink);
}

} // namespace GTM

// Avoid a dependency on libstdc++ for the pure virtuals in gtm_dispatch.
extern "C" void HIDDEN
__cxa_pure_virtual ()
{
  abort ();
}

using namespace GTM;

namespace {

class serial_dispatch : public gtm_dispatch
{
 public:
  serial_dispatch() : gtm_dispatch(false, true) { }

  // The read_lock and write_lock methods are implented by the base class.

  virtual bool trycommit() { return true; }
  virtual void rollback() { abort(); }
  virtual void reinit() { }
  virtual void fini() { }
};

} // anon namespace

static const serial_dispatch o_serial_dispatch;

gtm_dispatch *
GTM::dispatch_serial ()
{
  return const_cast<serial_dispatch *>(&o_serial_dispatch);
}

// Put the transaction into serial-irrevocable mode.

void
GTM::gtm_transaction::serialirr_mode ()
{
  struct gtm_dispatch *disp = gtm_disp ();
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
      set_gtm_disp (dispatch_serial ());
    }
}

void ITM_REGPARM
_ITM_changeTransactionMode (_ITM_transactionState state)
{
  assert (state == modeSerialIrrevocable);
  gtm_tx()->serialirr_mode ();
}
