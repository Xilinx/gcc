int a[256];

void check() { 
    int i; 
#pragma cilk grainsize=*NULL  /* { dg-error "undeclared" } */
    _Cilk_for(i = 0; i < 256; i++) { 
	a[i] = i; 
    }
}


