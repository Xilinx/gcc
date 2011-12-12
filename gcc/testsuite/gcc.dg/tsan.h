/* Helper declarations and functions for ThreadSanitizer instrumentation (-ftsan) testing */

int printf (char *str, ...);
void exit (int);

/* Variables referenced by the instrumentation. */

__thread void **__tsan_shadow_stack;
__thread int __tsan_thread_ignore;

/* Local helper vars.  */

__thread void *shadow_stack[1024];
__thread int pad[1024];
__thread int mop_expect = 0;
__thread int mop_depth = 0;
__thread void* mop_addr = 0;
__thread unsigned long mop_pc = 0;
__thread unsigned mop_flags = 0;
__thread unsigned mop_line = 0;

/* Setups shadow stack var (not instrumented).  */

void
__tsan_init (void)
{
  __tsan_shadow_stack = shadow_stack;
}

/* Declare that we expect an instrumented memory access (not instrumented).
   depth - stack depth of the mop (0 - main, 1 - func called from main and so on).
   addr - memory access address.
   is_store - store/load.
   is_sblock - superblock flag of the access.
   size - memory access size in bytes. */

void
__tsan_expect_mop (int depth, void const volatile *addr, int is_store,
                   int is_sblock, int size, unsigned line)
{
  if (mop_expect)
    {
      printf ("missed mop: addr=%p pc=%p line=%d\n",
              mop_addr, (void*)mop_pc, mop_line);
      exit (1);
    }

  mop_expect = 1;
  mop_depth = depth;
  mop_addr = (void*)addr;
  mop_pc = (unsigned long)__builtin_return_address(0);
  mop_flags = (!!is_sblock) | ((!!is_store) << 1) | ((size - 1) << 2);
  mop_line = line;
}

/* Memory access function (referenced by instrumentation, not instrumented).  */

void
__tsan_handle_mop (void *addr, unsigned flags)
{
  unsigned long pc;
  int depth;

  printf ("mop: addr=%p flags=%x called from %p line=%d\n",
          addr, flags, __tsan_shadow_stack [-2], mop_line);
  if (mop_expect == 0)
    return;

  /* Verify parameters with what we expect.  */

  if (addr != mop_addr)
    {
      printf ("incorrect mop addr: %p/%p line=%d\n",
              addr,  mop_addr, mop_line);
      exit (1);
    }

  pc = (unsigned long)__builtin_return_address(0);
  if (pc < mop_pc - 100 || pc > mop_pc + 100)
    {
      printf ("incorrect mop pc: %p/%p line=%d\n",
              (void*)pc, (void*)mop_pc, mop_line);
      exit (1);
    }

  depth = __tsan_shadow_stack - shadow_stack - 1;
  if (depth != mop_depth)
    {
      printf ("incorrect mop depth: %d/%d line=%d\n",
              depth, mop_depth, mop_line);
      exit (1);
    }

  if (flags != mop_flags)
    {
      printf ("incorrect mop flags: %x/%x line=%d\n",
              flags, mop_flags, mop_line);
      exit (1);
    }

  mop_expect = 0;
}
