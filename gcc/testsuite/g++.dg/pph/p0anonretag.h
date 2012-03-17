#ifndef P0ANONRETAG_H
#define P0ANONRETAG_H

typedef struct S1
{
  unsigned long s1;
  struct S1 *s2;
  char *s3;
} S1;

typedef struct
{
  unsigned int s4;
  unsigned int s5;
  int s6;
  unsigned int *s7;
} S2;

#endif
