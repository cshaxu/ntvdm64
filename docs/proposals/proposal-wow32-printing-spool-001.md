# WOW32 — Printing and spool

## Objective and admission

Owner-directed split dated 2026-09-28. This is one unnumbered candidate T in
[Queue](../states/QUEUE.md), not an active task or an implementation approval.
It owns workstream **W15**, formerly successor S15 / T422 S17, under the
[WOW32 program](proposal-wow32-production-completion-002.md). Its S identifiers
below are local to this candidate; historical S references resolve through the
program transfer table. Existing code, stable capability IDs, tests and failures
are inherited rather than reset. No new global source audit is required.

## Complete scope and dependencies

Inherited: print family: selected guest print/spool operations, conversions, job/DC lifecycle, failure/cancel/cleanup using W07/W08; document any unavailable external device with mock evidence.

Expected reuse boundaries: [Messages, callbacks and task execution](proposal-wow32-message-task-execution-001.md), [Modules, memory and resource aliases](proposal-wow32-modules-memory-aliases-001.md), [GDI identity, DC and drawing](proposal-wow32-gdi-dc-drawing-001.md), [Fonts, text and metafiles](proposal-wow32-fonts-text-metafiles-001.md).
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
| S1 | Print and spool contract | Map selected guest print/spool operations, conversions and endpoint availability. | Job/DC lifecycle and failure/cancel assertions cover each selected family. |
| S2 | Job and drawing services | Complete original print/spool paths using the existing GDI/font owners. | Real guest jobs and conversions have observable content and correct results. |
| S3 | Failure and retirement | Complete cancellation, endpoint errors and job/DC/task cleanup. | Failed, cancelled and repeated jobs leave no stale resources or callbacks. |
| S4 | Printing acceptance | Test available real endpoints and bounded mocks for unavailable devices. | Report real-host limitations explicitly and preserve all application/DOS regressions. |

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
