#ifndef A0INTEGER_H
#define A0INTEGER_H

class integer
{
    int value;
public:
    integer()
        : value(0) { }
    integer( int v )
        : value(v) { }
    integer( const integer& other )
        : value( other.value ) { }
    integer& operator=( const integer& other )
        { value = other.value; return *this; }
    operator int() const
        { return value; }
    bool operator <( const integer& other ) const
        { return value < other.value; }
};

#endif
