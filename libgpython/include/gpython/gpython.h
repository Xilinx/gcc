/* This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef __GCC_GPYTHON_H__
#define __GCC_GPYTHON_H__

#if __STDC_VERSION__ < 199901L
# if __GNUC__ >= 2
#  define __func__ __FUNCTION__
# else
#  define __func__ "<unknown>"
# endif
#endif

/* Abstract out some useful compiler attributes from GCC */
#if defined( GUNC )
# define __gpy_fmt_check( x, y )		\
  __attribute__ ((format (printf, x, y)))
# define __gpy_no_return			\
  __attribute__ ((noreturn))
# define __gpy_malloc				\
  __attribute__ ((malloc))
# define __gpy_pure				\
  __attribute__ ((pure))
# define __gpy_unused				\
  __attribute__ ((unused))
# define __gpy_nonnull				\
  __attribute__((nonnull)) 
#else
# define __gpy_fmt_check( x, y )
# define __gpy_no_return
# define __gpy_malloc
# define __gpy_pure
# define __gpy_unused
# define __gpy_nonnull
#endif

typedef struct gpy_rr_object_state_t {
  char * obj_t_ident;
  signed long ref_count;
  void * self;
} gpy_rr_object_state_t ;

typedef gpy_rr_object_state_t * gpy_object_state_t;

typedef gpy_object_state_t (*binary_op)( gpy_object_state_t,
					 gpy_object_state_t );

enum GPY_LIT_T { TYPE_INTEGER, TYPE_STRING };

typedef struct gpy_rr_literal_t {
  enum GPY_LIT_T;
  union {
    int integer;
    char * string;
    /* ... */
  }
} gpy_literal_t ;

typedef struct gpy_number_prot_t
{
  bool init;

  binary_op n_add;
  binary_op n_sub;
  binary_op n_div;
  binary_op n_mul;
  binary_op n_pow;

  binary_op n_let;
  binary_op n_lee;
  binary_op n_get;
  binary_op n_gee;
  binary_op n_eee;
  binary_op n_nee;
  binary_op n_orr;
  binary_op n_and;

} gpy_num_prot_t ;

typedef struct gpy_type_obj_def_t {
  char * identifier;
  size_t builtin_type_size;
  void * (*init_hook)( gpy_object_state_t );
  void (*destroy_hook)( void * );
  void (*print_hook)( void * , FILE * , bool );
  const struct gpy_number_prot_t * binary_protocol;
} gpy_type_obj_def_t ;

#define gpy_assert( expr )						\
  ((expr) ? (void) 0 : gpy_assertion_failed( #expr, __LINE__,		\
					     __FILE__, __func__ ));

#define gpy_free( x ) \
  gpy_assert( x );    \
  free( x );	      \
  x = NULL;

#ifdef DEBUG
extern void
__gpy_debug__( const char *, unsigned ,
               const char *, const char *, ... )
  __gpy_fmt_check(4,5) ;
#endif

extern void
__gpy_error__( const char *, unsigned ,
               const char *, const char *, ... )
  __gpy_fmt_check(4,5) ;

extern void
__gpy_fatal__( const char *, unsigned ,
               const char *, const char *, ... )
  __gpy_fmt_check(4,5) ;

#ifdef DEBUG
# define debug( ... )						\
  __gpy_debug__( __FILE__, __LINE__, __func__, __VA_ARGS__ );
#else
# define debug( ... )
#endif

#define fatal( ... )						\
  __gpy_fatal__( __FILE__, __LINE__, __func__, __VA_ARGS__ );

#define error( ... )						\
  __gpy_error__( __FILE__, __LINE__, __func__, __VA_ARGS__ );


extern void gpy_assertion_failed( const char * , unsigned , const char * ,
				  const char * );

extern void gpy_rr_init_runtime( void );
extern gpy_object_state_t gpy_rr_fold_integer( int );

extern void gpy_obj_integer_mod_init( void );

#endif //__GCC_GPYTHON_H__
