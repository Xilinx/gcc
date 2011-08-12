/* Copyright (C) 2008, 2009, 2011 Free Software Foundation, Inc.
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

void
GTM::gtm_transaction::decide_retry_strategy (gtm_restart_reason r)
{
  struct abi_dispatch *disp = abi_disp ();

  this->restart_reason[r]++;
  this->restart_total++;

  bool retry_irr = (r == RESTART_SERIAL_IRR);
  bool retry_serial = (this->restart_total > 100 || retry_irr);

  if (r == RESTART_CLOSED_NESTING)
    retry_serial = true;

  if (retry_serial)
    {
      // In serialirr_mode we can succeed with the upgrade to
      // write-lock but fail the trycommit.  In any case, if the
      // write lock is not yet held, grab it.  Don't do this with
      // an upgrade, since we've no need to preserve the state we
      // acquired with the read.
      // FIXME this might be dangerous if we use serial mode to change TM
      // meta data (e.g., reallocate the lock array). Likewise, for
      // privatization, we must get rid of old references (that is, abort)
      // or let privatizers know we're still there by not releasing the lock.
      if ((this->state & STATE_SERIAL) == 0)
	{
	  this->state |= STATE_SERIAL;
	  serial_lock.read_unlock ();
	  serial_lock.write_lock ();
	}

      // ??? We can only retry with dispatch_serial when the transaction
      // doesn't contain an abort.
      if ((this->prop & pr_hasNoAbort) && (r != RESTART_CLOSED_NESTING))
	retry_irr = true;
    }

  if (retry_irr)
    {
      this->state = (STATE_SERIAL | STATE_IRREVOCABLE);
      disp->fini ();
      disp = dispatch_serialirr ();
      set_abi_disp (disp);
    }
  else
    {
      disp = dispatch_serial();
      set_abi_disp (disp);
    }
}


// Decides which TM method should be used on the first attempt to run this
// transaction.
// serial_lock will not have been acquired if this is the outer-most
// transaction. If the state is set to STATE_SERIAL, the caller will set the
// dispatch.
GTM::abi_dispatch*
GTM::gtm_transaction::decide_begin_dispatch (uint32_t prop)
{
  // ??? Probably want some environment variable to choose the default
  // STM implementation once we have more than one implemented.
  if (prop & pr_hasNoAbort)
    return dispatch_serialirr_onwrite();
  state = STATE_SERIAL;
  if (prop & pr_hasNoAbort)
    state |= STATE_IRREVOCABLE;
  return 0;
}
