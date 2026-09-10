# M0 T397 closure — WOW16 dialog and message boundary

## Outcome

T397 closes its approved WOW16 service/UI boundary scope. It does **not**
claim that WRITE opened a document window, accepted application input,
completed a Win16 message loop, or exited successfully.

The Win32/x86 product has a continuous ordinary, no-diagnostic path through:

```text
DPMI32/DOSX bootstrap
  -> WOW32 load and W32 dispatch
  -> source-shaped class/window/menu/cursor projection
  -> WRITE FInitWinInfo shared error path
  -> original "Not enough memory for Write..." #32770 dialog
```

The only current product and acceptance executable is `ntvdm32.exe`. Native
x64 is not built, staged, run, accepted, or repaired by this closure.

## Recovered boundary

T397 preserves original call shapes while providing only the smallest
public-Win32/session-owned seams needed to reach the boundary:

- copied opaque class/window identity and callback state;
- projected 16-bit window identity recovery only at public USER32 entry;
- source-ordered menu conversion through `LoadMenuIndirectW` after original
  resource materialization;
- public cursor/icon construction from the original DIB carriers; and
- session-scoped GDI token projection through the existing host-resource
  mapping manager, rather than an NT4 shared GDI table.

These seams permit original WRITE class registration and `MSWRITE_MENU`
creation, including a native `HMENU`. They neither recreate private
USER/CSRSS/Win32k state nor fabricate a Win16 task scheduler.

## Error-dialog disposition

The visible top-level `Write` window is not a document surface. Passive
ordinary capture identifies it as the original `#32770` error dialog whose
text begins "Not enough memory for Write to complete this operation." Its
sole action is `OK`. Earlier menu/close/input experiments are therefore not
WRITE application-lifecycle evidence.

The shared `FInitWinInfo` error label is source-audited. Isolated external
success-continuation controls cover early bootstrap allocations, stock/resource
cursor and accelerator guards, `MmwCreate` child controls, and post-parent
`ObjInit`, `FInitArgs`, `fMessageInzFailed`, `ValidateMemoryDC`, and
`InitCommDlg` result guards. Retaining the same ordinary dialog after a forced
fall-through excludes only unique attribution to that guard; it never proves
that the forced path normally succeeds.

The `InitCommDlg` forced continuation reaches a later DOSX `PMIntrReflector`
GPF. The external CPU40/SAS/LDT sampler maps it to staged, source-identical
DOSX code with paging off and dynamic `Start_of_M_area` backing. It is neither
WRITE code nor an ordinary DPMI32/WOW32 failure, and selects no CPU, BOP,
DPMI, DOSX, or WOW32 repair.

## Explicit boundary

The sole admitted module-qualified live-return mechanism, VDMDBG, receives no
`STATUS_VDM_EVENT` initialization in this custom CCPU container. It cannot
resolve the WRITE module selector and is diagnostic-only in any case. CCPU
decoder-side file, event, and broad instruction probes also perturb the
ordinary path and are rejected. The exact ordinary-return value is unavailable
without crossing S4 stop conditions into invasive guest/CPU tracing or a
broader UI/runtime owner.

This is an explicit unavailable boundary, not evidence that the original
low-memory result is correct, that allocation is exhausted, or that a
replacement is warranted.

## Evidence and successor

The consolidated observations, source maps, reversible-control limits and
sampler result are in [the S4 cursor/resource frontier](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t397-s4-cursor-resource-frontier-001.md).
The next queue candidate is unified app-owned error interaction in
[QUEUE.md](../states/QUEUE.md). It requires a new numeric T/S admission before
implementation and owns future custom public-Win32 error presentation and
response policy. T397 authorizes no further UI, guest, CCPU, BOP, DPMI32, or
x64 work.
