#include <stdio.h>
#include <string.h>

char *progname;       /* program name */
int proglen;          /* length of program name */

int len(char *program)
{
    char *name;
    int length;

    name = program;
    length = strlen(name);

    return length;
}

int main (int argc, char **argv)
{
    progname = argv[0];
    printf("This program is called %s\n", progname);

    proglen = len(progname);
    printf("That is a word of length %d\n", proglen);

	return 0;
}

