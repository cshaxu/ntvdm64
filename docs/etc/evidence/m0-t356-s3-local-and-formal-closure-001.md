# M0 T356 S3 — Local and formal selected-cohort closure

## Focused x86 fixture

Build root: `build/M0-T356/S3/base-vdm-x86`.

The task-owned Ninja graph compiled and linked the existing source-shaped Base
VDM fixture and its direct owner set: Base VDM client/local/broker, copied
broker record, app launch declaration, x86 thread-start compatibility and the
session mapping/lease owner.

Command (inside a fixed MSVC x86 developer environment):

```text
ninja -C build/M0-T356/S3/base-vdm-x86 test -j 6
```

Observed terminal line:

```text
PASS: local Base VDM broker contract
```

The fixture covers the selected normal copied record, missing/empty/multiline
and duplicate declaration rejection, first-VDM query, buffer-capacity failure
and retry, pending DOS wait/wake/retry, re-entry count, current-directory
one-shot transfer, unavailable separate-WOW request and session-local
`ExitVDM`.  It does not execute a guest instruction or synthesize a PSP/result.

## Fresh formal CPU40/x86 link

Build root: `build/M0-T356/S3/formal-x86`.

The task-owned graph was generated with the selected original-source generator
and Node 22 GDP slot tool, then executed in the same fixed MSVC x86 developer
environment:

```text
ninja -C build/M0-T356/S3/formal-x86 original-softpc-process.exe -j 6
```

The graph completed all 428 actions and linked:

```text
build/M0-T356/S3/formal-x86/original-softpc-process.exe
build/M0-T356/S3/formal-x86/original-softpc-process.exe.map
```

The generated graph explicitly selects:

- `mvdm-host/dos/command/{cmdmisc.c,cmdexec.c}`;
- `mvdm-host/dos/dem/demmisc.c`;
- `adapter-mvdm-host-out/basesrv/source/base_vdm_local.c`;
- `adapter-mvdm-host-out/softpc/mvdm_softpc_execution.c`.

The first invocation of this graph failed before any compiler action because
`cl.exe` was not inherited by Ninja.  Re-running the identical graph through
the fixed MSVC x86 developer environment completed successfully.  That is a
verification-container correction, not a product source change.

MSVC emitted existing historical warnings during the successful link.  S3 did
not suppress, alter or classify them: this task's x86 source/command closure
does not authorize a warning or ABI remediation program.

## Result

S3 passes its local contract and formal-link requirements.  Neither result
demonstrates that `COMMAND.COM /C EXIT`, NTDOS `$Exec`, a guest child, or a
controlled guest return has executed.  The sole permitted next step is S4's
one fixed-container observation, with unchanged media and command declaration.
