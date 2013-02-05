/* { dg-options "-Wno-div-by-zero" } */

int a[256];

void check() { 
    int i; 
#pragma cilk grainsize=5/0 /* { dg-error "cannot convert grain" } */
    _Cilk_for(i = 0; i < 256; i++) { 
	a[i] = i; 
    }
}


