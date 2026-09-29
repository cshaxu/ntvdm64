# WOW32 — Messages, callbacks and task execution

## Objective and admission

Owner-directed split dated 2026-09-28. This is one unnumbered candidate T in
[Queue](../states/QUEUE.md), not an active task or an implementation approval.
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

## Proposed S tasks

| S | Scope | Work | Required closure |
| --- | --- | --- | --- |
| S1 | Inherited boundary and failing execution cases | Bind C01--C03/C09 and execution C10 to production callers; reproduce E92 and reuse E91/E93 with their original evidence limits. | A finite caller/test matrix distinguishes real guest, native peer and mock evidence. |
| S2 | Message delivery, replies and reentry | Complete native send/post/reply, early and duplicate reply, nested callbacks and original execution handoff through modern USER. | Real Win16/Win16 and both Win16/Win32 directions prove correct results and execution ordering. |
| S3 | Task execution and cancellation | Complete init/yield/hung registration, waits, cancellation, task loss and callback frame/lease retirement. | Normal, failed and interrupted execution leave no blocked sender, stale callback or live lease. |
| S4 | Package acceptance and handoff | Run the execution matrix, retained application frontiers and production regression; hand exact execution contracts to USER consumers. | All owned rows pass or retain an explicitly approved non-pass disposition; no local SMS transport or second scheduler remains. |

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
