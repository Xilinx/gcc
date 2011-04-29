/* { dg-do run } */
/* { dg-options "-O2 -fno-strict-enum-precision" } */

enum X {
 X1,
 X2
};

int main(int argc, char *argv[]) {
 X x = static_cast<X>(argc + 999);
 if (x == X1) return 1;
 if (x == X2) return 1;
 return 0;
}
