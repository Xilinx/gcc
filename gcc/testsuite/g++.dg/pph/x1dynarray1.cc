// { dg-xfail-if "ICE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "internal compiler error: in resume_scope" "" { xfail *-*-* } 0 }
// { dg-bogus "from .*" "" { xfail *-*-* } 0 }
// { dg-bogus "fails macro validation " "" { xfail *-*-* } 0 }
// { dg-prune-output "In file included from " }

#include "x1dynarray1.h"
#include <iostream>
#include <algorithm>

void lowrap(       std::dynarray< int > & target,
             const std::dynarray< int > & source )
{
    std::dynarray< int > sorted( source );
    std::sort( sorted.begin(), sorted.end() );
    const int* srt = &sorted.front();
    std::dynarray< int >::iterator tgt( target.begin() );
    for ( ; tgt != target.end(); tgt++ ) {
        *tgt = *srt;
        if ( srt == &sorted.back() )
            srt = &sorted.front();
        else
            srt++;
    }
}

int main() {
    std::dynarray< int > alpha(8);
    std::dynarray< int > gamma(3);
    for ( std::dynarray< int >::size_type i = 0; i < gamma.size(); i++ )
	gamma[i] = 4 - i;
    lowrap( alpha, gamma );
    int sum = 0;
    for ( std::dynarray< int >::size_type i = 0; i < alpha.size(); i++ )
	sum += alpha.at(i);
    return sum - 23;
}
