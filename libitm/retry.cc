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

void
GTM::gtm_transaction::decide_retry_strategy (gtm_restart_reason r)
{
  struct abi_dispatch *disp = abi_disp ();

  this->restart_reason[r]++;
  this->restart_total++;

  bool retry_irr = (r == RESTART_SERIAL_IRR);
  bool retry_serial = (this->restart_total > 100 || retry_irr);

  if (retry_serial)
    {
      // In serialirr_mode we can succeed with the upgrade to
      // write-lock but fail the trycommit.  In any case, if the
      // write lock is not yet held, grab it.  Don't do this with
      // an upgrade, since we've no need to preserve the state we
      // acquired with the read.
      if ((this->state & STATE_SERIAL) == 0)
	{
	  this->state |= STATE_SERIAL;
	  serial_lock.read_unlock ();
	  serial_lock.write_lock ();
	}

      // ??? We can only retry with dispatch_serial when the transaction
      // doesn't contain an abort.  TODO: Create a serial mode dispatch
      // that does logging in order to support abort.
      if (this->prop & pr_hasNoAbort)
	retry_irr = true;
    }

  if (retry_irr)
    {
      this->state = (STATE_SERIAL | STATE_IRREVOCABLE);
      disp->fini ();
      disp = dispatch_serial ();
      set_abi_disp (disp);
    }
  else
    {
      if (r == RESTART_NOT_READONLY)
	{
	  assert ((this->prop & pr_readOnly) == 0);
	  if (disp->read_only ())
	    {
	      disp->fini ();
	      disp = dispatch_wbetl ();
	      set_abi_disp (disp);
	      return;
	    }
	}
      disp->reinit ();
    }
}
