// { dg-options "-Wmissing-pph"
#define NAME v
#define VALUE 1
#include "d0symnotinc.h" // { dg-warning "cannot open PPH file .*" }
