// pph asm xdiff 26015
// xfail BOGUS DUPVAR

#include "c0variables3.h"
#include "c0variables4.h"

int main()
{
    return gbl_initial + gbl_init_const + gbl_manifest;
}
