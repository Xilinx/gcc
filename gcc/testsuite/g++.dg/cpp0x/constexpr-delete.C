// { dg-options -std=c++0x }

constexpr bool never() = delete; // { dg-error "constexpr .* deleted" }
