# Proposal — independent project-wide implementation minimization review

## Objective and admission

Owner-requested candidate: **implementation minimization review**
(实现最小化复审). Audit ALL project-owned implementations for duplication of
behavior already provided by original OpenNT, including the existing mvdm-host
and non-mvdm mirrors, then consolidate those implementations back to their
original owners wherever composable. This is audit AND implementation, not
an inventory-only task and not a review limited to one historical delivery.
Preserve all demonstrated working capabilities. Original OpenNT remains authoritative: correct local bindings
to the original contract, never change the original contract merely to fit
local code. A semantic correction to original source requires a demonstrated
original bug, a minimal repair and focused positive/negative proof.

The owner has admitted this package through [Current](../states/CURRENT.md).
Current owns the numeric task and one active S; this proposal retains the full
project-wide scope and S decomposition. Admission is not an audit-completion
claim and does not pre-authorize unrelated architectural changes.

This candidate is an independent audit after
[implementation of the existing DOS16 and WOW32 inventories](proposal-mvdm-whole-closure-recovery-001.md)
and [mirror-diff review](proposal-unconnected-mirror-diff-cleanup-001.md).
All three serve the same goals: maximize justified original integration while
excluding ineligible original parts, and minimize mirror differences and
autonomous implementation without losing verified capabilities. This task
enters from all project-owned implementations and their actual owners.

Consume the shared inventory, stable finding/source-owner IDs, source/build
identities and tests, but independently inspect the actual implementation,
original alternative and earlier reasoning for every in-scope owner. Record
the prior conclusion, independent judgment, evidence and implemented or
retained disposition. Reuse unchanged evidence only after verifying input
identity and relevance; prior sign-off alone is not second-review evidence.
An independent reviewer may overturn earlier inclusion, exclusion, mirror or
local-provider decisions and implement corrections within this scope. Newly
discovered original-owner gaps are explicitly resolved through applicable
admission and architecture governance, not silently deferred. Earlier tasks
must complete cleanup necessary to their own repairs; this review is not a
license for temporary duplication.

## Project-wide scope and baseline

Freeze the current repository revision and formal build selections at task
admission. Inventory every project-owned production body, copied fragment,
declaration/constant authority and source-selection shim, regardless of age,
directory name or previous task closure. Include:

- all executable components, their worker/client/server bindings and shared
  ABI/package components;
- adapters, compatibility shims, generated providers and any retained overlay,
  including bodies moved out of historical overlay directories;
- autonomous logic hidden inside mirror diffs in mvdm-host and non-mvdm;
- selected and unselected implementations, separating live providers from
  dead duplicates, reference-only source and genuinely test-only doubles.

Compare both text and behavior with pinned original OpenNT and with both
existing mirrors. Independently challenge original parts excluded or included
by earlier reviews when their roles affect an implementation decision; original
existence alone does not make an owner eligible, and a missing runtime trace
alone does not justify exclusion. Match original owners, interfaces, state transitions,
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

The T448 preclosure selected-build mirror sweep also identified three
historical standalone declaration headers without an original file at their
historical mirror paths: `opennt-host/base/ntos/rtl/environapi.h`, `zwapi.h`,
and `opennt-host/netapi/xactsrv/xactsrv_wksta_minimal.h`. These declarations
now reside in `src/compat/include`; `zwapi.h` also exists under the original
`src/non-mvdm/public/internal/base/inc` path. The first two have
existing OPENNT-HOST-015/067 records; the third is present since `639c55e8f`.
Review their provenance, ownership and placement against the mirror rules as
part of this proposal. This observation alone does not establish a runtime
defect or authorize deletion of declarations needed by selected code.

| Area | Source-grounded question and required disposition |
| --- | --- |
| Segment register/cache binding | The original c_reg.h defines ES/CS/SS/DS/FS/GS. modesw.c duplicates those numbers and private declarations in a local patch. The corrected numbers are necessary; eliminate duplicated authority using original declarations through the smallest appropriate boundary. Justify each cache refresh against the actual DPMI transition: a successful ES reproduction alone does not prove all six refreshes are minimal, and removing a necessary refresh is not acceptable diff reduction. |
| SetSTATUS provider | nt_cpu.c is already linked; its original setSTATUS body is inside A3CPU conditional compilation. First attempt selecting that existing body with the smallest build/conditional adaptation, without enabling the A3CPU backend, its initializer or IRET machinery. Bind the original provider and remove the binder copy when feasible. If not feasible, document the exact compile/link or ABI obstruction; do not repeat the inaccurate claim that the whole translation unit is unavailable. setFLAGS is not an equivalent substitute because of privilege filtering. |
| Explicit protected address | Preserve original SIM32's explicit pm contract independently of current CPU PE/VM. Continue using original descriptor decoding. Audit declarations and call shape for further simplification; never revive current-mode inference for an explicitly protected address. |
| XMS callback recovery and mapped movement | Preserve original xmsmemr.c commit/decommit selection; do not restore local clearing policy. Recheck the minimal mover boundary against physical mappings and overlapping ranges. Original forward SAS copying alone is not general memmove, and the original backward entry is not an implemented provider. Retain only demonstrated necessary mechanics. |
| Original BOOL bug | Independently confirm the DpmiReallocateVirtualMemory BOOL declaration/backends and caller failure reproduction. Preserve the minimal corrected BOOL test unless a directly reusable original correction supersedes it; do not remove a proven fix merely to lower the line count. |
| Selection and diagnostics | Ensure removed local bodies have no alternate production selection, original symbols resolve to their intended owners, and test-only tracing is absent from deployed execution. |

## S decomposition

The original family-based sequence below is superseded for S2–S6 by the owner-approved
[category remediation plan](../etc/operations/m0-t483-mirror-register.md#owner-approved-s2s6-remediation-sequence).
S1 closed as the explicitly narrowed mirror-diff audit, not the project-wide
implementation audit originally described below. Retain the original scope and
acceptance obligations as later coverage; do not execute the old numbering in
parallel with the new sequence.

| S | Complete scope | Exit requirement |
| --- | --- | --- |
| S1 | Freeze the project-wide implementation/mirror/build inventory, existing audits and prior implementation and mirror-review results; independently map each local semantic body to original owners or a proved product-only requirement. | Every owned production source is covered; each finding records prior and independent decisions, caller, original alternative, necessity evidence and disposition. Group work by original owner. |
| S2 | Independently review and consolidate CPU/SAS/C-VID, mode transitions, addresses and memory families, including the known register/FLAGS cases. | Original declarations/provider bodies own behavior wherever justified and composable; retained fixes and every boundary are evidenced. |
| S3 | Independently review and consolidate process/command/BaseClient/BaseSrv, protocol/resource and worker-lifetime families, including shared logic and mirror-embedded policy copies. | One justified owner for original task/service policy; necessary standalone transport, authentication and process-local mechanics remain; each candidate is resolved or evidenced. |
| S4 | Independently review and consolidate all remaining families: DEM/files/OEM conversion, Console/input/video/host services, redirector/network, WOW/debugger/VDD and other selected packages. | No owner is omitted because of earlier closure; correct erroneous original admission/exclusion and local-provider decisions within applicable governance, and avoid fake-success providers. |
| S5 | Independently reconcile the full project inventory with existing audits and prior implementation and mirror-review decisions; re-search after consolidation, run formal deployment/regression and report footprint. | Every candidate and reversed decision has evidence, retained local bodies have a concrete rationale, and acceptance and delivery gates pass. |

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
baseline, separately for mvdm-host and non-mvdm. Report removed/added overlay
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
