# M0 T317 S3 P2 — COMMAND worker / Base VDM context

## Purpose

Close the local execution-context seam required when the original COMMAND
child worker calls `GetNextVDMCommand` solely to increment and decrement the
Base VDM re-entry count.

## Original contract

The selected original `cmdexec.c` keeps its worker body and order:

1. increment `Exe32ActiveCount`;
2. call `GetNextVDMCommand` with `INCREMENT_REENTER_COUNT`;
3. create, resume, wait for and collect the local child process;
4. call `GetNextVDMCommand` with `DECREMENT_REENTER_COUNT`;
5. decrement the active count and exit the worker.

The original BaseClient implementation at
`O:/repos.external/OpenNT/base/win32/client/vdm.c` handles those state values
as a distinct server operation, rather than treating them as a command-record
request. Its BaseSrv/CSR transport is outside the product boundary.

## Recovered binding

`base_vdm_local` registers a finite owner-private bind/unbind hook with the
already active neutral `session`. The hook validates that the current session
is its owner and puts only the existing `base_vdm_local` record into that
thread's TLS. `thread_start_compat` continues to establish only the session
binding around the original cdecl worker; it has no BaseSrv knowledge.

Thus no stack pointer, native handle, guest pointer, or Base VDM record enters
an asynchronous MVDM ABI. The original `cmdexec.c` call expressions remain
unchanged.

## Verification

- External x86 focused build/run: `New-T307BaseVdmBrokerNinja.ps1
  -Architecture x86`, then `ninja -C build/M0-T307/S1/x86 test`. The fixture
  starts a detached cdecl worker, executes original-shape increment/decrement
  calls, verifies zero final re-entry count, and reports
  `PASS: local Base VDM broker contract`.
- Fresh formal x86 CCPU40 archive build: `New-T310OriginalSoftpcNinja.ps1
  -Architecture x86 -BuildRoot build/M0-T317/S3/command-child-r2`, then
  `ninja -C build/M0-T317/S3/command-child-r2 original-mvdm-command.lib
  basesrv-bindings.lib session.lib softpc-win32-bindings.lib`. All requested
  libraries completed; its selected defines include `CPU_40_STYLE` and omit
  `CPU_30_STYLE`.

## Boundary

This proves only local one-session child-worker context and balanced Base VDM
re-entry. It does not claim BaseSrv/CSRSS transport, a multi-session broker,
DOS `EXEC`/PSP parent restoration, remote Redirector, PIF product shell, WOW,
or guest execution.
