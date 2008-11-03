/* { dg-do compile } */

int g;
void f(void)
{
  __tm_atomic {		/* { dg-error "without transactional memory" } */
    g++;
  }
}
