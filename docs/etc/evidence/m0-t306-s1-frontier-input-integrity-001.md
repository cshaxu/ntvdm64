# M0 T306 S1 P1 — External-frontier input integrity

## Question

Do the existing MVDM host closure ledgers form a physical-identity-preserving
input for finite external-boundary package selection, without restarting the
source scan or merging same-named functions?

## Inputs

- `docs/etc/operations/zero-ledger1.tsv`
- `docs/etc/operations/one-ledger1.tsv`
- `docs/etc/operations/one-ledger1-definition-resolution.tsv`
- `docs/etc/operations/one-ledger1-declaration-binding-ledger.tsv`
- `docs/etc/operations/one-ledger1-signature-gate.tsv`

## Procedure

Read the TSV headers and row counts, group only the existing physical rows by
their recorded binding/result fields and caller source path, and compare the
result with the retained P18--P20 evidence. No callee body was read and no
row was merged or reclassified.

## Observations

- `zero-ledger1.tsv` contains 3,720 selected physical zero-degree definition
  identities (3,721 lines including its header).
- `one-ledger1.tsv` contains 7,686 physical direct calls:
  - 7,367 have `outside-zero-ledger1` and are the true direct external
    frontier;
  - 319 have `zero-ledger1-binding-ambiguous`. They remain source-identity
    exceptions, not external package candidates.
- Of the 7,367 external calls, P18 records 4,262 with one or more original
  C/C++ source definition candidates and 3,105 with no such candidate.
- P19 retains 310 unique source/declaration candidates; P20 confirms physical
  call/declaration/definition arity for 251 of them. These are eligible
  package-boundary seeds only, not selected providers.
- High-volume caller groups are already visible without new scanning:
  `vdmredir/vrnetapi.c` (311), `wow32/wkman.c` (297),
  `vdmredir/vrnmpipe.c` (282), `wow32/wgdi.c` (230), and historical
  SoftPC-host control files. They establish owner cohorts but do not determine
  a provider disposition by themselves.

## Interpretation

The ledgers are suitable for T306: they retain every caller physical identity,
source hash, call line, linkage and candidate relationship needed for a
bounded package decision.  The 319 ambiguity rows must first be resolved as
zero-degree legality exceptions; they cannot be silently assigned to an
adapter.  The 251 signature-confirmed rows are the only currently safe seeds
for original non-MVDM package closure inspection.  Public/source-absent
classification remains a required T306 work item for the 3,105 no-body rows.

## Confidence and follow-up

High confidence for counts and provenance because they are direct TSV facts.
No runtime, source-body or provider conclusion is implied.

The next T306 step partitions the 251 eligible source candidates by original
package root and follows each package only to the declared stopping boundary.
In parallel it classifies the source-absent cohort into public Win32/CRT,
macro/assembly/import-only, small closure or hard-boundary groups. It must not
reopen global name-based resolution.
