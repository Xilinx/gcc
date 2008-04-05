/* Implementation header for mudflap runtime library.
   Mudflap: narrow-pointer bounds-checking by tree rewriting.
   Copyright (C) 2002, 2003, 2004, 2005 Free Software Foundation, Inc.
   Contributed by Frank Ch. Eigler <fche@redhat.com>
   and Graydon Hoare <graydon@redhat.com>

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

In addition to the permissions in the GNU General Public License, the
Free Software Foundation gives you unlimited permission to link the
compiled version of this file into combinations with other programs,
and to distribute those combinations without any restriction coming
from the use of this file.  (The General Public License restrictions
do apply in other respects; for example, they cover modification of
the file, and distribution when not linked into a combine
executable.)

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.  */

/* Public libbounds declarations -*- C -*- */

#ifndef BOUNDS_RUNTIME_H
#define BOUNDS_RUNTIME_H

typedef void *__bounds_ptr_t;
typedef unsigned int __bounds_uintptr_t __attribute__ ((__mode__ (__pointer__)));
typedef __SIZE_TYPE__ __bounds_size_t;

/* Codes to describe the type of access to check: __bounds_check arg 3 */

#define __MF_CHECK_READ 0
#define __MF_CHECK_WRITE 1

/* Codes to describe a region of memory being registered: __bounds_*register arg 3 */

#define __MF_TYPE_HEAP 1
#define __MF_TYPE_HEAP_I 2
#define __MF_TYPE_STACK 3
#define __MF_TYPE_STATIC 4
#define __MF_TYPE_GUESS 5

/* Codes to describe the type of comparison being performed: __bounds_comp arg 3 */

#define __BOUNDS_COMP_LT 0
#define __BOUNDS_COMP_LE 1
#define __BOUNDS_COMP_GT 2
#define __BOUNDS_COMP_GE 3
#define __BOUNDS_COMP_EQ 4
#define __BOUNDS_COMP_NE 5

/* The public bounds-checking API */

