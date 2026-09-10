# Td S2 P4 — DEM and COMMAND Interface-Conformance Audit

## Question

Do every tracked DEM/COMMAND BOP and their direct OpenNT dependencies preserve
the smallest original source interface possible, and which ones must migrate
to the one guest-pointer mapping manager before their next source-path change?

## Inputs

- `src/bx-vdm/bop/opennt/dem/demdisp.c`: original 73-entry `apfnSVC` table.
- `src/bx-vdm/bop/opennt/command/cmddisp.c`: original 17-entry `apfnSVCCmd`
  table.
- The imported DEM/COMMAND source files and their named shim headers.
- `BOP-DEPENDENCY-116` / `BX-VDM-001` and the Td shared-pointer governance.

## Procedure

1. Enumerated every original dispatcher slot and compared the active imported
   function with the associated named shim.
2. Marked an entry `opennt-shaped-facade` only where the original source body
   and calling/layout shape are retained without a private guest-pointer
   representation.
3. Marked native-width host-handle representation only as the registered
   `BX-VDM-001` exception.
4. Marked every reached family-local `GetVDMAddr`/`Sim32*` mapping as
   `migration-debt`, even if existing local tests call it code complete.
5. Marked product-shell-only debug, WOW, console-input, Redirector and
   cross-DOS paths deferred rather than inventing a replacement.

## Results

The tracker overlay now covers exactly 123 P4 rows: all 73 DEM BOP entries,
all 17 COMMAND entries, and dependencies 001 through 033.

| Disposition | Count | Meaning |
| --- | ---: | --- |
| `opennt-shaped-facade` | 43 | Original source/table/form is retained through a bounded named seam. |
| `registered-exception` | 6 | The sole relevant custom representation is the approved opaque host-handle manager. |
| `migration-debt` | 64 | The source body is imported, but its family-local historical pointer helper must migrate to dependency 118. |
| `deferred` | 10 | The route needs an explicit later VDD/debug/WOW/Redirector/console/cross-DOS owner package. |

`migration-debt` is deliberately not a claim that an entry is unusable or that
its OpenNT owner was discarded.  It records the precise remaining composition
work: preserve the original call spelling and use the shared bounded mapping
lease instead of a family-private helper.  No route is newly enabled by this
audit.

## Verification

`tools/governance/Test-TdS2P4DemCommandAudit.ps1` proves that the bounded P4
overlay names all 90 BOP entries and all 33 direct dependencies exactly once,
and contains each required disposition class.

## Follow-up

P5 applies the same method to XMS and DPMI.  P7 must reconcile the resulting
321-row tracker and only then decide whether frozen T257 S5 can be re-admitted.
Before any P4 migration-debt BOP gains a newly expanded source route, its owner
package must migrate to `BOP-DEPENDENCY-118` and prove direct/bounce lease
lifetime and write-back/refusal behavior.
