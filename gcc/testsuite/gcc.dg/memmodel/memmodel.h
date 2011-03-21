int memmodel_fini = 0;

void __attribute__((noinline))
memmodel_done ()
{
  memmodel_fini = 1;
}
