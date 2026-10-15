# WOW32 — Messages, callbacks and task execution

## Objective and admission

Owner-directed split dated 2026-09-28. Following accepted T446 closure, this
package is admitted as T447 in [Current](../states/CURRENT.md) and removed
from Queue. Current owns the active brief; the
[working plan](../etc/operations/t447-wow-execution-plan.md) owns its sequence.
It owns workstream **W01**, formerly successor S1 / T422 S3, under the
[WOW32 program](proposal-wow32-production-completion-002.md). Its S identifiers
below are local to this candidate; historical S references resolve through the
program transfer table. Existing code, stable capability IDs, tests and failures
are inherited rather than reset. No new global source audit is required.

## Complete scope and dependencies

C01--C03 plus C09 and execution-related C10: native send/post/reply, early/duplicate reply, waits, cancellation, reentry, original task init/yield/hung registration, task loss and callback frame/lease cleanup. Modern USER alone owns transport. Prove real guest and native-peer interactions, not only the E92/E93 fixtures.

Expected reuse boundaries: [CCPU40/V86 contract audit](proposal-ccpu40-v86-guest-contract-audit-001.md) and the inherited, verified T422 foundations.
Queue order is the planned execution order; it does not assert an unproved
runtime dependency. At admission, prove which listed reuse edges are actual
prerequisites; do not require an unrelated service merely because it is listed. A missing
original-owner dependency receives an explicit bounded promotion; do not build
a temporary provider or silently absorb another candidate's implementation.
Necessary error and fatal-cleanup paths belong to the earliest consuming owner,
with stable-ID handoff, even when general hard-error recovery is later.

## Adopted S tasks (2026-10-14)

| S | Scope | Work | Required closure |
| --- | --- | --- | --- |
| S1 | Inherited boundary and failing execution cases | Bind C01--C03/C09 and execution C10 to production callers; reproduce E92 and reuse E91/E93 with their original evidence limits. | A finite caller/test matrix distinguishes real guest, native peer and mock evidence. |
| S2 | Callback and object foundations | Original callback entry/resolver plus required W02 identity, publication, update, rollback and normal retirement. | Production-connected first callbacks and direct readers; paired startup/teardown and real guest evidence. |
| S3 | Basic controls/dialogs/menu dependencies | Required W03 EDIT local-memory/dialog DS, W06 internal children/dialog lifecycle and W09 MENU-01 parser/graph/mutations. | Normal SOL/WRITE initialization with retained WINMINE; promote earlier message prerequisites when needed. |
| S4 | Full message identity/reply/reentry | Complete original handoff, explicit/implicit mixed sends, early/duplicate replies and waits. | Real Win16/Win16 and both native directions prove results and execution ordering; no semantic waiver. |
| S5 | Task lifetime and individual termination | Original hung/exit/cancellation and approved NT Monitor request-to-HTASK deletion, with reuse protection. | Delete either real task while its sibling stays usable; explicit worker termination remains separate. |
| S6 | Package acceptance and handoff | Reconcile all owned rows, source/diff minimization and residual candidate scopes; retain same-package regression/publication gates. | No unfinished implementation transferred to acceptance; exact evidence and remaining limitations are handed off. |

The approved [cross-T audit](../etc/operations/t447-wow-execution-plan.md#owner-requested-cross-t-planning-audit)
assigns only dependency-complete portions of W02/W03/W06/W09 here, not their
whole families. Stable W/C/DATA IDs survive. Basic cleanup belongs to each
earliest consumer, not only S5. Each production P retains non-regression and
publication gates; S3 additionally requires normal application initialization.
The owner-directed source checkpoint is not a delivered P or S closure.

At delivered P15 the owner requests a finite S2 freeze. The
[scope audit](../etc/evidence/m0-t447-s2-scope-freeze.md) fixes F01--F12 and
four remaining blockers A1--A4: callback restoration, remaining callback
execution contexts, terminal refusal and native-return completion safety.
S2 remains active; these public-contract gaps cannot be deferred to S3/S4/S5.
Concrete controls/EDIT/dialog/menu functionality stays S3; complete message
identity/reply/cross-task behavior stays S4; targeted termination stays S5.
The complete C/W scope is retained, with no new semantic waiver or guest change.

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
The approved planning adoption and development checkpoint do not imply runtime
acceptance or publication; Current remains the sole active packet authority.
