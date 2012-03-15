#ifndef X0TMPLDFLTPARM_H
#define X0TMPLDFLTPARM_H

template< typename T >
struct auxillary
{
};

template< typename T, typename U = auxillary< T > >
struct primary
{
};

typedef primary< int > handlei;
typedef primary< long > handlel;

#endif
