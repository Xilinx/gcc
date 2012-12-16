/* <feature>
    The three items inside parentheses in the grammar, separated by semicolons,
     are the initialization, condition, and increment
   </feature>
*/

int a[5000];

void func ()
{
    int i, j = 0;
    _Cilk_for(i = 0; i < 5000 || j == 0; i++) /* { dg-error "_Cilk_for condition must be one of the" } */
    {
	a[i] = i;
    }
}
