// { dg-options "-Winvalid-pph" }
namespace smother {
#include "x1struct1.h" // { dg-warning "PPH file .* not included at global scope" "" }
}
