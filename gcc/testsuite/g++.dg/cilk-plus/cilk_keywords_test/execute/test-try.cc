/* { dg-do run } */
/* { dg-options "-ldl -lcilkrts" } */



/* Please run this in the following way:

   time ./a.out

   The output should be something like this:

   real    0m10.002s     <== This is the main thing that should match. 
   user    0m10.793s
   sys     0m53.855s

*/


#include <sys/time.h>
#include <cstdlib>
#include <unistd.h>
int main2(int argc, char ** argv);
void mysleep(int a);
void mysleep(int a) 
{
  sleep(5);
}

int main2(int argc, char ** argv) 
{
  _Cilk_spawn mysleep(5); 
  try 
    { 
      _Cilk_spawn mysleep(5); 
      mysleep(5);
    }
    catch(int err) 
      {
      } 
}

int main (int argc, char **argv)
{
  struct timeval tp_start, tp_end;
  double sec, usec, start, end;
  int int_time;
  gettimeofday (&tp_start, NULL);
  main2 (argc, argv);
  gettimeofday (&tp_end, NULL);

  sec = static_cast<double> (tp_start.tv_sec);
  usec= static_cast<double> (tp_start.tv_usec)/1E6;
  start = sec + usec;

  sec = static_cast<double> (tp_end.tv_sec);
  usec= static_cast<double> (tp_end.tv_usec)/1E6;
  end = sec + usec;
  
  int_time = (int) (end-start);
  if (int_time != 10)
    abort ();

  return 0;
}

