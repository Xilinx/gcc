/* { dg-do compile } */
/* { dg-options "-fgnu-tm -fdump-ipa-tmipa" } */

/* Test that indirect calls set the irrevocable bit.  */

void (indirect)(void);

extern crap() __attribute__((tm_irrevocable));

foo(){
    __tm_atomic {
      (indirect)();
    }
}

/* { dg-final { scan-ipa-dump-times "GTMA_MAY_ENTER_IRREVOCABLE" 1 "tmipa" } } */
