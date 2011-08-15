/* fake_mutex.h                  -*-C++-*-
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
 * Cilkscreen fake mutexes are provided to indicate to the Cilkscreen race
 * detector that a race should be ignored.
 */

#ifndef FAKE_MUTEX_H_INCLUDED
#define FAKE_MUTEX_H_INCLUDED

#include <cilktools/cilkscreen.h>

// If this is Windows, specify the linkage
#ifdef _WIN32
#define CILKSCREEN_CDECL __cdecl
#else
#define CILKSCREEN_CDECL
#endif // _WIN32

namespace cilkscreen
{
    class fake_mutex
    {
    public:

        // Wait until mutex is available, then enter
        virtual void lock()
        {
            __cilkscreen_acquire_lock(&lock_val);
        }

        // A fake mutex is always available
        virtual bool try_lock() { lock(); return true; }

        // Releases the mutex
        virtual void unlock()
        {
            __cilkscreen_release_lock(&lock_val);
        }

    private:
        int lock_val;
    };

    // Factory function for fake mutex
    fake_mutex *CILKSCREEN_CDECL create_fake_mutex() { return new fake_mutex(); }

    // Destructor function for fake mutex - The mutex cannot be used after
    // calling this function
    void CILKSCREEN_CDECL destroy_fake_mutex(fake_mutex *m) { delete m; }

} // namespace cilk

#endif  // FAKE_MUTEX_H_INCLUDED
