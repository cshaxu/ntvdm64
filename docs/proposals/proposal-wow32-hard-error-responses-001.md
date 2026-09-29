# WOW32 — WOW hard-error responses

## Objective and admission

Owner-directed split dated 2026-09-28. This is one unnumbered candidate T in
[Queue](../states/QUEUE.md), not an active task or an implementation approval.
It owns workstream **W19**, formerly successor S19 / T422 S21, under the
[WOW32 program](proposal-wow32-production-completion-002.md). Its S identifiers
below are local to this candidate; historical S references resolve through the
program transfer table. Existing code, stable capability IDs, tests and failures
are inherited rather than reset. No new global source audit is required.

## Complete scope and dependencies

Inherited: ERROR-01: original response mapping, cancellation, fatal task/worker tail and cleanup. Reconcile the queued general error-dialog proposal without duplicating its provider or silently excluding WOW callers.

Expected reuse boundaries: [Messages, callbacks and task execution](proposal-wow32-message-task-execution-001.md), [USER objects and shared view](proposal-wow32-user-objects-shared-view-001.md).
Queue order is the planned execution order; it does not assert an unproved
runtime dependency. At admission, prove which listed reuse edges are actual
prerequisites; do not require an unrelated service merely because it is listed. A missing
original-owner dependency receives an explicit bounded promotion; do not build
a temporary provider or silently absorb another candidate's implementation.
Necessary error and fatal-cleanup paths belong to the earliest consuming owner,
with stable-ID handoff, even when general hard-error recovery is later.

## Proposed S tasks

| S | Scope | Work | Required closure |
| --- | --- | --- | --- |
| S1 | Residual error ownership | Reconcile ERROR-01 and error dependencies already completed by earlier WOW packages. | Each residual caller/response has one owner; no completed provider is rebuilt. |
| S2 | Response mapping and cancellation | Complete original hard-error response encoding/button mapping and selected cancellation paths. | Actual guest/native presentation returns the original response to its caller. |
| S3 | Fatal task and worker cleanup | Verify W32HungAppNotifyThread and original fatal task/worker tails without private CSRSS/USER recreation. | Target task completion and resource release are observed, or the exact unavailable boundary remains non-pass. |
| S4 | Error acceptance and handoff | Run WOW response/lifetime regression and pass remaining non-WOW callers to general error recovery. | No earlier package deferred mandatory safe error handling to this package. |

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
