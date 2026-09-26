# Proposal — Original-source implementation minimization review

## Objective and admission

Owner-requested candidate: **implementation minimization review**
(实现最小化复审). Audit ALL project-owned implementations for duplication of
behavior already provided by original OpenNT, including the existing mvdm
and opennt-host mirrors, then consolidate those implementations back to their
original owners wherever composable. This is audit AND implementation, not
an inventory-only task and not a review limited to one historical delivery.
Preserve all demonstrated working capabilities. Original OpenNT remains authoritative: correct local bindings
to the original contract, never change the original contract merely to fit
local code. A semantic correction to original source requires a demonstrated
original bug, a minimal repair and focused positive/negative proof.

This is a queued candidate, not an active numeric task. Allocate its T number
only at admission through CURRENT.md. It does not interrupt or replace the
active package. The owner's queue request authorizes this proposal, not its
implementation before admission.

This is the final queued review, after
[whole-closure recovery](proposal-mvdm-whole-closure-recovery-001.md). Consume its
shared inventory, stable finding/source-owner IDs, source/build identities and
tests rather than commissioning another fresh global discovery pass. S1
reconciles full coverage but researches changed, unclassified or contradicted
rows; S5 independently checks dispositions and post-change selections, reusing
valid unchanged evidence. No owner is exempt from coverage.

The distinction is missing capability versus unnecessary implementation:
whole-closure recovery owns missing/cropped/unconnected originals and the
duplicates displaced by recovery; this review owns remaining duplication and
unnecessary local code across working capabilities. Newly discovered capability
gaps receive an explicit original-owner repair disposition through governance,
not a silent expansion or an unowned deferral. Earlier packages must already
minimize their own changes; this final review does not license temporary copies.

## Project-wide scope and baseline

Freeze the current repository revision and formal build selections at task
admission. Inventory every project-owned production body, copied fragment,
declaration/constant authority and source-selection shim, regardless of age,
directory name or previous task closure. Include:

- all executable components, their worker/client/server bindings and shared
  ABI/package components;
- adapters, compatibility shims, generated providers and any retained overlay,
  including bodies moved out of historical overlay directories;
- autonomous logic hidden inside mirror diffs in mvdm and opennt-host;
- selected and unselected implementations, separating live providers from
  dead duplicates, reference-only source and genuinely test-only doubles.

Compare both text and behavior with pinned original OpenNT and with both
existing mirrors. Match original owners, interfaces, state transitions,
failure behavior and lifetime, not merely matching names or lines. Existing
but unselected or conditionally excluded original functions are first-class
recovery candidates. Original code not yet mirrored must receive a source-
policy reuse/admission disposition, not be silently classified as absent.
This task does not permit project-invented mirror files or private kernel/CSR
runtime imports. Original OpenNT files may be imported at original paths under
the current source policy; evidence of a needed new admission must be raised
explicitly rather than disguised as justification for autonomous duplication.

Separate actual original-source bugs, local adaptation bugs, necessary modern
host mechanics, duplicate/copied original logic and diagnostic-only code.
Every candidate must be resolved by restore/delete/minimize/retain with source
and verification evidence. No package is exempt because COMMAND/EDIT already
works, it was closed earlier, or it contains no literal duplicate text.

The [S36 evidence](../etc/evidence/m0-t420-s36-suballoc-capability-review.md)
and changes from `6f5ae6690` to `5ad0ac874` provide concrete seed examples only.
They neither bound the search nor prove other packages minimal. Reconcile
intervening work without reverting unrelated changes. Scope is project-wide
implementation consolidation, not speculative CPU or product redesign.

## Known examples, not an exhaustive worklist

| Area | Source-grounded question and required disposition |
| --- | --- |
| Segment register/cache binding | The original c_reg.h defines ES/CS/SS/DS/FS/GS. modesw.c duplicates those numbers and private declarations in a local patch. The corrected numbers are necessary; eliminate duplicated authority using original declarations through the smallest appropriate boundary. Justify each cache refresh against the actual DPMI transition: a successful ES reproduction alone does not prove all six refreshes are minimal, and removing a necessary refresh is not acceptable diff reduction. |
| SetSTATUS provider | nt_cpu.c is already linked; its original setSTATUS body is inside A3CPU conditional compilation. First attempt selecting that existing body with the smallest build/conditional adaptation, without enabling the A3CPU backend, its initializer or IRET machinery. Bind the original provider and remove the binder copy when feasible. If not feasible, document the exact compile/link or ABI obstruction; do not repeat the inaccurate claim that the whole translation unit is unavailable. setFLAGS is not an equivalent substitute because of privilege filtering. |
| Explicit protected address | Preserve original SIM32's explicit pm contract independently of current CPU PE/VM. Continue using original descriptor decoding. Audit declarations and call shape for further simplification; never revive current-mode inference for an explicitly protected address. |
| XMS callback recovery and mapped movement | Preserve original xmsmemr.c commit/decommit selection; do not restore local clearing policy. Recheck the minimal mover boundary against physical mappings and overlapping ranges. Original forward SAS copying alone is not general memmove, and the original backward entry is not an implemented provider. Retain only demonstrated necessary mechanics. |
| Original BOOL bug | Independently confirm the DpmiReallocateVirtualMemory BOOL declaration/backends and caller failure reproduction. Preserve the minimal corrected BOOL test unless a directly reusable original correction supersedes it; do not remove a proven fix merely to lower the line count. |
| Selection and diagnostics | Ensure removed local bodies have no alternate production selection, original symbols resolve to their intended owners, and test-only tracing is absent from deployed execution. |

