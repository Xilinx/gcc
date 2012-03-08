/* This test checks if the __builtin_cpu_* calls are recognized. */

/* { dg-do run } */

int
fn1 ()
{
  if (__builtin_cpu_supports_cmov () < 0)
    return -1;
  if (__builtin_cpu_supports_mmx () < 0)
    return -1;
  if (__builtin_cpu_supports_popcount () < 0)
    return -1;
  if (__builtin_cpu_supports_sse () < 0)
    return -1;
  if (__builtin_cpu_supports_sse2 () < 0)
    return -1;
  if (__builtin_cpu_supports_sse3 () < 0)
    return -1;
  if (__builtin_cpu_supports_ssse3 () < 0)
    return -1;
  if (__builtin_cpu_supports_sse4_1 () < 0)
    return -1;
  if (__builtin_cpu_supports_sse4_2 () < 0)
    return -1;
  if (__builtin_cpu_is_amd () < 0)
    return -1;
  if (__builtin_cpu_is_intel () < 0)
    return -1;
  if (__builtin_cpu_is_intel_atom () < 0)
    return -1;
  if (__builtin_cpu_is_intel_core2 () < 0)
    return -1;
  if (__builtin_cpu_is_intel_corei7 () < 0)
    return -1;
  if (__builtin_cpu_is_intel_corei7_nehalem () < 0)
    return -1;
  if (__builtin_cpu_is_intel_corei7_westmere () < 0)
    return -1;
  if (__builtin_cpu_is_intel_corei7_sandybridge () < 0)
    return -1;
  if (__builtin_cpu_is_amdfam10 () < 0)
    return -1;
  if (__builtin_cpu_is_amdfam10_barcelona () < 0)
    return -1;
  if (__builtin_cpu_is_amdfam10_shanghai () < 0)
    return -1;
  if (__builtin_cpu_is_amdfam10_istanbul () < 0)
    return -1;
  if (__builtin_cpu_is_amdfam15_bdver1 () < 0)
    return -1;
  if (__builtin_cpu_is_amdfam15_bdver2 () < 0)
    return -1;

  return 0;
}

int main ()
{
  return fn1 ();
}
