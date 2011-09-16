/* Test __sync_mem routines for existence and proper execution on 16 byte 
   values with each valid memory model.  */
/* { dg-do run } */
/* { dg-require-effective-target sync_int_128 } */
/* { dg-options "-mcx16" { target { x86_64-*-* } } } */

/* Test the execution of the __sync_mem_*OP builtin routines for an int_128.  */

extern void abort(void);

__int128_t v, count, res;
const __int128_t init = ~0;

/* The fetch_op routines return the original value before the operation.  */

void
test_fetch_add ()
{
  v = 0;
  count = 1;

  if (__sync_mem_fetch_add (&v, count, __SYNC_MEM_RELAXED) != 0)
    abort ();

  if (__sync_mem_fetch_add (&v, 1, __SYNC_MEM_CONSUME) != 1) 
    abort ();

  if (__sync_mem_fetch_add (&v, count, __SYNC_MEM_ACQUIRE) != 2)
    abort ();

  if (__sync_mem_fetch_add (&v, 1, __SYNC_MEM_RELEASE) != 3) 
    abort ();

  if (__sync_mem_fetch_add (&v, count, __SYNC_MEM_ACQ_REL) != 4) 
    abort ();

  if (__sync_mem_fetch_add (&v, 1, __SYNC_MEM_SEQ_CST) != 5) 
    abort ();
}


void
test_fetch_sub()
{
  v = res = 20;
  count = 0;

  if (__sync_mem_fetch_sub (&v, count + 1, __SYNC_MEM_RELAXED) !=  res--) 
    abort ();

  if (__sync_mem_fetch_sub (&v, 1, __SYNC_MEM_CONSUME) !=  res--) 
    abort ();

  if (__sync_mem_fetch_sub (&v, count + 1, __SYNC_MEM_ACQUIRE) !=  res--) 
    abort ();

  if (__sync_mem_fetch_sub (&v, 1, __SYNC_MEM_RELEASE) !=  res--) 
    abort ();

  if (__sync_mem_fetch_sub (&v, count + 1, __SYNC_MEM_ACQ_REL) !=  res--) 
    abort ();

  if (__sync_mem_fetch_sub (&v, 1, __SYNC_MEM_SEQ_CST) !=  res--) 
    abort ();
}

void
test_fetch_and ()
{
  v = init;

  if (__sync_mem_fetch_and (&v, 0, __SYNC_MEM_RELAXED) !=  init) 
    abort ();

  if (__sync_mem_fetch_and (&v, init, __SYNC_MEM_CONSUME) !=  0) 
    abort ();

  if (__sync_mem_fetch_and (&v, 0, __SYNC_MEM_ACQUIRE) !=  0)
    abort ();

  v = ~v;
  if (__sync_mem_fetch_and (&v, init, __SYNC_MEM_RELEASE) !=  init)
    abort ();

  if (__sync_mem_fetch_and (&v, 0, __SYNC_MEM_ACQ_REL) !=  init) 
    abort ();

  if (__sync_mem_fetch_and (&v, 0, __SYNC_MEM_SEQ_CST) !=  0) 
    abort ();
}

void
test_fetch_nand ()
{
  v = init;

  if (__sync_mem_fetch_nand (&v, 0, __SYNC_MEM_RELAXED) !=  init) 
    abort ();

  if (__sync_mem_fetch_nand (&v, init, __SYNC_MEM_CONSUME) !=  init) 
    abort ();

  if (__sync_mem_fetch_nand (&v, 0, __SYNC_MEM_ACQUIRE) !=  0 ) 
    abort ();

  if (__sync_mem_fetch_nand (&v, init, __SYNC_MEM_RELEASE) !=  init)
    abort ();

  if (__sync_mem_fetch_nand (&v, init, __SYNC_MEM_ACQ_REL) !=  0) 
    abort ();

  if (__sync_mem_fetch_nand (&v, 0, __SYNC_MEM_SEQ_CST) !=  init) 
    abort ();
}