## S decomposition

| S | Complete scope | Exit requirement |
| --- | --- | --- |
| S1 | Freeze the project-wide implementation/mirror/build inventory; map every local semantic body to original OpenNT owners or a proved modern-product-only requirement. Audit functional duplicates, copied fragments, constants and unselected originals. | Coverage accounts for every owned production source and all mirror deltas; each candidate has a caller, original alternative, necessity evidence and concrete disposition. Group the complete implementation worklist by original owner, not historical task. |
| S2 | Consolidate CPU/SAS/C-VID, mode transitions, addresses and memory families. Resolve the known register/FLAGS examples and every additional duplicate in these owner families, retaining proven original bug fixes. | Original declarations/provider bodies own the behavior wherever composable; each remaining boundary is evidenced. No register/FLAGS-only partial closure substitutes for the full family ledger. |
| S3 | Consolidate process/command/BaseClient/BaseSrv, protocol/resource and worker-lifetime families, including shared component logic and mirror-embedded policy copies. | Original task/service policy has one owner; genuinely standalone transport, authentication and process-local resources retain only necessary mechanics. All identified duplicates are implemented away or have a proved retention boundary. |
| S4 | Consolidate all remaining families found by the complete inventory: DEM/files/OEM conversion, Console/input/video/host services, redirector/network, WOW/debugger/VDD and other selected packages. | No family is omitted because of directory placement or earlier closure. All candidates receive implemented dispositions; unsupported host services are not replaced with fake-success providers. |
| S5 | Independent whole-project re-audit, formal x86 deployment, complete regression and footprint report; search again for remaining copies after consolidation. | Every inventory item is reconciled, every retained local implementation has a proven reason, and all acceptance requirements below pass; reviewed changes are committed/pushed at delivery. |

S1 may split the implementation rows into additional sequential S packets by
complete original-owner package for manageable execution. Such splitting
must preserve the full inventory, every candidate and final acceptance; it
must not reduce this T to the known S36 examples or a fixed number of easy
repairs. No candidate may disappear through an unapproved deferral.

## Verification and exit criteria

Every implementation S must build the selected MSVC Win32/x86 profile and
pass the established COMMAND/MEM/EDIT tests, directly and through interactive
and nested COMMAND routes. Test guest text and expected exit behavior, not
process completion alone. Keep original guest media immutable.

Additional mandatory checks are:

- per-owner positive, negative and teardown tests for every replaced provider,
  with equivalent observable results before and after consolidation;
- complete inventory-to-disposition reconciliation, including unselected
  originals, retired local providers, mirror-internal patches and build paths;

- all six selected segment indices originate from authoritative definitions;
  mode-transition tests cover null/restored ES and the justified cache set;
- original host FLAGS restoration sets and clears every supported bit,
  including behavior distinguishing it from CPL/IOPL-filtered guest POPF;
- explicit protected-address lookup works while the CPU is in real mode;
- XMS/DPMI normal allocation, forced relocation, overlap/data preservation,
  failed growth, free/reuse and same-worker child-exit capacity recovery;
- worker termination on test-owned broker loss without inventing idle expiry;
- all 17 established product regression routes, formal symbol/map inspection,
  deployed artifact hashes, documentation governance and diff checks.

Report mirror changes relative both to pinned OpenNT and the task admission
baseline, separately for mvdm and opennt-host. Report removed/added overlay
and worker/adapter implementation lines separately from declarations, build
scripts, tests and documentation. List each remaining local semantic body
with its exact unavailable original alternative and reason for retention.
No numeric reduction target can justify losing a proven capability.

## Non-goals and stop conditions

No project-invented files in either mirror, guest-media edits, second CPU profile, blanket
A3CPU activation, speculative CCPU instruction changes, new generic helper
framework, formatting-only churn or claim that WRITE is now accepted.
Do not reintroduce the wrong segment numbering, null SetSTATUS slot, false
success on DPMI realloc failure or locally invented XMS zeroing. Escalate
actual scope/ABI expansion through normal task governance; lack of proof is
not permission to label a retained deviation necessary.
