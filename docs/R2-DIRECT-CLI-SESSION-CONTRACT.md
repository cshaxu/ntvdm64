# R2 Direct CLI Session Contract

Status: R2.1 implementation contract, 2026-08-07.

## Scope

`ntdos64-run` is the modern research-runner outer shell. It owns command-line
parsing, image classification, direct PE host handoff, bounded cancellation,
and exit-code collection. It is not the historical NTVDM process, PIF, WOW,
or console-server shell.

## Session Rules

1. PE32 and PE32+ targets are passed to documented `CreateProcessW`; their
   arguments, normal inherited console streams, working directory, and exit
   code remain host-process behavior.
2. DOS and NE targets require an explicit engine argument. The dispatcher
   only starts that engine as a child process with `--`; it supplies no DOS
   bootstrap, BOP, SVC, device, or namespace behavior.
3. The runner creates one child process group per handoff. Ctrl-C, Ctrl-Break,
   and console-close notifications set a private cancel event. Only the
   `run_process` owner consumes it, terminates that child, waits for it, and
   closes all process/event handles.
4. Cancellation never changes registry state, installs a handler outside the
   runner lifetime, injects code, enumerates ambient drives, or sends a
   control event to unrelated processes.
5. A normal child exit is returned unchanged. A cancelled child exposes the
   documented Windows control-C termination status returned by the child
   process; the runner does not synthesize a DOS exit status.

## Exclusions

This contract neither revives `nt_reset.c` nor imports its PIF/WOW/console
policy. It does not make the historical CCPU chain callable from the modern
MinGW runtime. Historical-source execution remains in its separate x86
compiler/CRT island until a later explicit adapter boundary is admitted.

## Verification

- A PE child that executes `exit 37` returns `37` through `ntdos64-run`.
- The existing DOS/NE explicit-engine rejection remains exit `3` when no
  engine is selected.
- A manual cancellation check may target a disposable child process only; it
  must confirm that the runner reaps its child and does not leave a process
  running. No historical guest image is needed for this check.
