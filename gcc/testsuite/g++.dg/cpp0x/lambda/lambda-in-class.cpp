// { dg-do "compile" }
// { dg-options "-std=c++0x"}

template<typename F>
void call(F f) { f(); }

class C {
  private:
    int m_i;

  public:
    C() {
      m_i = -1;
      call([this] () -> void { m_i = 0; });
      assert(m_i == 0);
      call([this] () -> void { this->m_i = 1; });
      assert(m_i == 1);
      call([&] () -> void { m_i = 2; });
      assert(m_i == 2);
      call([&] () -> void { this->m_i = 3; });
      assert(m_i == 3);
      call([=] () -> void { m_i = 4; }); // copies 'this' or --copies-m_i--?
      assert(m_i == 4);
      call([=] () -> void { this->m_i = 5; });
      assert(m_i == 5);
    }

};

int main() {
  C c;

  call([this] () -> void {}); // { dg-error: "cannot capture `this' outside of class method" }

  return 0;
}

