#include <stdio.h>

char *progname;       /* program name */

int main (int argc, char **argv)
{
    progname = argv[3];
    printf("This program is called %s\n", progname);

	return 0;
}

/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
