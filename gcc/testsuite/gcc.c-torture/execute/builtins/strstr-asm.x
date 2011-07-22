# If we are using gold plugins for LTO compiles, we fail to find my_strstr.
set torture_eval_before_compile {
  if {[string match {*-flto*} "$option"]} {
    continue
  }
}
return 0
