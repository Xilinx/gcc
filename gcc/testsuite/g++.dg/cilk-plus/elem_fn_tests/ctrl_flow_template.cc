/* This test will create 2 clones of the function below, one for mask and one 
   without the mask
 */

template <class My_Type>
__attribute__ ((vector(vectorlength(4), processor (core_i7_sse4_2), uniform (x,y))))
My_Type ef_add (My_Type x, My_Type y) 

{
  if (x > 0) {
    if (y > 0) {
      return x + y;
    }
    else
      return (x-y);
  }
  else
    return 0;
}

int main(int argc, char **argv)
{
  return ef_add<int> (argc, argc); 
}

