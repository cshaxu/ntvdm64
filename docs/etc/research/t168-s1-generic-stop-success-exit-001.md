# T168 S1 generic STOP success exit 001

## Change

After the existing interceptor validates a typed `BX_NTVDM_EXCEPTION_STOP`,
`src/bochs/cpu/exception.cc` now calls original `SIM->quit_sim(0)`. It replaces
only the previous mechanical `async_event`/`kill_bochs_request`/CPU longjmp
sequence. The branch does not read the instruction window, selector, service,
guest memory, or adapter-private state.

## Evidence

`Test-BochsUdInterceptBoundary.ps1` passes. A fresh
`artifacts/build/current/t168-generic-stop-exit-r1` deferred-POST derivative
compiled `main.o` and `cpu/exception.o`, compiled its already-admitted current
adapter object set, and linked `ntdos64-t98-current-adapter.exe` successfully.
The output SHA-256 is
`A551FB968C30804F8C6129FA393157F0ED36D781E4E968D6CFD015D664747669`.
No Bochs archive or device target was rebuilt.

## Interpretation

This is a generic exit disposition, not a BOP implementation: adapter remains
the sole producer of STOP. The original textconfig exit-1 fallback remains for
declined/pass-through and all other CPU-loop returns. Native runtime proof is
separately admitted because T168 does not permit a second native run.
