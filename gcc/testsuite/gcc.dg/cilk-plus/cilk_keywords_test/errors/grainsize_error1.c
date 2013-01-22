void check() { 
  int i, a[256]; 
#pragma cilk grainsize=cilk /* { dg-error "undeclared" } */
  _Cilk_for(i = 0; i < 256; i++) {
    a[i] = i;
  } 
}

