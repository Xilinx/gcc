#include <string.h>

#include <iostream>
using std::cout;
using std::endl;

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
    cout << "This program is called " << progname << endl;

    proglen = len(progname);
    cout << "That is a word of length " << proglen << endl;

	return 0;
}

