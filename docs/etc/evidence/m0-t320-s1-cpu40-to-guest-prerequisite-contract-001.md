# M0 T320 S1 — CPU40-to-guest prerequisite contract

## Question

What original CPU40/worker path must be recovered before the frozen declared
DOS program can reach its NTDOS `EXEC` and controlled-return contract?

## Source order

The selected original order is unambiguous:

```text
obj.vdm/ntvdm.c::main
  -> base/support/main.c::host_main
  -> host/src/nt_msscs.c::InitialiseDosEmulation
       -> reset / SetupInstallableVDD / scs_init
       -> original NTIO.SYS read into SAS RAM
       -> CS:IP = NTIO_LOAD_SEGMENT:NTIO_LOAD_OFFSET
  -> host/src/nt_cpu.c::host_start_cpu
  -> base/ccpu386/c_main.c::c_cpu_simulate
  -> original NTIO/NTDOS/COMMAND guest execution
  -> frozen NTDOS EXEC/parent return contract
```

`InitialiseDosEmulation` performs the NTIO file read and CS:IP assignment
before `host_start_cpu`; it is not a deferred app loader.  The current fixed
console markers `50:11`, `50:3B`, `50:0F`, `50:1B`, and `54:05` consequently
prove progress through original guest/bootstrap code, but do not prove the
later declared program or `54:0B` return.

## Worker classification

The pre-CPU setup also introduces original host workers:

- `nt_reset.c::reset -> nt_start_event_thread -> ConsoleEventThread` uses a
  local `try/except(VdmUnhandledExceptionFilter(...))`; it is covered by the
  existing known-thread report seam.
- `base/system/timer.c -> host_timer_init -> HeartBeatThread` is likewise a
  named heartbeat path covered by the same original filter contract.
- `nt_com.c::PollCommsThread` has its own original filter and requires a COM
  open; it is not a basic DOS-bootstrap predecessor.
- `nt_rflop.c::fdc_thread` is created directly only after an FDC data command
  finds no usable medium; it has no local filter.  P76 independently placed
  the earlier `0xc0000005` caller in original `floppy.c::wait_int`, whose
  recursive CPU40 transfer depends on the FDC/INT15/vector completion chain.
- `nt_det.c::InitDetect` and `nt_error.c::ErrorDialogBoxThread` are raw
  thread forms gated by hardware-detection or error-dialog paths; neither is
  a valid speculative recovery target.
- COMMAND child and pipe workers are only created after later `54:08` or
  redirection paths, which current evidence has not reached.

## Interpretation

The original main-thread CPU40 run and the original FDC/INT15 recursive
cohort are the only immediate prerequisites to the frozen DOS `EXEC` path.
The current fixed observation's bypass of both the top-level return and the
known-thread filter prevents attribution to a single raw thread without an
inadmissible debugger or changed runtime container.  It does not justify a
new BOP provider, guest rewrite, or generic worker replacement.

The narrow next recovery cohort is therefore the original
`floppy.c::wait_int -> FDC completion/INT15 vector -> host_simulate ->
c_cpu_simulate` contract already evidenced by P76, including its original
raw-FDC-worker edge only where that edge is actually reached.  Its required
negative behavior remains a wait/failure; no synthetic interrupt or disk
completion is authorized.

## S1 exit

This map identifies the complete immediate machine-to-guest gate and a single
source owner cohort.  S2 may now recover only that cohort's earliest missing
original prerequisite, preserving original interfaces and failure behavior.
