set height 0
break main
disp/i $pc
run

set $ret = 0
while memmodel_fini != 1
  call memmodel_other_threads()
  stepi
  set $ret |= memmodel_step_verify()
end

set $ret |= memmodel_final_verify()
continue
quit $ret
