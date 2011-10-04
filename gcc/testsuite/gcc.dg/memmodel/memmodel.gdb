set height 0
break main
disp/i $pc
run

set $ret = 0
while (__gdb_memmodel_fini != 1) && (! $ret)
  call __gdb_wrapper_other_threads()
  stepi
  set $ret |= memmodel_step_verify()
end

if (! $ret)
  set $ret |= __gdb_wrapper_final_verify()
end
continue
quit $ret