void
test_fetch_xor ()
{
  v = init;
  count = 0;

  if (__sync_mem_fetch_xor (&v, count, __SYNC_MEM_RELAXED) !=  init) 
    abort ();

  if (__sync_mem_fetch_xor (&v, ~count, __SYNC_MEM_CONSUME) !=  init) 
    abort ();

  if (__sync_mem_fetch_xor (&v, 0, __SYNC_MEM_ACQUIRE) !=  0) 
    abort ();

  if (__sync_mem_fetch_xor (&v, ~count, __SYNC_MEM_RELEASE) !=  0) 
    abort ();

  if (__sync_mem_fetch_xor (&v, 0, __SYNC_MEM_ACQ_REL) !=  init) 
    abort ();

  if (__sync_mem_fetch_xor (&v, ~count, __SYNC_MEM_SEQ_CST) !=  init) 
    abort ();
}

void
test_fetch_or ()
{
  v = 0;
  count = 1;

  if (__sync_mem_fetch_or (&v, count, __SYNC_MEM_RELAXED) !=  0) 
    abort ();

  count *= 2;
  if (__sync_mem_fetch_or (&v, 2, __SYNC_MEM_CONSUME) !=  1) 
    abort ();

  count *= 2;
  if (__sync_mem_fetch_or (&v, count, __SYNC_MEM_ACQUIRE) !=  3) 
    abort ();

  count *= 2;
  if (__sync_mem_fetch_or (&v, 8, __SYNC_MEM_RELEASE) !=  7) 
    abort ();

  count *= 2;
  if (__sync_mem_fetch_or (&v, count, __SYNC_MEM_ACQ_REL) !=  15) 
    abort ();

  count *= 2;
  if (__sync_mem_fetch_or (&v, count, __SYNC_MEM_SEQ_CST) !=  31) 
    abort ();
}

/* The OP_fetch routines return the new value after the operation.  */

void
test_add_fetch ()
{
  v = 0;
  count = 1;

  if (__sync_mem_add_fetch (&v, count, __SYNC_MEM_RELAXED) != 1)
    abort ();

  if (__sync_mem_add_fetch (&v, 1, __SYNC_MEM_CONSUME) != 2) 
    abort ();

  if (__sync_mem_add_fetch (&v, count, __SYNC_MEM_ACQUIRE) != 3)
    abort ();

  if (__sync_mem_add_fetch (&v, 1, __SYNC_MEM_RELEASE) != 4) 
    abort ();

  if (__sync_mem_add_fetch (&v, count, __SYNC_MEM_ACQ_REL) != 5) 
    abort ();

  if (__sync_mem_add_fetch (&v, count, __SYNC_MEM_SEQ_CST) != 6) 
    abort ();
}


void
test_sub_fetch ()
{
  v = res = 20;
  count = 0;

  if (__sync_mem_sub_fetch (&v, count + 1, __SYNC_MEM_RELAXED) !=  --res) 
    abort ();

  if (__sync_mem_sub_fetch (&v, 1, __SYNC_MEM_CONSUME) !=  --res) 
    abort ();                                                  
                                                               
  if (__sync_mem_sub_fetch (&v, count + 1, __SYNC_MEM_ACQUIRE) !=  --res) 
    abort ();                                                  
                                                               
  if (__sync_mem_sub_fetch (&v, 1, __SYNC_MEM_RELEASE) !=  --res) 
    abort ();                                                  
                                                               
  if (__sync_mem_sub_fetch (&v, count + 1, __SYNC_MEM_ACQ_REL) !=  --res) 
    abort ();                                                  
                                                               
  if (__sync_mem_sub_fetch (&v, count + 1, __SYNC_MEM_SEQ_CST) !=  --res) 
    abort ();
}

void
test_and_fetch ()
{
  v = init;

  if (__sync_mem_and_fetch (&v, 0, __SYNC_MEM_RELAXED) !=  0) 
    abort ();

  v = init;
  if (__sync_mem_and_fetch (&v, init, __SYNC_MEM_CONSUME) !=  init) 
    abort ();

  if (__sync_mem_and_fetch (&v, 0, __SYNC_MEM_ACQUIRE) !=  0) 
    abort ();

  v = ~v;
  if (__sync_mem_and_fetch (&v, init, __SYNC_MEM_RELEASE) !=  init)
    abort ();

  if (__sync_mem_and_fetch (&v, 0, __SYNC_MEM_ACQ_REL) !=  0) 
    abort ();

  v = ~v;
  if (__sync_mem_and_fetch (&v, 0, __SYNC_MEM_SEQ_CST) !=  0) 
    abort ();
}

