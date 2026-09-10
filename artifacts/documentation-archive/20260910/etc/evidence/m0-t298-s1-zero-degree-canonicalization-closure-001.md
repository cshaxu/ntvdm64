# M0 T298 S1 — zero-degree canonicalization closure evidence

## Scope

T298 derives a canonical view from, but never alters, T295's raw source audit.
The raw definition ledger remains the identity evidence: every one of its
11,872 source/path/hash/line/linkage rows has exactly one canonical-definition
row.  The raw first-degree candidate ledger likewise remains immutable.

## Source-shaped normalization

The generator reads only the selected original MVDM source bytes named by the
file recovery ledger.  It recognizes a Ctags macro-definition tag as a
function only when the source line invokes an all-uppercase function-definition
macro whose first argument is the source-declared function name.  The
canonical row preserves the raw tag spelling, macro definition location and
the exact source-line evidence.  Ordinary functions that happen to share a
conditional macro spelling are not renamed.

This recovered 175 macro-generated definitions, including the historical
`YODA_COMMAND(name)` form, without merging file-static definitions across
source files.

## Result

- 11,872 raw zero-degree identities map one-to-one to 11,872 canonical rows.
- 10,928 canonical groups result: 10,095 ordinary singletons, 175
  macro-generated singletons, 655 product/conditional variant families and
  three same-source static redeclaration/variant families.
- All 2,821 raw first-degree call sites have a resolution row.
- Two raw candidates are now source-proven canonical zero-degree calls after
  macro normalization; the remaining 2,819 call sites form 801 distinct
  external interface spellings.
- The 801 external spellings retain their T295 initial classification: 362
  public Win32/CRT leaf candidates and 439 candidates for the separate
  first-degree source audit.  T298 does not select an implementation,
  adapter, package or second-degree dependency.

## Reproducibility and boundary

Run `node tools/governance/export-t298-s1-zero-degree-canonical-ledgers.mjs`
from a Git Bash environment with Node 22.  It regenerates four derived TSVs:
the canonical definition and group ledgers, the canonical external-interface
ledger, and the complete raw-call resolution ledger.  It throws if raw-row or
call-site coverage is lost.

No product source, adapter, build graph, original function body, guest input
or `src.old` is consumed or changed by this packet.  The resulting 801
external spellings are the only valid input frontier for the subsequent
first-degree audit.
