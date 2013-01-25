/* { dg-do run } */
/* { dg-options " "} */

#if HAVE_IO
#include <cstdio>
#endif
#include <cassert>

const int n = 8;

float x[n], y[n], z[n];

int main() {
    for( int i=1; i<=5; i+=4 ) {
        x[0:n] = 3;
        y[0:n] = i;
        z[0:n] = 0;
        assert( __sec_reduce_add(x[0:n])==3*n );
        assert( __sec_reduce_add(y[0:n])==i*n );
        assert( __sec_reduce_add(z[0:n])==0 );

        if (x[0:n] >= y[0:n]) {  
            z[0:n] = x[0:n] - y[0:n];
        } else {   
            z[0:n] = x[0:n] + y[0:n];
        }
#if HAVE_IO
        printf("x\ty\tz\n");
        for( size_t k=0; k<n; ++k ) {
            printf("%g\t%g\t%g\n",x[k],y[k],z[k]);
        }
#endif
        assert( __sec_reduce_add(x[0:n])==3*n );
        assert( __sec_reduce_add(y[0:n])==i*n );
        assert( __sec_reduce_add(z[0:n])==(3>=i?3-i:3+i)*n );
    }
    return 0;
}
