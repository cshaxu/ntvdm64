# Bochs-First OpenNT Owner Closure Decision

## Finding

The repository contains a successful, default-disabled i386 fixture for the
original route `BIOS[50h] -> MS_bop_0 -> DemDispatch(11h) -> demLoadDos`.
It is historical-owner evidence only: its source closure includes CCPU/SAS
and the complete historical `BIOS[]` table, so it cannot be promoted into the
Bochs-first runtime.

The full original `BIOS[]` closure is currently blocked by the C-VIDC generic
81-slot `Video` composition. The audited source proves that `C_Video` and the
generic `Video` are distinct storage objects and that `ev_glue.c` requires the
latter's real providers. No authoritative source/build artifact establishes
the missing generic storage/initializer/lifecycle binding. The documented
prohibitions therefore apply: no `Video = C_Video`, short table, fake storage,
fake handler, or reduced BIOS table may be used to make the link succeed.

This is a missing historical composition boundary, not a Bochs CPU, memory,
or device defect. Expanding Bochs cannot repair it.

## Consequence

The current hard rule that the first service must enter the complete original
`BIOS[]` table cannot be satisfied from the available evidence without
reconstructing unknown SoftPC composition. Continuing that route would be a
new monitor/composition project and would contradict the minimal-Bochs plan.

The already verified generic `#UD` state bridge and write preflight do not
solve this; they deliberately contain no BOP or OpenNT service policy.

## Approved Narrow Selection Exception

On 2026-08-10 the owner approved the missing selection mapping to be
recovered in the adapter, with the historical BIOS-array form as evidence.
The implementation is limited to the exact `0x50 -> MS_bop_0` identity
record in `adapter-bios-selector-map-v1.md`. It replaces only the unavailable
`BIOS[50h]` function-pointer selection. It does not recreate `BIOS[]`, carry
a pointer, decode a BOP, invoke OpenNT, or admit another selector.

All other selector values explicitly fail lookup. The generic-`Video`
composition blocker is therefore still retained rather than worked around.

## Remaining Owner-Entry Decision

There are only two evidence-honest choices:

1. **Remain strict.** Keep the complete `BIOS[]` requirement. The project
   remains at the mechanical bridge gate until authoritative generic-`Video`
   composition input is recovered. No guest owner can be connected.
2. **Authorize owner entry after the admitted selection.** Let a future isolated OpenNT service
   island invoke the exact original `MS_bop_0` owner after the adapter has
   independently and exactly identified an admitted `C4 C4 50` boundary.
   `MS_bop_0` still owns reading the following service byte, `DemDispatch`,
   the original IP increment, and idle/termination behavior. The exception
   bypasses only the historical `BIOS[50h]` function-pointer table selection,
   whose complete static closure is unavailable; it must not create a selector
   switch, service switch, replacement `DemDispatch`, or synthetic DOS return.

This second choice avoids the `BIOS[]`/generic-`Video` blocker, but it is not a
small one-function link shortcut. `MS_bop_0` calls the original
`DemDispatch`, whose `apfnSVC[]` static table has 73 entries (including its
historical repeated not-implemented slots) and therefore
requires the full original DEM service/object closure even when runtime input
is only service `11h`. It also needs a state and ordinary-memory provider for
`get*`, `set*`, and `Sim32GetVDMPointer`. Those providers must remain an
isolated, typed transaction island; importing CCPU execution, reducing the
table, or substituting a service dispatcher remains prohibited.

The selector recovery is approved, but connecting it to an owner transaction
still requires an exact instruction-inspection contract and a new registered
exception before code. It is not a claim
that the full historical BIOS composition or DEM closure has been restored.
Choice 1 preserves strict fidelity but leaves the current vertical slice unable
to reach NTIO's first owner.

## Evidence

- `docs/S5-ORIGINAL-DEMLOADDOS-CLOSURE.md`: existing original-owner fixture;
- `docs/research/ccpu-bios-closure/S5-TRANSPORT-V2-BASELINE-REJECTION.md`:
  non-promotable CCPU transport and full-table requirement;
- `docs/research/softpc-source-matrix/NT4-CVID-DISPATCH-AND-STORAGE-AUDIT.md`
  and `NT4-GENERIC-VIDEO-RECOVERY-CANDIDATE.md`: missing generic-Video
  composition and rejected substitutions;
- `src/opennt/base/mvdm/softpc.new/host/src/nt_bop.c` and
  `base/mvdm/dos/dem/demdisp.c`: original `MS_bop_0` and dispatch ownership.

## Closure Measurement

On 2026-08-10 the initializer in `demdisp.c` was mechanically enumerated:
73 table entries refer to the historical DEM service set. This is why a link
that calls only `DemDispatch(0x11)` still cannot omit the other entries: their
function addresses are materialized in the original table. Replacing it with a
single-entry table, a direct `demLoadDos` call, or a local selector/service
switch would cease to execute the original dispatcher and is not an available
implementation option.
