typedef int type;
type val = 3;

template< typename T >
T identity(T arg)
{ return arg + val; }

template<>
int identity< type >(type arg)
{ return arg + val; }

template
short identity(short arg);

int main() {
  return identity( 'a' );
}
