# M0 T420 S16 SubAlloc Package Recovery

## Scope and source-first result

S16 closes the selected `src/mvdm/suballoc` package.  Its formal OpenNT
`sources` manifest contains one production unit: `suballoc.c`.  That unit and
the private `suballcp.h` companion match the admitted OpenNT source after
normalized comparison; no allocator algorithm, lifecycle branch, or mirror
file was added or changed.

The original `tsa.c` program is retained as source evidence, not selected as
a standalone test: it reserves native virtual memory and stores its process
address in 32-bit `ULONG` values.  That is an NT4 native-address-space test,
not a valid CPU40 worker contract.  S16 instead introduces a test-only
fixture that calls the unchanged original state machine with numeric guest
linear values and callback stubs.  It proves initial free state, allocation,
blocked in-place growth followed by relocation through the original memory
mover, release/decommit, and restoration of the original free-state totals.

## Boundary disposition

`src/mvdm/inc/suballoc.h` retains the pre-existing
`MVDM-SUPPORT-DIV-007` selector:
`_M_IX86` selects the original 4096-byte x86 commitment granularity when the
modern MSVC target does not define the historical `i386` macro.  It cannot be
restored literally without selecting the 65536-byte non-x86 allocator shape.
Defining `i386` globally is not equivalent: XMS uses that same historical
symbol to select its excluded direct-host-pointer backend.  Thus this one
header condition is the minimal product-profile compatibility boundary; the
allocator body remains byte-identical in behavior and source shape.

The compiler's `Status` warning in original `CommitChunk` was audited.  Its
only callers pass the closed `SACommit` or `SADecommit` enum values, each of
which assigns `Status` before the check.  No invalid action is externally
reachable; adding a synthetic default branch would increase mirror diff
without changing a product path.

## Footprint and verification

- Mirror production implementation delta: **zero lines**; `suballoc.c` is
  normalized-identical to OpenNT.
- Adapter/autonomous production implementation delta: **zero lines**.
- Test-only addition: one lifecycle fixture and its Ninja generator; neither
  participates in the product link graph.
- Focused x86 fixture passed, including callback granularity, allocation,
  forced relocation, decommit, and free-state restoration.
- Fresh formal x86 `product-programs` built all five deliverables (503 Ninja
  nodes): `run16.exe`, `basesrv.exe`, `ntvdm.exe`, `dtmgr.exe`, and
  `VDMREDIR.dll`.
- The artifacts were hash-verified after deployment to `O:\winnt`.
  Clean bounded `run16.exe MEM.EXE` and
  `run16.exe COMMAND.COM /C MEM.EXE` each returned `0`.

The owner-accepted interactive COMMAND/MEM/EDIT route remains the established
product baseline.  S16 has no production behavior change; its new focused
fixture plus fresh direct regressions verify the recovered package without
misrepresenting manual interaction as an automated result.
