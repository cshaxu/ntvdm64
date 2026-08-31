# M0 T318 S2 P22 — Fixed Container Admission And CCPU Fault Boundary

## Purpose

Continue the P21 fixed non-debug observation without changing the observer,
console ownership, firmware, media content, command line, or timeout.  This
checkpoint distinguishes an app-side declaration rejection from the original
SoftPC startup path, then records the next attributable runtime boundary.

## App composition outcome identity

`app/entry.c` now gives its own pre-original assembly failures distinct exit
statuses.  It does not translate any result returned by the original SoftPC
entry.  The former fixed-stage result `1` is now `69`, which identifies the
failure as app launch-declaration publication.

The cause is source-shaped rather than a BaseSrv or Console failure:

- Original `VDMINFO` restricts `MAXIMUM_VDM_CURRENT_DIR` to 64 bytes.
- The P21 stage's absolute `dos` directory exceeds that original command-record
  limit.
- `app_launch_declaration_publish` correctly rejects that record before
  `host_main` is entered; widening the original field would be the wrong fix.

## Formal container

The new formal stage is `build/M0-T318/S2/r`.  It contains the same fixed
firmware and DOS media tree as P21, but has an original-admissible absolute DOS
root.  From this checkpoint onward it is the single runtime container:

- observer: `build/tools/console-startup-observer.exe`;
- product arguments: `-f -o --ordinary-child`;
- real owned `CONIN$` and `CONOUT$` handles;
- timeout: 8000 ms;
- only the formally linked `original-softpc-process.exe` may be replaced.

The P21 long-path stage remains evidence of the rejected precondition; it is
not used for future execution observations.

## Result

With the short stage and the exact same observer contract, the x86 product
reaches original host startup and exits with:

```text
container=console-owning-nondebug
result=exited
exit=0xc0000005
timeout-ms=8000
```

There is no debugger, process-memory observation, BOP tracing, or changed
console mode in this result.  It is therefore the first attributable
non-debug CCPU/startup fault boundary, but it does not identify a specific
instruction and does not claim NTIO, NTDOS, EXEC, or parent return.

## Static owner-cluster disposition

The original path remains `host_main -> host_applInit ->
InitialiseDosEmulation -> scs_init -> host_start_cpu -> cpu_simulate`.

- The real-console cluster (`init_host_uis`, `InitScreenDesc`,
  `nt_start_event_thread`) is no longer the current terminal condition: the
  fixed container supplies valid console handles and execution passes the app
  declaration gate.
- `scs_init` still calls `GetNextVDMCommand(NULL)` before COMMAND, DEM, XMS or
  DBG initialization.  Its Base VDM-shaped provider remains a separate owner
  cluster and has not been bypassed.
- The next mechanical cluster is complete CCPU/C-VID vector composition.  The
  selected C-VID facade calls fields of `Cpu`; its generated header and the
  CCPU header have different vector layouts.  This must be recovered as one
  source/build-contract audit, not by adding a null-call shim or changing the
  runtime observer.

## Next action

Audit the selected original CCPU/C-VID build composition: generated vector
layout, defining translation unit, vector initialization, and all exact CCPU
providers.  Only after that whole mechanical contract is closed may the same
formal container be rerun.
