template< typename T >
struct wrapper {
  T value;
  static T cache;
};

template< typename T >
T wrapper<T>::cache = 3;

template<>
struct wrapper<char> {
  int value;
  static int cache;
};

int wrapper<char>::cache = 2;

template
struct wrapper<short>;

template
long wrapper<long>::cache;

int main() {
  wrapper<char> vc;
  wrapper<short> vs;
  wrapper<int> vi;
  return 0;
}
