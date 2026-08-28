# M0 T300 S1 — MVDM-Local Call-Closure Rebaseline Plan

## Admission

The owner corrected the zero-degree source-graph boundary: all original
definitions in `mvdm-host`, plus their continuous resolved call closure while
the target remains in selected OpenNT `mvdm` source, are zero-degree. The first
resolved target outside `mvdm` is one-degree. Existing but unreachable local
MVDM code does not enter this closure.

## Objective

Rebaseline the derived T298/T299 degree ledgers by physical definition identity
and publish the exact zero-degree closure/reclassification and boundary rows.

## Method

1. Inventory eligible original definitions already mirrored below project
   `mvdm-*` mirrors, excluding overlays and declaration-only roots.
2. Begin with all `mvdm-host` definitions and resolve their transitive call
   closure only while each target is selected original OpenNT `mvdm` source.
3. Record every promoted definition, predecessor call, component ownership,
   source identity and any ambiguity or missing target.
4. Emit only calls which leave this closure as first-degree. In particular,
   calls formerly recorded as second-degree only because their MVDM caller was
   misclassified as first-degree return to this first-degree ledger.

## Non-goals

No source import, provider selection, adapter implementation, compilation,
runtime link edge, or change to tool/firmware/guest final disposition.

## Expected Outputs

- a project-MVDM definition identity ledger;
- a zero-degree closure/reclassification ledger;
- a regenerated first-degree boundary ledger; and
- concise evidence comparing the former and rebaselined boundaries.

## Verification

Review every promoted row for selected physical original identity, confirm no
overlay or `src.old` path appears, run documentation inventory/governance
checks, and run `git diff --check`.
