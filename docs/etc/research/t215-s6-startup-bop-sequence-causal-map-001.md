# T215 S6: Startup BOP Sequence Causal Map

## Question

Can the first controlled `50:3D` terminal in the current source-built
composition be attributed to SYSINIT or to an already-started COMMAND.COM
exit path without reading guest memory, arguments, or provider state?

## Inputs

- The existing generic-UD bridge and its accepted outcomes.
- OpenNT `doskrnl/bios/sysinit1.asm`, `cmd/command/init.asm`,
  `softpc.new/host/src/nt_bop.c`, and `dos/command/cmdenv.c`.
- Current T204 startup-image provider plus the T215 S5 direct-provider
  CONFIG/AUTOEXEC overlay.
- `tools/Invoke-T214S3CompositionOwnerFixture.ps1`, source-built with MSVC
  x64 `/MT`.

## Procedure

The fixture alone enables an adapter-owned, default-off 128-record observer.
After an event has already been accepted by the normal bridge, it copies only
CS:EIP, selector, optional service byte, and already selected disposition.
It neither reads guest memory nor changes provider, CPU, result, or control
flow.  The fixture checks disabled-copy rejection, finite non-overflowing
capture, and equality between the final sequence record and the existing
terminal observer, then disables both observers.

## Observation

The fresh `t215-s6-bop-sequence-r1` run records 27 accepted events and no
overflow.  The relevant ordered suffix is:

```text
8DC8:0732  54:0C  accepted
9346:6BDF  50:12  accepted
9346:64F6  50:00  accepted
9346:443C  50:21  accepted
9346:635D  50:16  accepted
9346:61A3  50:02  accepted
8DC8:08AF  5E     accepted
0034:6BDF  50:12  accepted
8DC8:0984  50:3D  controlled stop
```

Thus the current native path actually consumes the attached startup
configuration: `54:0C` is followed by the normal DEM open/read/seek/close
family.  The former conclusion that the overlay might merely be installed but
unused is no longer applicable.

## Source Mapping And Confidence

`sysinit1.asm` labels `ProcessConfig` and issues `CMDSVC SVC_GETCONFIGSYS`
there.  Later it opens and executes the command interpreter (`int 21h` open,
then EXEC); an error falls through `comerr` to
`SVC SVC_DEMEXITVDM`.  `cmd/command/init.asm` has a separate
`SVC SVC_DEMEXITVDM` only in COMMAND's own-parent exit loop.

The terminal remains in the same `8DC8` segment that issued `54:0C`, and the
sequence contains no observable COMMAND exit boundary between the configuration
read and `50:3D`.  This makes the SYSINIT command-interpreter load/error path
the higher-confidence classification.  It is not proof of the exact failing
DOS operation: the observer intentionally lacks path, register and guest-memory
data, and it does not assign a new implementation task from the trace.

The preceding selector `5E` is also not interpreted as a service
implementation instruction.  Historical `MS_bop_E` invokes `UMBNotify(0)` and
`demDasdInit()` for AL zero; its observed occurrence is retained only as
owner-routing evidence.

## Follow-up

The next proposed recovery must be admitted as a whole owner package for
guest executable image visibility and COMMAND.COM load/search/handle
contracts, with original source/ABI/failure mapping before implementation.
It must not be a leaf `50:3D` patch, a trace-derived backlog item, or a change
to Bochs/mantle.
