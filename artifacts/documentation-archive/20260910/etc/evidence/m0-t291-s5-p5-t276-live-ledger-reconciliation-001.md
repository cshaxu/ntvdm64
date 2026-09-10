# M0 T291 S5 P5 — T276 live-ledger reconciliation

## Question

How does the completed immutable T276 final-integration audit become an input
to the current MVDM/OpenNT package-boundary tracker without creating a second
mutable authority or silently reclassifying an original file?

## Inputs

- `m0-t276-final-integration-disposition-ledger.tsv` — 1,689 selected
  non-guest MVDM paths, each with a final non-`unresolved` disposition;
- `mvdm-file-recovery-ledger.tsv` — the live identity/provenance ledger;
- T276 closure and taxonomy records; and
- the T291 S5 P5 boundary-audit plan.

## Procedure

The reconciliation joins on the immutable `MVDM-FILE-*` identifier only.
It copies T276's final disposition, owner/link boundary, change class, named
adapter, mapping consequence, evidence and audit state into new live columns.
It does not alter the selected source path, hash, package root, file kind or
mirror-identity fields. The ten external OpenNT evidence-import rows have no
T276 counterpart and remain explicitly `unresolved-p5-external-audit` until
their complete package/slice review reaches a final result.

For the live four-class composition view, the deterministic projection is:

- `adapter-backed` becomes `exact-adapter-bound`;
- `binding-only` and `overlay-required` become `minimal-binding-diff`, with
  T276's change class retaining the distinction; and
- `firmware-only`, `guest-only`, `not-host-runtime` and `tool-only` become
  `mirror-only/profile-excluded`.

This is an expected final composition projection, not a claim that any source
body is currently compiled or linked. T276 records no `direct` MVDM path.

## Observation

The live ledger contains 1,699 rows: the same 1,689 `MVDM-FILE-*` identifiers
as T276 plus ten external OpenNT evidence-import rows. The immutable ledger
has exactly one row for every MVDM identifier and no duplicate key.

## Result

The current tracker receives all T276 final fields for every selected MVDM
path while retaining T276 as immutable historical evidence. Remaining external
rows are visible rather than being misreported as already classified.

## Follow-up

P5 must complete package/slice and reached-interface dispositions for the ten
external rows, then reconcile package, build and reversion ledgers before
stopping for owner review.
