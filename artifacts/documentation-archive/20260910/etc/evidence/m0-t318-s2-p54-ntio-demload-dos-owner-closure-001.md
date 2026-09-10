# M0 T318 S2 P54 — NTIO `demLoadDos` owner closure

## Scope

This record closes the source-side owner audit for the first selected NTIO
host-service transaction.  It does not claim that a fixed-container run has
reached that transaction.

## Original transaction

The selected original path remains:

```text
NTIO.SYS `msinit.asm::init`
  -> BOP 50h, service 11h
  -> `nt_bop.c::MS_bop_0`
  -> `demdisp.c::DemDispatch(0x11)`
  -> `demmisc.c::demLoadDos`
  -> original NTDOS image bytes at guest DI:0000
```

`MS_bop_0` reads exactly one service byte through original
`Sim32GetVDMPointer`, dispatches it, then advances IP by one.  `DemDispatch`
selects original `demLoadDos` at `SVC_DEMLOADDOS` (0x11).  No app loader,
alternate BOP dispatcher, or host COMMAND route participates.

## Direct prerequisites and disposition

- `demLoadDos` obtains its destination through original x86
  `GetVDMAddr(DI, 0)`.  In the selected CPU40 x86 runtime this is the original
  contiguous SAS linear-address contract; it is not an opaque mapping token.
  The pre-existing dual-width mirror divergence only prevents a native-width
  cursor truncation during compile/link.  It does not replace the x86 loader
  or guest memory layout.
- The already-selected P2/P46 DOS-media binding supplies the original
  `pszDefaultDOSDirectory`.  `demLoadDos` itself retains its original
  `\\ntdos.sys` name formation, `CreateFileOem`, 16 KiB `ReadFile` loop,
  `CloseHandle`, and fatal `TerminateVDM` direction.
- The formal x86 dependency record for `obj/dem/demfile.obj` resolves
  `softpc.h` to `src/mvdm-host/inc/softpc.h`, not to an adapter shadow header.
  The corresponding `obj/host/nt_bop.obj` record resolves original `sim32.h`.
  Therefore the first transaction has no active redirector pointer-scope
  dependency.

## Boundary and next action

The source-shaped first transaction is closed.  Runtime reachability remains
unproved because the current immutable product has only bounded liveness
evidence.  The next product repair must arise from a complete owner cohort;
only then may a newly linked EXE receive one observation in the unchanged
`runtime-r33-config-system-media` container.  NTIO/NTDOS/EXEC instrumentation
and any observer variation remain outside this record.
