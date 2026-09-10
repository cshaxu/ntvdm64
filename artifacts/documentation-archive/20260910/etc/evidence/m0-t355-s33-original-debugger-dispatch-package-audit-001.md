# M0 T355 S33 — original debugger dispatch package audit

## Package boundary

The original MVDM `dbg` runtime package is one source file:

```text
mvdm-host/dbg/dbg.c
```

Its original `sources` file identifies it as the `mvdm/dbg` library.  The
nearby `bde`, `vdmdbg`, and `vdmexts` trees are debugger tools/extensions, not
the selected SoftPC startup dispatch library.  They are therefore not a
runtime dependency of the reached `MS_bop_6` entry.

## Reached original path

The original BOP table assigns selector `56` to `MS_bop_6` in
`softpc.new/base/bios/bios.c`.  Its source body is exactly:

```text
MS_bop_6 -> DBGDispatch
```

The fixed S32 observation identifies this exact `DBGDispatch` entry as the
`ERROR_CALL_NOT_IMPLEMENTED` terminal.  The observer intentionally logs only
selected DEM/COMMAND BOP services; the absence of a literal `56` line from
that report is not evidence that the BOP did not occur.

The original Win16 caller supplies a decisive static classification:

```text
mvdm-guest/wow16/kernel31/wow16cal.asm::WOWNotifyTHHOOK
  push PMODE32
  push seg THHOOK
  push offset THHOOK
  push DBG_TOOLHELP
  FBOP BOP_DEBUGGER
```

Thus the ordinary Win16 bootstrap uses mode `DBG_TOOLHELP` (15).  In original
`dbg.c::DBGDispatch`, that mode only stores the 16:16 ToolHelp hook and the
16/32-bit flag.  It does not call `SendVDMEvent`, CSRSS, BaseSrv, a debugger
DLL, or a debugger transport.

## Original function dispositions

- `DBGInit` has already been independently restored through the same original
  `ProcessDebugPort` query/result direction.  Its current state must become
  the shared state used by the remaining source-shaped dispatch functions.
- `DBGDispatch` is a mixed entry.  `DBG_TOOLHELP`, `DBG_ATTACH`,
  `DBG_MODLOAD`, and the unknown-mode `AX=0` path have no private debugger
  transport dependency.  Stack decoding does require the selected CPU40
  `SS:SP` and `mvdm_softpc_effective_address` contracts plus a bounded,
  synchronous session guest-memory lease.
- `SegmentLoad`, `SegmentMove`, `SegmentFree`, `ModuleLoad`,
  `ModuleSegmentMove`, `ModuleFree`, `SingleStep`, `Breakpoint`, `GPFault`,
  `DivOverflow`, `DllStart`, `TaskStop`, and `DBGNotifyNewTask` retain original
  no-event/default-false behavior whenever `fDebugged` is false.  Their
  debugged branches build `VDMCONTEXT` and deliver `STATUS_VDM_EVENT` through
  the NT4 debugger/CSR product; that transport is outside the ordinary
  profile.
- `DBGNotifyRemoteThreadAddress` and `DBGNotifyDebugged` are WOW-side state
  setters.  They do not require a transport by themselves, but their only
  selected consumers are debugged/WOW paths and must retain their original
  state direction for later WOW recovery.
- `SendVDMEvent` is the true non-recoverable private transport boundary for
  this package: it raises the historical `STATUS_VDM_EVENT` debugger event.
  No current public Win32 API supplies the same VDM-debugger broker contract.

## Bounded successor

S34 may replace the over-broad `DBGDispatch` unavailable terminal with the
complete original **ordinary non-debug** disposition: shared debug-state,
source-order stack decode, `DBG_TOOLHELP` registration, original no-event
outcomes, and exact `AX=0` default result.  It must use the existing session
mapping-manager lease for the finite guest frame rather than publish an alias.
The debugged `SendVDMEvent` cohort remains explicitly unavailable; S34 must
not add a synthetic event, debugger broker, or BOP-specific fabricated
success.
