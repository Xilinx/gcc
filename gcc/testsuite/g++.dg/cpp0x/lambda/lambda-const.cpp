template<typename F>
void call(const F& f) { f(); }


int main() {
  call([] () -> void {});
  //call([] () -> void mutable {}); // error (in call()): f does not have const operator()

  int i;
  call([&i] () -> void { i = 0; });
  //call([i] () -> void { i = 0; }); // error: assignment to non-reference capture in const lambda

  return 0;
}

