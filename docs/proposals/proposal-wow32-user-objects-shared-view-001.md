# WOW32 — USER objects and shared view

## Objective and admission

Owner-directed split dated 2026-09-28, subsequently admitted as T451. The
active packet and delivery state are authoritative in [Current](../states/CURRENT.md);
this proposal does not independently admit additional work.
It owns workstream **W02**, formerly successor S2 / T422 S4, under the
[WOW32 program](proposal-wow32-production-completion-002.md). Its S identifiers
below are local to T451; historical S references resolve through the
program transfer table. Existing code, stable capability IDs, tests and failures
are inherited rather than reset. No new global source audit is required.

## Complete scope and dependencies

Approved T447 transfer (2026-10-14): T447 S2 delivers the dependency-complete
WND/CLS/desktop/handle foundation used by execution, including direct readers,
first callbacks, updates, reuse, rollback and destruction. This candidate
retains only independent residual system/native/external-object and geometry
cases after reconciliation. C04--C07/object-C10 remain broad, open rows until
all their cases are assigned and verified; narrow projections cannot close
them. The local S table below applies to those residual cases, not duplicate
implementation of T447's foundation. See the approved
[transfer audit](../etc/operations/t447-wow-execution-plan.md#owner-requested-cross-t-planning-audit).

C04--C07 plus object-related C10: desktop/WND/CLS/handles, registration, publication/update/direct reads, stale reuse, geometry, mutation, nested destruction and rollback. Own-created, system/native and other-task object contracts are explicit. Objects do not close without their cleanup.

Expected reuse boundaries: [Messages, callbacks and task execution](proposal-wow32-message-task-execution-001.md).
Queue order is the planned execution order; it does not assert an unproved
runtime dependency. At admission, prove which listed reuse edges are actual
prerequisites; do not require an unrelated service merely because it is listed. A missing
original-owner dependency receives an explicit bounded promotion; do not build
a temporary provider or silently absorb another candidate's implementation.
Necessary error and fatal-cleanup paths belong to the earliest consuming owner,
with stable-ID handoff, even when general hard-error recovery is later.

## Admitted S tasks

| S | Scope | Work | Required closure |
| --- | --- | --- | --- |
| S1 | Object and producer contract | Map C04--C07/object C10 and original PMODE32 data consumers, including native/system/other-task objects. | Every direct read has a producer, update rule, lifetime and executable assertion. |
| S2 | Identity, classes and publication | Complete desktop/WND/CLS identity, registration, first-callback publication and original layout/rebasing. | Guest direct reads and first callbacks observe valid objects, including external objects. |
| S3 | Mutation and destruction | Complete geometry, host-originated updates without thunk refresh, stale reuse, nested destruction and rollback. | Reentry, mutation and failed creation withdraw or update the correct object without corrupting another task. |
| S4 | Object acceptance and handoff | Run direct-data, callback and object-lifetime tests plus production regression. | All selected object classes and direct-read paths are accounted for; consumers reuse this single view owner. |
| S5 | Direct-root native CUI route and handoff | Authenticate nested execution contexts against the NTSRV-owned root; order completion and Console restoration before root retirement. Retain the sequence-specific final-frame contract and clean shared creation ownership. | Ten root/inherited EXIT cases, Control/DOS/WOW regressions, failure paths and coherent publication; [actual source findings and evidence](../etc/evidence/m0-t451-s5-direct-root-native-cui-route.md), not a short-lived-command hypothesis. |

## Verification, boundaries and handoff

The program's [architecture and immutable-input rules](proposal-wow32-production-completion-002.md#architecture-and-immutable-inputs),
[executable-test requirements](proposal-wow32-production-completion-002.md#mandatory-executable-tests-for-each-implementation-s),
[production regression and publication gates](proposal-wow32-production-completion-002.md#every-production-code-p-regression-and-owner-side-testing)
and [inherited evidence](proposal-wow32-production-completion-002.md#inherited-research-code-and-evidence)
apply in full to this candidate. Build the selected MSVC Win32/x86 /MT CCPU40
production path. Preserve immutable guest media, one original owner per
mechanism, modern USER transport and the prohibition on a second scheduler.

Check in tests for every owned row: exact callers, inputs, assertions, normal
behavior, failures, reentry/asynchrony where relevant and resource/task cleanup.
Real guest/native interactions and content are required where available;
compile, symbol, process-exit or mock-only results are insufficient. Explicit
unavailable external or approved original-guest limitations stay non-pass.
Each production-code delivery retains DOS17, independent WRITE/WINMINE/SOL
frontiers and all applicable accepted package regressions; use the current
complete package and verify its hashes. No progress in one app offsets another's
regression. C11/C12 apply locally, not only at final program acceptance.

Close only this candidate's complete owner scope, with exact artifacts, test
entrypoints, remaining approved limitations and stable handoff IDs for consumers.
Do not defer unfinished implementation, errors or cleanup to integrated acceptance.
This planning split itself modifies no runtime, admits no active packet and
requires no commit or publication; the owner requested working-tree edits only.
