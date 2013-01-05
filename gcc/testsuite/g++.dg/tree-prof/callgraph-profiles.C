/* Verify if call-graph profile sections are created
   with -freorder-functions=. */
/* { dg-require-section-exclude "" } */
/* { dg-require-linker-function-reordering-plugin "" } */
/* { dg-options "-O2 -freorder-functions=callgraph -ffunction-sections --save-temps -Wl,-plugin-opt,file=callgraph-profiles.C.dump -Wl,-plugin-opt,split_segment=yes" } */

int
notcalled ()
{
  return 0;
}

int __attribute__ ((noinline))
foo ()
{
  return 1;
}

int __attribute__ ((noinline))
bar ()
{
  return 0;
}

int main ()
{
  int sum;
  for (int i = 0; i< 1000; i++)
    {
      sum = foo () + bar();
    }
  return sum * bar ();
}

/* { dg-final-use { scan-assembler "\.gnu\.callgraph\.text\.main" } } */
/* { dg-final-use { scan-assembler "\.string \"1000\"" } } */
/* { dg-final-use { scan-file callgraph-profiles.C.dump "Callgraph group : main _Z3barv _Z3foov\n" } }  */
/* { dg-final-use { scan-file callgraph-profiles.C.dump "\.text\.*\.main\n.text\.*\._Z3barv\n\.text\.*\._Z3foov\n\.text\.*\._Z9notcalledv" } }  */
/* { dg-final-use { scan-file callgraph-profiles.C.dump "Moving 1 section\\(s\\) to new segment" } }  */
/* { dg-final-use { cleanup-saved-temps } }  */
/* { dg-final-use { remove-build-file "callgraph-profiles.C.dump" } }  */
