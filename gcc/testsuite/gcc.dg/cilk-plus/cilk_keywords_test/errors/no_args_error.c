int spawn_1 ();
typedef int(*func) (int);

void check () {
      func var = spawn_1;
        _Cilk_spawn var (); /* { dg-error "too few arguments to function" } */
}

