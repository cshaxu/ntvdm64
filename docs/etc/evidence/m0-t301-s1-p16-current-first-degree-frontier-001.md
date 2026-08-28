# M0 T301 S1 P16 — Current First-Degree Frontier

## Corrected input

The first-degree frontier is now frozen at **12,694 physical call identities**
from 362 original zero-degree caller files.

The prior 12,723-row output was corrected: 29 edges had been emitted from
newly discovered MVDM bodies that did not pass zero-degree admission.  Those
bodies remain discovery evidence; they cannot create a first-degree call.

The current caller include closure contains 735 unique original header
identities.  Each physical call has one source-disposition row in
`operations/mvdm-first-degree-disposition-ledger.tsv`:

- 6,139 public CRT leaves;
- 1,462 private host hard boundaries;
- 358 MVDM variant/parser-gap records;
- 1,535 original non-MVDM body candidates requiring declaration/frontier and
  linkage binding before any body is selected;
- 351 macro or indirect forms;
- 115 declaration-only unresolved records; and
- 2,734 source-unavailable or declaration-only records.

The last four categories are not implementation decisions.  They are explicit
physical work remaining in T301.  In particular, no non-MVDM candidate body
has yet been selected or parsed, and no second-degree body was read.

## Verification

The frontier generator now rejects a caller absent from the lawful P15 zero
closure.  Its regenerated source map and declaration shape ledger have the
same 12,694 physical candidate IDs as the frontier.
