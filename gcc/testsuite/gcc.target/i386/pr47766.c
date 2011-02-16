/* { dg-do compile } */
/* { dg-options "-O2 -fstack-protector" } */

int
parse_opt (int key)
{
   struct
   {
     int arg[key];
   } reqdata;
  return 0;
}
