# M0 T318 S2 P11 — Original CPU execution after NTIO admission

## Question

Does the intact, source-shaped CPU40 startup merely wait in host composition,
or does it enter the original SoftPC CPU execution route after the P10 NTIO
load decision?

## Method

The P9 real-console observer retained its own child process handle for five
seconds and read its public Win32 process CPU time immediately before its
controlled test-only termination. No product source or execution state was
instrumented.

## Observation

The intact profile with original `-f` Stream I/O and the admitted ordinary
child remained `STILL_ACTIVE`; its accumulated process CPU time was
approximately 2703 ms within the five-second wall-clock window. No error
dialog or `/C VER` console text was observed.

## Source interpretation

`softpc.new/base/support/main.c` calls original
`InitialiseDosEmulation(argc, argv)` and then
`host_start_cpu()`. `softpc.new/host/src/nt_cpu.c::host_start_cpu` calls the
original `cpu_simulate()` body. Together with P10's NTIO success/failure
counterfactual, the sustained CPU time is evidence that the successful
original route enters CCPU execution rather than only remaining in Base VDM or
console setup.

## Boundary

The observation does not identify individual guest instructions. It cannot
prove that NTDOS loaded, that COMMAND completed `/C VER`, or that ordinary
EXEC/parent return occurred. Those remain the next guest-owned continuation
requirements of S2.
