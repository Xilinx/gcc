#include "call.h"

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

  return 0;
}

