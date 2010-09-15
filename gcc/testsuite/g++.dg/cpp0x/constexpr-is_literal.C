// { dg-options -std=c++0x }

#include <type_traits>

#define IS_LIT(T) (std::is_literal_type<T>::value)
#define SA(X) static_assert (X, #X)

enum E1 { };
enum class E2 { };
struct Literal {};

struct NotLiteral {
private:
  NotLiteral(const NotLiteral&);
};

SA (IS_LIT (int));
SA (IS_LIT (int[]));
SA (IS_LIT (int[3]));
SA (IS_LIT (double));
SA (IS_LIT (void *));
SA (IS_LIT (decltype (nullptr)));
SA (IS_LIT (int Literal::*));
SA (IS_LIT (void (Literal::*)()));
SA (IS_LIT (E1));
SA (IS_LIT (E2));
SA (IS_LIT (Literal));
SA (!IS_LIT (NotLiteral));
SA (IS_LIT (NotLiteral *));
SA (IS_LIT (NotLiteral NotLiteral::*));
SA (IS_LIT (NotLiteral (NotLiteral::*)(NotLiteral)));
