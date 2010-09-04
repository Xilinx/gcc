// { dg-options -std=c++0x }

template<class T>
constexpr bool source(T&&) { return true; }

template<bool> struct sink {};

template<class T>
struct instance {	static T value; };

template<class T>
auto check(sink<source(instance<T>::value)>*) -> char;

template<class>
auto check(...) -> char (&)[2];

struct Literal {};

struct NotLiteral {
private:
  NotLiteral(const NotLiteral&);
};

static_assert(sizeof(check<int>(0)) == 1, "Error");
static_assert(sizeof(check<double>(0)) == 1, "Error");
static_assert(sizeof(check<Literal>(0)) == 1, "Error");
static_assert(sizeof(check<NotLiteral>(0)) != 1, "Error");

int main() {}
