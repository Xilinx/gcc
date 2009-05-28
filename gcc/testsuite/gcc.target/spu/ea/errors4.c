/* Invalid __ea declarations.  */
/* { dg-do compile } */
/* { dg-options "-O2 -std=gnu99 -pedantic-errors" } */

__ea char ea_str[] = "abc";
char lm_str[] = "abc";

__ea char *lm_ea_ptr1 = "abc";				/* { dg-error "initializer element pointing to a named address space" } */
__ea char *lm_ea_ptr2 = (__ea char *)"abc";		/* { dg-error "initializer element pointing to a named address space" } */
__ea char *lm_ea_ptr3 = ea_str;				/* { dg-error "initializer element pointing to a named address space" } */
__ea char *lm_ea_ptr4 = (__ea char *)ea_str;		/* { dg-error "initializer element pointing to a named address space" } */
__ea char *lm_ea_ptr5 = lm_str;				/* { dg-error "initializer element pointing to a named address space" } */
__ea char *lm_ea_ptr6 = (__ea char *)lm_str;		/* { dg-error "initializer element pointing to a named address space" } */

__ea char * __ea ea_ea_ptr1 = ea_str;			/* { dg-error "initializer element is not allowed in named address space" } */
__ea char * __ea ea_ea_ptr2 = (__ea char *)ea_str;	/* { dg-error "initializer element is not allowed in named address space" } */

char * __ea ea_lm_ptr1 = lm_str;			/* { dg-error "initializer element is not allowed in named address space" } */
char * __ea ea_lm_ptr2 = (char *)lm_str;		/* { dg-error "initializer element is not allowed in named address space" } */

struct foo {
  int first;
  __ea char *ptr;
  int last;
};

__ea struct foo ea_struct1 = {
  10,
  (__ea char *)0,
  11,
};

__ea struct foo ea_struct2 = {
  20,
  0,
  21,
};

struct foo ea_struct3 = {
  30,
  ea_str,		/* { dg-error "(initializer element pointing to a named address space)|(near initialization)" "" } */
  31,
};

struct foo ea_struct4 = {
  40,
  (__ea char *)lm_str,	/* { dg-error "(initializer element pointing to a named address space)|(near initialization)" "" } */
  41,
};

struct bar {
  int first;
  char *ptr;
  int last;
};

__ea struct foo ea_struct5 = {
  50,
  0,
  51,
};

__ea struct foo ea_struct6 = {
  60,
  (char *)0,
  61,
};

__ea struct bar ea_struct7 = {
  70,
  lm_str,
  71,
};		/* { dg-error "(initializer element is not allowed in named address space)|(near initialization)" "" } */

struct bar lm_struct8 = {
  80,
  0,
  81,
};

struct bar lm_struct9 = {
  90,
  (char *)0,
  91,
};

struct bar lm_struct10 = {
  100,
  lm_str,
  101,
};
