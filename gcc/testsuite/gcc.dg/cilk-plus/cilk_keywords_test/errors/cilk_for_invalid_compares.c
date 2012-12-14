/* <feature>
     The operator denoted OP shall be one of !=, <=, <, >=, or >
   </feature>
*/

#define ARRAY_SIZE 10000

int a[ARRAY_SIZE];

int main (void)
{
    int ii;
    _Cilk_for(ii = 0; ii == ARRAY_SIZE; ii++) /* { dg-error "_Cilk_for condition must be one of the" } */
    {
	a[ii] = ii;
    }
}
