/* This test will create 1 clone of the function below, just one for mask
 * for the pentium4 processor.
 */
#ifdef __x86__
#define My_Type float
__attribute__ ((vector(vectorlength(4), mask, processor (pentium_4), linear(y), uniform (x))))
My_Type ef_add (My_Type x, My_Type y) 

{
  return x + y;
}
#endif
