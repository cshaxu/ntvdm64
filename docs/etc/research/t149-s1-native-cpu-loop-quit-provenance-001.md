# T149 S1 native CPU-loop quit provenance

The apparent T146/T148 early quit is the observer's intended benchmark stop,
not a BOP, guest, adapter, device, or CPU archive behavior change.

`Invoke-T95S7RuntimeTraceObservation.ps1` launches Bochs with `-benchmark 1`.
`main.cc` registers `benchmarkTimer` for `benchmark_mode * 1000000` emulated
ticks; `pc_system.cc:409-414` sets `kill_bochs_request=1`; `event.cc` returns
from `cpu_loop` when that flag is set; and `main.cc:1087-1093` exits the loop.
The T148 log records CPU-loop quit at tick `1,025,044`, matching the one
million tick budget plus normal timer handling granularity.

Thus prior absence of the late prefetch trace from these runs proves only that
the bounded benchmark expired first. It is not evidence against the
predecessor path. No semantic correction is indicated. Any later observation
must use a separately admitted larger fixed instruction budget, not a retry
of these one-million benchmarks.
