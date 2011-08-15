/* lock_guard.h                  -*-C++-*-
 *
 *************************************************************************
 *
 * Copyright (C) 2011 
 * Intel Corporation
 * 
 * This file is part of the Intel Cilk Plus Library.  This library is free
 * software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * Under Section 7 of GPL version 3, you are granted additional
 * permissions described in the GCC Runtime Library Exception, version
 * 3.1, as published by the Free Software Foundation.
 * 
 * You should have received a copy of the GNU General Public License and
 * a copy of the GCC Runtime Library Exception along with this program;
 * see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 **************************************************************************
 *
 * Lock guard patterned after the std::lock_guard class template proposed in
 * the C++ 0x draft standard.
 *
 * An object of type lock_guard controls the ownership of a mutex object
 * within a scope. A lock_guard object maintains ownership of a mutex object
 * throughout the lock_guard object's lifetime. The behavior of a program is
 * undefined if the mutex referenced by pm does not exist for the entire
 * lifetime of the lock_guard object.
 */

#ifndef LOCK_GUARD_H_INCLUDED
#define LOCK_GUARD_H_INCLUDED

#include <cilk/cilk.h>

namespace cilkscreen
{
    template <class Mutex>
    class lock_guard
    {
    public:
        typedef Mutex mutex_type;

        explicit lock_guard(mutex_type &m) : pm(m)
        {
            pm.lock();
            locked = true;
        }

        ~lock_guard()
        {
            locked = false;
            pm.unlock();
        }

    private:
        lock_guard(lock_guard const&);
        lock_guard& operator=(lock_guard const&);

    private:
        // exposition only:
        mutex_type &pm;
        bool locked;
    };
}

#endif  // LOCK_GUARD_H_INCLUDED