#ifdef __cplusplus
extern "C" {
#endif

extern void __bounds_check (void* referent, void *ptr, __bounds_size_t sz, 
       int type, const char *location) __attribute((nothrow));
extern void *__bounds_arith (void *base, void *addr, 
       const char *location) __attribute((nothrow));
extern int __bounds_comp (void *left, void *right, int op_type,
       const char *location) __attribute((nothrow));
extern void *__bounds_cast (void *ptr, 
       const char *location) __attribute((nothrow));

extern void __bounds_register (void *ptr, __bounds_size_t sz, int type, 
       const char *name) __attribute((nothrow));
extern void __bounds_unregister (void *ptr, __bounds_size_t sz, int type)
       __attribute((nothrow));

extern void * __bounds_create_oob (void* ptr, void* old_ptr, 
       const char* location) __attribute((nothrow));

/* Redirect some standard library functions to libmudflap.  These are
   done by simple #define rather than linker wrapping, since only
   instrumented modules are meant to be affected.  */

#ifdef __BOUNDS_CHECKING_ON
#pragma redefine_extname memcpy __boundswrap_memcpy
#pragma redefine_extname memmove __boundswrap_memmove
#pragma redefine_extname memset __boundswrap_memset
#pragma redefine_extname memcmp __boundswrap_memcmp
#pragma redefine_extname memchr __boundswrap_memchr
#pragma redefine_extname memrchr __boundswrap_memrchr
#pragma redefine_extname strcpy __boundswrap_strcpy
#pragma redefine_extname strncpy __boundswrap_strncpy
#pragma redefine_extname strcat __boundswrap_strcat
#pragma redefine_extname strncat __boundswrap_strncat
#pragma redefine_extname strcmp __boundswrap_strcmp
#pragma redefine_extname strcasecmp __boundswrap_strcasecmp
#pragma redefine_extname strncmp __boundswrap_strncmp
#pragma redefine_extname strncasecmp __boundswrap_strncasecmp
#pragma redefine_extname strdup __boundswrap_strdup
#pragma redefine_extname strndup __boundswrap_strndup
#pragma redefine_extname strchr __boundswrap_strchr
#pragma redefine_extname strrchr __boundswrap_strrchr
#pragma redefine_extname strstr __boundswrap_strstr
#pragma redefine_extname memmem __boundswrap_memmem
#pragma redefine_extname strlen __boundswrap_strlen
#pragma redefine_extname strnlen __boundswrap_strnlen
#pragma redefine_extname bzero __boundswrap_bzero
#pragma redefine_extname bcopy __boundswrap_bcopy
#pragma redefine_extname bcmp __boundswrap_bcmp
#pragma redefine_extname index __boundswrap_index
#pragma redefine_extname rindex __boundswrap_rindex
#pragma redefine_extname asctime __boundswrap_asctime
#pragma redefine_extname ctime __boundswrap_ctime
#pragma redefine_extname gmtime __boundswrap_gmtime
#pragma redefine_extname localtime __boundswrap_localtime
#pragma redefine_extname time __boundswrap_time
#pragma redefine_extname strerror __boundswrap_strerror
#pragma redefine_extname fopen __boundswrap_fopen
#pragma redefine_extname fdopen __boundswrap_fdopen
#pragma redefine_extname freopen __boundswrap_freopen
#pragma redefine_extname fclose __boundswrap_fclose
#pragma redefine_extname fread __boundswrap_fread
#pragma redefine_extname fwrite __boundswrap_fwrite
#pragma redefine_extname fgetc __boundswrap_fgetc
#pragma redefine_extname fgets __boundswrap_fgets
#pragma redefine_extname getc __boundswrap_getc
#pragma redefine_extname gets __boundswrap_gets
#pragma redefine_extname ungetc __boundswrap_ungetc
#pragma redefine_extname fputc __boundswrap_fputc
#pragma redefine_extname fputs __boundswrap_fputs
#pragma redefine_extname putc __boundswrap_putc
#pragma redefine_extname puts __boundswrap_puts
#pragma redefine_extname clearerr __boundswrap_clearerr
#pragma redefine_extname feof __boundswrap_feof
#pragma redefine_extname ferror __boundswrap_ferror
#pragma redefine_extname fileno __boundswrap_fileno
#pragma redefine_extname printf __boundswrap_printf
#pragma redefine_extname fprintf __boundswrap_fprintf
#pragma redefine_extname sprintf __boundswrap_sprintf
#pragma redefine_extname snprintf __boundswrap_snprintf
#pragma redefine_extname vprintf __boundswrap_vprintf
#pragma redefine_extname vfprintf __boundswrap_vfprintf
#pragma redefine_extname vsprintf __boundswrap_vsprintf
#pragma redefine_extname vsnprintf __boundswrap_vsnprintf
#pragma redefine_extname access __boundswrap_access
#pragma redefine_extname remove __boundswrap_remove
#pragma redefine_extname fflush __boundswrap_fflush
#pragma redefine_extname fseek __boundswrap_fseek
#pragma redefine_extname ftell __boundswrap_ftell
#pragma redefine_extname rewind __boundswrap_rewind
#pragma redefine_extname fgetpos __boundswrap_fgetpos
#pragma redefine_extname fsetpos __boundswrap_fsetpos
#pragma redefine_extname stat __boundswrap_stat
#pragma redefine_extname fstat __boundswrap_fstat
#pragma redefine_extname lstat __boundswrap_lstat
#pragma redefine_extname mkfifo __boundswrap_mkfifo
#pragma redefine_extname setvbuf __boundswrap_setvbuf
#pragma redefine_extname setbuf __boundswrap_setbuf
#pragma redefine_extname setbuffer __boundswrap_setbuffer
#pragma redefine_extname setlinebuf __boundswrap_setlinebuf
#pragma redefine_extname opendir __boundswrap_opendir
#pragma redefine_extname closedir __boundswrap_closedir
#pragma redefine_extname readdir __boundswrap_readdir
#pragma redefine_extname recv __boundswrap_recv
#pragma redefine_extname recvfrom __boundswrap_recvfrom
#pragma redefine_extname recvmsg __boundswrap_recvmsg
#pragma redefine_extname send __boundswrap_send
#pragma redefine_extname sendto __boundswrap_sendto
#pragma redefine_extname sendmsg __boundswrap_sendmsg
#pragma redefine_extname setsockopt __boundswrap_setsockopt
#pragma redefine_extname getsockopt __boundswrap_getsockopt
#pragma redefine_extname accept __boundswrap_accept
#pragma redefine_extname bind __boundswrap_bind
#pragma redefine_extname connect __boundswrap_connect
#pragma redefine_extname gethostname __boundswrap_gethostname
#pragma redefine_extname sethostname __boundswrap_sethostname
#pragma redefine_extname gethostbyname __boundswrap_gethostbyname
#pragma redefine_extname wait __boundswrap_wait
#pragma redefine_extname waitpid __boundswrap_waitpid
#pragma redefine_extname popen __boundswrap_popen
#pragma redefine_extname pclose __boundswrap_pclose
#pragma redefine_extname execve __boundswrap_execve
#pragma redefine_extname execv __boundswrap_execv
#pragma redefine_extname execvp __boundswrap_execvp
#pragma redefine_extname system __boundswrap_system
#pragma redefine_extname dlopen __boundswrap_dlopen
#pragma redefine_extname dlerror __boundswrap_dlerror
#pragma redefine_extname dlsym __boundswrap_dlsym
#pragma redefine_extname dlclose __boundswrap_dlclose
#pragma redefine_extname fopen64 __boundswrap_fopen64
#pragma redefine_extname freopen64 __boundswrap_freopen64
#pragma redefine_extname stat64 __boundswrap_stat64
#pragma redefine_extname fseeko64 __boundswrap_fseeko64
#pragma redefine_extname ftello64 __boundswrap_ftello64
#pragma redefine_extname semop __boundswrap_semop
#pragma redefine_extname semctl __boundswrap_semctl
#pragma redefine_extname shmctl __boundswrap_shmctl
#pragma redefine_extname shmat __boundswrap_shmat
#pragma redefine_extname shmdt __boundswrap_shmdt
#pragma redefine_extname __ctype_b_loc __boundswrap___ctype_b_loc
#pragma redefine_extname __ctype_toupper_loc __boundswrap___ctype_toupper_loc
#pragma redefine_extname __ctype_tolower_loc __boundswrap___ctype_tolower_loc
#pragma redefine_extname getlogin __boundswrap_getlogin
#pragma redefine_extname cuserid __boundswrap_cuserid
#pragma redefine_extname getpwnam __boundswrap_getpwnam
#pragma redefine_extname getpwuid __boundswrap_getpwuid
#pragma redefine_extname getgrnam __boundswrap_getgrnam
#pragma redefine_extname getgrgid __boundswrap_getgrgid
#pragma redefine_extname getservent __boundswrap_getservent
#pragma redefine_extname getservbyname __boundswrap_getservbyname
#pragma redefine_extname getservbyport __boundswrap_getservbyport
#pragma redefine_extname gai_strerror __boundswrap_gai_strerror
#pragma redefine_extname getmntent __boundswrap_getmntent
#pragma redefine_extname inet_ntoa __boundswrap_inet_ntoa
#pragma redefine_extname getprotoent __boundswrap_getprotoent
#pragma redefine_extname getprotobyname __boundswrap_getprotobyname
#pragma redefine_extname getprotobynumber __boundswrap_getprotobynumber

/* Disable glibc macros.  */
#define __NO_STRING_INLINES

#endif /* __BOUNDS_CHECKING_ON */


#ifdef __cplusplus
}
#endif

#endif /* BOUNDS_RUNTIME_H */
