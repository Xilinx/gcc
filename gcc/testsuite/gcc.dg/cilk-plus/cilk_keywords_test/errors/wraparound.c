void check(int stride, int *a) { 
    int i; 
    _Cilk_for (i = 255; i > 0; i += stride)  /* This is OK!  */
      a[i] |= 4; 
    _Cilk_for (i = 255; i > 0; i += -1)      /* This is OK!  */
      a[i] |= 4; 
    _Cilk_for (i = 255; i > 0; i -= 1)       /* This is OK!  */
      a[i] |= 4; 
    _Cilk_for (i = 255; i > 0; i-- )         /* This is OK!  */
      a[i] |= 4; 
    _Cilk_for (i = 255; i > 0; --i )         /* This is OK!  */
      a[i] |= 4; 

    _Cilk_for (i = 255; i > 0; ++i )         /* { dg-error "greater than" } */
      a[i] |= 4; 
    _Cilk_for (i = 255; i > 0; i++ )         /* { dg-error "greater than" } */
      a[i] |= 4; 
    _Cilk_for (i = 255; i > 0; i +=2)        /* { dg-error "greater than" } */
      a[i] |= 4; 
    _Cilk_for (i = 255; i > 0; i -=-2)       /* { dg-error "greater than" } */
      a[i] |= 4; 
} 

