# M0 T310 S8 P4 — Whole-tree static ABI audit baseline

## Scope

This is the source-first baseline for every retained original source under
`src/mvdm-host` and `src/mvdm-support`. It supersedes a warning-driven,
single-package repair order. A compiler warning is only an audit lead; no
candidate is declared safe or assigned an implementation solely because it did
or did not produce a warning.

## Inventory method

`tools/audit/Export-T310WholeTreeAbiInventory.mjs` reads all `.c`, `.h`,
`.inc`, `.asm`, and `.s` files in those two roots, including split-line
historical `IFN`/`IPT` forms. It records source candidates for:

- function-pointer declarations, table slots and typedefs;
- indirect function-pointer calls;
- explicit pointer-to-fixed-width and fixed-width-to-pointer conversions; and
- x86/x64/i386 conditional branches.

Each record deliberately requires review of its declaration, assignment or
initializer, selected callers and original-package context before a final
disposition. The TSV is therefore a complete candidate census, not a safety
claim or a generated change list.

## Initial census

The first normalized scan covers **1,412 source files** and emits **1,961
unique candidates**:

- 490 function-pointer declarations or table slots;
- 1,244 indirect calls;
- 41 explicit pointer-to-fixed-width conversions;
- 9 explicit fixed-width-to-pointer conversions; and
- 177 architecture conditionals.

The largest owner-package clusters are `softpc.new` (1,282), `wow32` (200),
`ieuvddex` (162), `vdmexts` (104), and `dpmi32` (68). This count intentionally
includes selected and unselected historical packages: their original profile
and final product disposition must be decided before repair work begins.

The implementation-level review view deliberately subdivides the selected
`softpc.new/host/src` files instead of treating them as one generic host
bucket. It currently produces 35 review clusters, separating the original
CCPU access table, memory mapping, system interrupt, display, input, storage,
communications, configuration, BOP and VDD binding families. This is an audit
partition only: it does not change package ownership or authorize a new
adapter.

`Export-T310WholeTreeContractReviewIndex.mjs` joins the source census and
unsuppressed compiler ledger into one source-file × responsibility-cluster
checklist. Its initial 360 rows cover every retained source file with a static
ABI candidate or a source-located compiler diagnostic. Files with neither are
not falsely represented as an ABI review task. Each row remains pending until
the complete original declaration/assignment/caller contract is read.

The first selected-cluster subledger,
`m0-t310-s8-p4-ccpu-access-table-ledger.tsv`, makes the full original CCPU
selection table reviewable as one 62-slot contract rather than a few reached
warnings. It joins original slot declaration, initializer, generated CCPU
declaration and public declaration, then flags only non-equivalent type or
prototype contracts after documented historical alias normalization.

## Review order

1. Classify each retained package as selected source, a source-bearing but
   unselected profile, tool/debugger-only, or non-runtime evidence.
2. For each selected package, group declaration, assignment and indirect-call
   records into complete original interface contracts.
3. Read every explicit fixed-width crossing and identify guest numeric address,
   private SoftPC pointer, host object/handle, or scalar value. Only a true
   guest or physical crossing may use the session mapping manager.
4. Generate x86/x64 compiler diagnostic ledgers per complete selected package
   and compare them with the source clusters. Diagnostics expand coverage; they
   do not replace source review.
5. Decide one complete repair disposition per cluster: exact mirror,
   source-derived generated overlay, named same-shaped adapter, or original
   unavailable/not-selected. Only then implement in package-sized batches.

## Current status

No candidate in the TSV is yet a completed audit result. The previously
generated C-video/base-video carriers remain uncommitted, provisional inputs
to the `softpc.new` cluster review. They must be retained, revised or rejected
only after the full selected SoftPC pointer/callback cluster is reviewed.

## Compiler cross-check baseline

`Export-T310WholeTreeCompilerWarningLedger.mjs` independently records every
source-located diagnostic emitted by the formal selected
`original-softpc-candidate` rebuild. Before the policy change, a clean build
recorded a limited 1,570 source-located diagnostic occurrences because older
per-target build scripts suppressed warning families. The first clean rebuild
after removing every project-owned suppression records **40,587** source
diagnostic occurrences. The ledger preserves each source location, architecture
and diagnostic, plus `occurrence_count`; it does not discard repeated inclusion
of one original header. The raw build logs remain the authoritative record for
any diagnostic that cannot be normalized to a project source location.

The most material source-review leads are not the numerically largest warning
classes. `C4311`, `C4312`, `C4057`, `C4047`, `C4024`, and `C4113` identify
possible native-width or callback-contract errors; in this baseline they occur
principally in the complete CCPU executor, host-platform, input-controller and
firmware clusters. `C4005`, `C4244`, old-style-declaration, uninitialized-flow,
unused-label/variable and precedence warnings remain recorded too: each is
classified from the original source meaning, never mass-suppressed.

The compiler ledger has no final dispositions. A row is closed only when its
entire original interface contract has been read and reconciled with the
static candidate inventory.

## x86/x64 relevance classification

The x86 and x64 formal build logs are retained as source-location evidence.
`Export-T310WholeTreeWarningClassification.ps1` produces a complete
per-location classification rather than treating every compiler warning as a
required source change. The compatibility worklist contains only diagnostics
whose type/architecture combination can affect pointer width, fixed-width
value narrowing, call ABI, undeclared/old-style call contracts, result flow,
or an x64-only unused formal that may conceal such a contract.

All other diagnostics remain visible in the raw ledger, but are not a reason
to alter an original mirror, add an adapter, or change warning policy during
this S. A selected record is repaired only after its original declaration,
initializer and caller prove an x86/x64 semantic impact.

## Repair decision rule

For every complete original interface contract, the reviewer selects exactly
one of these dispositions and records the reason:

- **minimal mirror correction** — a small, source-proven declaration or
  expression correction that preserves the original function and call shape;
- **overlay implementation** — the original function/control flow remains a
  mirror, but a non-trivial native-width implementation belongs outside that
  file and is reached through a minimal registered divergence point;
- **mapping-manager crossing** — only when a value is genuinely a guest or
  physical-memory address crossing into the selected machine; private SoftPC
  pointers, host pointers, handles, and function pointers must not be turned
  into mapping IDs; or
- **not selected/unavailable** — the original profile or product facility is
  outside the selected non-invasive machine closure.
