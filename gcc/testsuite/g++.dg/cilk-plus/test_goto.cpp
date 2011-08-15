int main(int argc, char **argv)
{
  int x = 0;
  if (argc == 1)
  goto bye;
  x = 5;
bye:
  return x;
}
