/* Please run this in the following way:

   time ./a.out

   The output should be something like this:

   real    0m10.002s     <== This is the main thing that should match. 
   user    0m10.793s
   sys     0m53.855s

*/


#include <unistd.h>
void mysleep(int a) 
{
  sleep(5);
}

int main(int argc, char ** argv) 
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
