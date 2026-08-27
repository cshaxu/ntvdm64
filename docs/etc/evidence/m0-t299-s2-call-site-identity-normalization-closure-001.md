# M0 T299 S2 — first-degree call-site identity normalization closure

## Correction applied

T299 S1's 801 rows are retained only as a callee-spelling index. They do not
represent final function identities. S2 resolves every remaining raw external
call individually and treats every original implementation location as a
distinct function, even for identical spellings, signatures or file bytes.

## Verified ledger invariants

- All 2,819 T298 raw external calls have one or more rows in
  `mvdm-host-first-degree-call-implementation-ledger.tsv`; none is missing.
- The original definition ledger contains 76 physical implementation
  identities. The composite source-root/path/hash/definition-line/signature
  key is unique for all 76 rows.
- 200 call-to-implementation rows are include-constrained matches; 158 are
  original-tree discovery candidates outside that include scope and remain
  unselected; 22 call rows have no original C/C++ implementation body; 2,578
  rows are public or explicit historical terminal boundaries.
- All 528 second-degree initial candidates name one of the 76 concrete
  implementation IDs as their caller. No name-only interface group is a
  second-degree caller.

## Boundary

The eleven no-body spellings remain declaration, assembly or historical
boundary evidence. Discovery-only implementations remain candidate locations,
not selected providers. Neither case authorizes adapter work or source import.
The resulting call-to-implementation ledger and second-degree initial ledger
are the valid first-degree closure; T299 S1's former 63-definition/481-edge
figures are superseded.
