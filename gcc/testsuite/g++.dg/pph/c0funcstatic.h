#ifndef C0FUNCSTATIC_H
#define C0FUNCSTATIC_H
static int f() {
    static int x = 3;
    return x++;
}
#endif
