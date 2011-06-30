/* Copyright (C) 2009 Free Software Foundation, Inc.
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

namespace {

using namespace GTM;

class readonly_dispatch : public abi_dispatch
{
 private:
  gtm_version m_start;

 public:
  readonly_dispatch();

  virtual const gtm_cacheline *read_lock(const gtm_cacheline *, lock_type);
  virtual mask_pair write_lock(gtm_cacheline *, lock_type);
  virtual bool trycommit();
  virtual void rollback();
  virtual void reinit();
  virtual void fini();
  virtual bool trydropreference (void *ptr, size_t size) { return true; }
};

inline
readonly_dispatch::readonly_dispatch()
  : abi_dispatch(true, true), m_start(gtm_get_clock ())
{ }


const gtm_cacheline *
readonly_dispatch::read_lock(const gtm_cacheline *line, lock_type lock)
{
  switch (lock)
    {
    case NOLOCK:
    case R:
    case RaR:
      return line;

    case RfW:
      gtm_tx()->restart (RESTART_NOT_READONLY);

    case RaW:
    default:
      abort ();
    }
}

abi_dispatch::mask_pair
readonly_dispatch::write_lock(gtm_cacheline *line, lock_type lock)
{
  switch (lock)
    {
    case NOLOCK:
      {
	abi_dispatch::mask_pair pair;
	pair.line = line;
	pair.mask = &mask_sink;
	return pair;
      }

    case WaW:
      abort ();

    default:
      gtm_tx()->restart (RESTART_NOT_READONLY);
    }
}

bool
readonly_dispatch::trycommit ()
{
  return gtm_get_clock () == m_start;
}

void
readonly_dispatch::rollback ()
{
  /* Nothing to do.  */
}

void
readonly_dispatch::reinit ()
{
  m_start = gtm_get_clock ();
}

void
readonly_dispatch::fini ()
{
  delete this;
}

} // anon namespace

abi_dispatch *
GTM::dispatch_readonly ()
{
  return new readonly_dispatch();
}
