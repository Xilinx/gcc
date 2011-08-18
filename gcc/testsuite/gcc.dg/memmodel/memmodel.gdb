set height 0
break main
disp/i $pc
run

set $ret = 0
while (memmodel_fini != 1) && (! $ret)
  call memmodel_other_threads()
  stepi
  set $ret |= memmodel_step_verify()
end

if (! $ret)
  set $ret |= memmodel_final_verify()
end
continue
quit $ret
