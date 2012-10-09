/* { dg-do compile } */
/* { dg-options "-O2 -Warray-bounds -Werror" } */

#include <string.h>

struct s1 {
 short type;
 char str[32];
};
struct s2 {
   char str[8];
   int count; 
   struct s1 f1;
};

void foo (struct s2 *v2, struct s1 *v1)
{
 int size;
 char *e = v2->str;

 strcpy(v1->str, "Q");
 size = 11 + strlen(v1->str);  /* 12 */
 v1 = (struct s1*) (e + size);
 v1->type = 1;
}
