#include <iostream>
using std::cout;
using std::endl;

char *progname;       /* program name */

int main (int argc, char **argv)
{
    progname = argv[3];
    cout << "This program is called " << progname << endl;

	return 0;
}

/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
