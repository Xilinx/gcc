#ifndef A0KEYED_H
#define A0KEYED_H
struct keyed {
    int field;
    virtual int mix( int arg ) { return field ^ arg; }
    virtual int key( int arg );
    virtual ~keyed() { }
};
#endif
