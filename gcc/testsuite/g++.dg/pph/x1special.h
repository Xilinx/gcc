#ifndef X1SPECIAL_H
#define X1SPECIAL_H

struct B {
    B( int );
    B( double );
    B( const B& );
    B& operator=( const B& );
    B& operator=( int );
    operator double();
    ~B();
};

struct D {
    D( const B& );
};

int F( D );
int G( double );

#endif
