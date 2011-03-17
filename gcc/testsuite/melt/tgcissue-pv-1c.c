/* bug reported by Pierre Vittet 
   http://gcc.gnu.org/ml/gcc/2011-03/msg00206.html
   The C source file
   run in $GCCMELT_BUILD/gcc
   ./cc1 -fmelt-module-path=melt-modules -fmelt-source-path=melt-sources \
        -O2 -c -fmelt-mode=runfile \
        -fmelt-arg=$GCCMELT_SOURCE/gcc/testsuite/melt/tgcissue-pv-1m.melt \
	-fmelt-debug $GCCMELT_SOURCE/gcc/testsuite/melt/tgcissue-pv-1c.c
 */
int function1(void) {
  return 1;
}

int function2(void) {
  return 2;
}

int function3(void) {
  return 3;
}

int function4(void) {
  return 4;
}

int function5(void) {
  return 5;
}

int function6(void) {
  return 6;
}

int function7(void) {
  return 7;
}

int function8(void) {
  return 8;
}

int function9(void) {
  return 9;
}

int fun(int x, int y) {
  return function3() * x + function8() * y;
}
