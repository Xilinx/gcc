#ifndef X1FUNCSTATIC_H
#define X1FUNCSTATIC_H
static int f() {
    static int x = 3;
    return x++;
}
#endif
