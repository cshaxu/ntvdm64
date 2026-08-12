# T167 S1 native controlled-stop exit-contract audit 001

`exception.cc:1607-1610` handles an accepted typed adapter `STOP` mechanically:
it sets `async_event`, sets `kill_bochs_request`, and longjmps from the CPU
loop. `main.cc:1087-1093` then observes that flag and lets simulation return.
The old text configuration shell treats every such return as unexpected:
`gui/textconfig.cc:432-434` calls `SIM->quit_sim(1)`. `main.cc:355-367` then
shows the Win32 close prompt and returns `SIM->get_exit_code()`. Both CLI layers
preserve that code (`ntdos64_bochs_engine.c:244-248`, `ntdos64_run.c:323-339`).

Thus T166 exit 1 is native product-shell policy, not a BOP/guest failure. The
only source-respecting candidate is a separately registered change to the
already generic accepted-STOP path: call native `SIM->quit_sim(0)` there. It
does not inspect BOP bytes or add adapter semantics; all other CPU-loop returns
retain the original textconfig exit-1 behavior. T168 must implement and test
that exceptional Bochs change only after updating the intrusion register.
