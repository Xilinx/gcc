// Test that we don't have to deal with type punning
// { dg-options -std=c++0x }

union U { float f; unsigned long ul; };

constexpr U u = { 1.0 };
constexpr float f = u.f;
constexpr unsigned long ul = u.ul; // { dg-error "U::ul" }
