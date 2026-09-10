# M0 T301 S1 P13 — Rebased MVDM Zero / First-Degree Frontier

## Corrected definition

Zero degree comprises original MVDM functions reachable from `mvdm-host` by a
continuous chain of confirmed original-MVDM calls.  First degree begins only at
direct calls leaving that confirmed closure, plus any internal candidate whose
source shape/frontier is not yet proven.

## Rebase result

The rebase consumes 202 confirmed old-boundary calls and 80 confirmed expanded
internal edges.  It emits:

- 82 newly admitted original-MVDM zero-degree definition identities;
- 12,224 retained former first-degree calls;
- 223 direct exits from newly admitted MVDM bodies; and
- 40 unconfirmed internal candidates retained visibly at first degree.

The resulting physical first-degree ledger therefore has **12,487** rows.
The arithmetic is explicit: `12,426 - 202 + 223 + 40 = 12,487`.

## Guarantees

The rebase retains original path, hash and line identities.  It neither merges
same-spelling functions nor claims a runtime provider.  The 40 unconfirmed
internal calls are deliberately kept at first degree; they are not losses or
silent zero-degree admissions.

## Next work

T301 must classify every row of the rebased 12,487-call frontier and validate
the remaining 40 internal candidates.  P13 replaces the prior frontier only
as the active audit input; it does not change product code or build selection.
