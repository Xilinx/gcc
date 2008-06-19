template<typename F>
void call(F f) { f(); }


int main() {
  call([] () -> void {});
  call([] () -> void mutable {});

  int i;
  call([i] () -> void mutable { i = 0; });

  return 0;
}

