#ifndef A0KEYNO_H
#define A0KEYNO_H
struct keyno {
    int field;
    virtual int mix( int arg ) { return field ^ arg; }
    virtual ~keyno() { }
};
#endif
