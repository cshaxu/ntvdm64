# Pre-Governance Coding Standard Record

> **Superseded as the current coding authority on 2026-08-09.** Use
> [rules/CODING.md](rules/CODING.md). This detailed record remains supporting
> evidence during the controlled documentation migration.

## Primary Rule

Treat historical NT code as evidence first and editable source second. The default implementation path is an external adapter or build-layer fix, not a behavioral rewrite of MVDM or NTDOS code.

## Change Discipline

- Keep host-facing code, compatibility shims, generated configuration, and imported historical source in clearly separate directories or files.
- Make the smallest change that permits the required build or observed runtime behavior. Avoid style cleanups mixed with compatibility work.
- Preserve calling conventions, structure layout, segment assumptions, error propagation, and dispatch order unless evidence justifies change.
- State the historical source path and reason in the evidence record for every edit to imported material.
- Prefer compiler flags, wrapper translation units, compatibility headers, and build recipes before editing legacy source.

## Build And Runtime Boundaries

- The CLI host validates input and performs host duties; it does not silently replace historical DOS semantics.
- DOS and NE execution remains explicit BYOB engine behavior until supported engine integration is documented.
- Preserve the historical chain where applicable: `CCPU -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM -> NTDOS`.
- API or ABI gaps are research findings. Record the first missing boundary, caller, and smallest useful probe before introducing broad compatibility stubs.
- `ntdos64` must not fill a CPU/machine gap by assembling a parallel general
  x86 VM. Before adding instruction execution, segment/A20/ROM semantics,
  generic memory topology, interrupt/timer delivery, device controllers, or
  media backends, first pursue matching historical CCPU recovery. Promote a
  neutral requirement to `ntvdm64/core` only after a source-recovery record
  proves that the relevant CCPU surface is materially missing or cannot be
  made trustworthy without invented behavior. A local contract fixture may
  fail closed, but it may not grow into an execution backend.

## Verification

Every code or build change needs a focused command and expected result. For shared ABI, dispatcher, loader, or CLI changes, include a negative case proving that unsupported input is rejected rather than succeeding by accident.
