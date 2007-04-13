main() 
{
  char buf[3];
  gets(buf);  /* { dg-warning "gets" } */
  return 0;
}