void
test_nand_fetch ()
{
  v = init;

  if (__sync_mem_nand_fetch (&v, 0, __SYNC_MEM_RELAXED) !=  init) 
    abort ();              
                           
  if (__sync_mem_nand_fetch (&v, init, __SYNC_MEM_CONSUME) !=  0) 
    abort ();              
                           
  if (__sync_mem_nand_fetch (&v, 0, __SYNC_MEM_ACQUIRE) !=  init) 
    abort ();              
                           
  if (__sync_mem_nand_fetch (&v, init, __SYNC_MEM_RELEASE) !=  0)
    abort ();              
                           
  if (__sync_mem_nand_fetch (&v, init, __SYNC_MEM_ACQ_REL) !=  init) 
    abort ();              
                           
  if (__sync_mem_nand_fetch (&v, 0, __SYNC_MEM_SEQ_CST) !=  init) 
    abort ();
}



void
test_xor_fetch ()
{
  v = init;
  count = 0;

  if (__sync_mem_xor_fetch (&v, count, __SYNC_MEM_RELAXED) !=  init) 
    abort ();

  if (__sync_mem_xor_fetch (&v, ~count, __SYNC_MEM_CONSUME) !=  0) 
    abort ();

  if (__sync_mem_xor_fetch (&v, 0, __SYNC_MEM_ACQUIRE) !=  0) 
    abort ();

  if (__sync_mem_xor_fetch (&v, ~count, __SYNC_MEM_RELEASE) !=  init) 
    abort ();

  if (__sync_mem_xor_fetch (&v, 0, __SYNC_MEM_ACQ_REL) !=  init) 
    abort ();

  if (__sync_mem_xor_fetch (&v, ~count, __SYNC_MEM_SEQ_CST) !=  0) 
    abort ();
}

void
test_or_fetch ()
{
  v = 0;
  count = 1;

  if (__sync_mem_or_fetch (&v, count, __SYNC_MEM_RELAXED) !=  1) 
    abort ();

  count *= 2;
  if (__sync_mem_or_fetch (&v, 2, __SYNC_MEM_CONSUME) !=  3) 
    abort ();

  count *= 2;
  if (__sync_mem_or_fetch (&v, count, __SYNC_MEM_ACQUIRE) !=  7) 
    abort ();

  count *= 2;
  if (__sync_mem_or_fetch (&v, 8, __SYNC_MEM_RELEASE) !=  15) 
    abort ();

  count *= 2;
  if (__sync_mem_or_fetch (&v, count, __SYNC_MEM_ACQ_REL) !=  31) 
    abort ();

  count *= 2;
  if (__sync_mem_or_fetch (&v, count, __SYNC_MEM_SEQ_CST) !=  63) 
    abort ();
}


/* Test the OP routines with a result which isn't used. Use both variations
   within each function.  */

void
test_add ()
{
  v = 0;
  count = 1;

  __sync_mem_add_fetch (&v, count, __SYNC_MEM_RELAXED);
  if (v != 1)
    abort ();

  __sync_mem_fetch_add (&v, count, __SYNC_MEM_CONSUME);
  if (v != 2)
    abort ();

  __sync_mem_add_fetch (&v, 1 , __SYNC_MEM_ACQUIRE);
  if (v != 3)
    abort ();

  __sync_mem_fetch_add (&v, 1, __SYNC_MEM_RELEASE);
  if (v != 4)
    abort ();

  __sync_mem_add_fetch (&v, count, __SYNC_MEM_ACQ_REL);
  if (v != 5)
    abort ();

  __sync_mem_fetch_add (&v, count, __SYNC_MEM_SEQ_CST);
  if (v != 6)
    abort ();
}


void
test_sub()
{
  v = res = 20;
  count = 0;

  __sync_mem_sub_fetch (&v, count + 1, __SYNC_MEM_RELAXED);
  if (v != --res)
    abort ();

  __sync_mem_fetch_sub (&v, count + 1, __SYNC_MEM_CONSUME);
  if (v != --res)
    abort ();                                                  
                                                               
  __sync_mem_sub_fetch (&v, 1, __SYNC_MEM_ACQUIRE);
  if (v != --res)
    abort ();                                                  
                                                               
  __sync_mem_fetch_sub (&v, 1, __SYNC_MEM_RELEASE);
  if (v != --res)
    abort ();                                                  
                                                               
  __sync_mem_sub_fetch (&v, count + 1, __SYNC_MEM_ACQ_REL);
  if (v != --res)
    abort ();                                                  
                                                               
  __sync_mem_fetch_sub (&v, count + 1, __SYNC_MEM_SEQ_CST);
  if (v != --res)
    abort ();
}

