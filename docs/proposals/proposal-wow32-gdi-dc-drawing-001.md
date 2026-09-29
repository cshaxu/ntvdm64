# WOW32 — GDI identity, DC and drawing

## Objective and admission

Owner-directed split dated 2026-09-28. This is one unnumbered candidate T in
[Queue](../states/QUEUE.md), not an active task or an implementation approval.
It owns workstream **W07**, formerly successor S7 / T422 S9, under the
[WOW32 program](proposal-wow32-production-completion-002.md). Its S identifiers
below are local to this candidate; historical S references resolve through the
program transfer table. Existing code, stable capability IDs, tests and failures
are inherited rather than reset. No new global source audit is required.

## Complete scope and dependencies

Inherited: bounded original-shape handle carrier, stock/select/delete/release, transforms, drawing, bitmap/DIB and palette. Verify original current-slot reuse semantics, positive/negative content and resource cleanup; no invented generation/type policy.

Expected reuse boundaries: [USER objects and shared view](proposal-wow32-user-objects-shared-view-001.md), [Modules, memory and resource aliases](proposal-wow32-modules-memory-aliases-001.md), [Resource discovery and loading foundation](proposal-wow32-resource-loading-001.md).
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
| S1 | GDI identity and selected drawing surface | Freeze original 14-bit current-slot identity, release/reuse rules and selected DC/drawing operations. | Assertions distinguish original reuse from invented generation/type policies. |
| S2 | DC and drawing services | Complete stock/select/delete/release, transforms and selected drawing primitives. | Real guest drawing has asserted content and correct error/resource results. |
| S3 | Bitmap, DIB and palette | Complete raster/palette paths and WINMINE monochrome/DC investigation. | Positive/negative image content, selection and resource cleanup are verified. |
| S4 | Graphics acceptance | Run GDI lifetime and visible-content tests plus application/DOS regression. | One native GDI carrier owns the mapping; no required fix is removed solely to reduce diff size. |

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
