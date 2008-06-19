//#include <concepts>

template<typename F>
void call(F f) { f(); }

template<typename F, typename T1>
//requires std::Callable1<F, T1&>
void call(F f, T1& a1) { f(a1); }

template<typename F, typename T1>
//requires std::Callable1<F, const T1&>
void call(F f, const T1& a1) { f(a1); }

//template<typename F, typename T1, typename T2>
//requires std::Callable2<F, T1&, T2&>
//void call(F f, T1& a1, T2& a2) { f(a1, a2); }

