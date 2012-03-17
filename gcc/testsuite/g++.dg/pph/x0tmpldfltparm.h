#ifndef X0TMPLDFLTPARM_H
#define X0TMPLDFLTPARM_H

template< typename A >
struct auxillary
{
};

template< typename P, typename Q = auxillary< P > >
struct primary
{
};

typedef primary< int > handlei;
typedef primary< long > handlel;

#endif
