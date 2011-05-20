#ifndef C1FUNCSTATIC_H
#define C1FUNCSTATIC_H
static int f() {
    static int x = 3;
    return x++;
}
#endif
