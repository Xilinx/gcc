/* Define all the __sync verifications here.  Each size variation to be tested
   will define 'TYPE' and then include this file.  */

extern void abort(void);

TYPE v, count;

#define EXCHANGE(VAR, MODE)  if (__sync_mem_exchange (&VAR, count + 1, MODE)  \
				 !=  count++) abort()
void test_exchange()
{
  v = 0;
  count = 0;
  EXCHANGE (v, __SYNC_MEM_RELAXED);
  EXCHANGE (v, __SYNC_MEM_ACQUIRE);
  EXCHANGE (v, __SYNC_MEM_RELEASE);
  EXCHANGE (v, __SYNC_MEM_ACQ_REL);
  EXCHANGE (v, __SYNC_MEM_SEQ_CST);
}



main ()
{
  test_exchange();
  return 0;
}
