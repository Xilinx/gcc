#include "x1dynarray1.h"
#include <algorithm>

void lowrap(       dynarray< int > & target,
             const dynarray< int > & source )
{
    dynarray< int > sorted( source );
    std::sort( sorted.begin(), sorted.end() );
    const int* srt = &sorted.front();
    dynarray< int >::iterator tgt( target.begin() );
    for ( ; tgt != target.end(); tgt++ ) {
        *tgt = *srt;
        if ( srt == &sorted.back() )
            srt = &sorted.front();
        else
            srt++;
    }
}

int main() {
    dynarray< int > alpha(8);
    dynarray< int > gamma(3);
    for ( dynarray< int >::size_type i = 0; i < gamma.size(); i++ )
	gamma[i] = 4 - i;
    lowrap( alpha, gamma );
    int sum = 0;
    for ( dynarray< int >::size_type i = 0; i < alpha.size(); i++ )
	sum += alpha.at(i);
    return sum - 23;
}
