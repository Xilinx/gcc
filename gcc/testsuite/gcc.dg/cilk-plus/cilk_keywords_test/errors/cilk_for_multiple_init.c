/* <feature>
    The three items inside parentheses in the grammar, separated by semicolons,
     are the initialization, condition, and increment
   </feature>
*/

#define ARRAY_SIZE 50000
int a[ARRAY_SIZE];

void func()
{
    int i, j, k = 0;
    _Cilk_for(i = 0, j = 0; i < ARRAY_SIZE; i++) /* { dg-error "cannot have multiple initializations in" } */
    {
	a[i] = i;
    }
    _Cilk_for(i = 0, j = 0, k = 0; i < ARRAY_SIZE; i++) /* { dg-error "cannot have multiple initializations in" } */
    {
	a[i] = i;
    }
}
