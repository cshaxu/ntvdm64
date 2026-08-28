# M0 T300 S1 — MVDM-Local Closure Preliminary Result

## Question

Can the source graph apply the owner-defined rule that every continuous call
reachable from `mvdm-host` remains zero-degree while its resolved original
definition is still inside OpenNT `base/mvdm`?

## Inputs

- all original source mirrors under `mvdm-host`, `mvdm-support`, `mvdm-tools`,
  `mvdm-softpc-firmware`, and `mvdm-guest`;
- selected local OpenNT and OpenNT-4.5 source roots;
- closed T298/T299 degree ledgers; and
- `export-t300-s1-mvdm-local-closure.mjs`.

## Procedure

The scanner hashes and provenance-matches every local MVDM source file to a
selected original source file. It seeds all definitions in `mvdm-host`, follows
same-file or uniquely resolvable externally-linkable calls only while their
physical definition remains within `base/mvdm`, and emits the first unresolved
or outside-universe calls as the new first-degree boundary. Same-name multiple
definitions are retained as ambiguities rather than selected by spelling.

## Observations

- 14,295 unique provenance-resolved original definitions are present across
  the project MVDM mirrors.
- 12,148 definitions are `mvdm-host` roots.
- 12,392 definitions are in the reachable MVDM-local zero closure: 244 were
  reached through the continuous in-MVDM call chain.
- No local source file lacks a source identity. Twenty-two same-path changed
  mirror files are explicitly paired with their selected original bodies for
  this source audit; they do not silently contribute their modified body.
- 34,234 direct call occurrences currently leave the unique MVDM-local
  closure and are recorded as first-degree boundary candidates.
- Those occurrences are now governed by a 3,907-row deduplicated
  first-degree function-candidate ledger. It preserves every raw caller and
  candidate ID. 690 spellings carry forward T299's existing interface-audit
  evidence; 3,217 are newly exposed by the corrected zero boundary and remain
  physical-implementation-identity pending.
- 3,995 internal call occurrences have more than one same-spelled physical
  OpenNT MVDM definition. They remain unresolved; choosing one by name would
  violate the owner rule.
- The historical T299 first/second ledgers have no physical implementation
  row that directly intersects this closure. This confirms that the prior
  records were already derived from a different, external frontier; it does
  not remove the need to replace their planning boundary.

## Interpretation

The new root definition is implemented and the former root-only audit is no
longer used for planning. S1 is not closed: include/declaration-constrained
resolution is required for the 3,995 ambiguous MVDM-local edges before the
first-degree boundary can be called complete. The unambiguous 244 promotion
count is evidence of the intended small reclassification, not evidence that
all same-name calls may be automatically promoted.

## Follow-up

Continue S1 with original include/declaration scope resolution for ambiguous
in-MVDM call edges, then regenerate the final zero and first degree ledgers.