void
test_and ()
{
  v = init;

  __sync_mem_and_fetch (&v, 0, __SYNC_MEM_RELAXED);
  if (v != 0)
    abort ();

  v = init;
  __sync_mem_fetch_and (&v, init, __SYNC_MEM_CONSUME);
  if (v != init)
    abort ();

  __sync_mem_and_fetch (&v, 0, __SYNC_MEM_ACQUIRE);
  if (v != 0)
    abort ();

  v = ~v;
  __sync_mem_fetch_and (&v, init, __SYNC_MEM_RELEASE);
  if (v != init)
    abort ();

  __sync_mem_and_fetch (&v, 0, __SYNC_MEM_ACQ_REL);
  if (v != 0)
    abort ();

  v = ~v;
  __sync_mem_fetch_and (&v, 0, __SYNC_MEM_SEQ_CST);
  if (v != 0)
    abort ();
}

void
test_nand ()
{
  v = init;

  __sync_mem_fetch_nand (&v, 0, __SYNC_MEM_RELAXED);
  if (v != init)
    abort ();

  __sync_mem_fetch_nand (&v, init, __SYNC_MEM_CONSUME);
  if (v != 0)
    abort ();

  __sync_mem_nand_fetch (&v, 0, __SYNC_MEM_ACQUIRE);
  if (v != init)
    abort ();

  __sync_mem_nand_fetch (&v, init, __SYNC_MEM_RELEASE);
  if (v != 0)
    abort ();

  __sync_mem_fetch_nand (&v, init, __SYNC_MEM_ACQ_REL);
  if (v != init)
    abort ();

  __sync_mem_nand_fetch (&v, 0, __SYNC_MEM_SEQ_CST);
  if (v != init)
    abort ();
}



void
test_xor ()
{
  v = init;
  count = 0;

  __sync_mem_xor_fetch (&v, count, __SYNC_MEM_RELAXED);
  if (v != init)
    abort ();

  __sync_mem_fetch_xor (&v, ~count, __SYNC_MEM_CONSUME);
  if (v != 0)
    abort ();

  __sync_mem_xor_fetch (&v, 0, __SYNC_MEM_ACQUIRE);
  if (v != 0)
    abort ();

  __sync_mem_fetch_xor (&v, ~count, __SYNC_MEM_RELEASE);
  if (v != init)
    abort ();

  __sync_mem_fetch_xor (&v, 0, __SYNC_MEM_ACQ_REL);
  if (v != init)
    abort ();

  __sync_mem_xor_fetch (&v, ~count, __SYNC_MEM_SEQ_CST);
  if (v != 0)
    abort ();
}

void
test_or ()
{
  v = 0;
  count = 1;

  __sync_mem_or_fetch (&v, count, __SYNC_MEM_RELAXED);
  if (v != 1)
    abort ();

  count *= 2;
  __sync_mem_fetch_or (&v, count, __SYNC_MEM_CONSUME);
  if (v != 3)
    abort ();

  count *= 2;
  __sync_mem_or_fetch (&v, 4, __SYNC_MEM_ACQUIRE);
  if (v != 7)
    abort ();

  count *= 2;
  __sync_mem_fetch_or (&v, 8, __SYNC_MEM_RELEASE);
  if (v != 15)
    abort ();

  count *= 2;
  __sync_mem_or_fetch (&v, count, __SYNC_MEM_ACQ_REL);
  if (v != 31)
    abort ();

  count *= 2;
  __sync_mem_fetch_or (&v, count, __SYNC_MEM_SEQ_CST);
  if (v != 63)
    abort ();
}

main ()
{
  test_fetch_add ();
  test_fetch_sub ();
  test_fetch_and ();
  test_fetch_nand ();
  test_fetch_xor ();
  test_fetch_or ();

  test_add_fetch ();
  test_sub_fetch ();
  test_and_fetch ();
  test_nand_fetch ();
  test_xor_fetch ();
  test_or_fetch ();

  test_add ();
  test_sub ();
  test_and ();
  test_nand ();
  test_xor ();
  test_or ();

  return 0;
}
