// { dg-do compile }

int g;
void f(void)
{
  __transaction {	// { dg-error "without transactional memory" }
    g++;
  }
}
