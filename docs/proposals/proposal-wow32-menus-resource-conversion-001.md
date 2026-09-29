# WOW32 — Resource conversion, menus and accelerators

## Objective and admission

Owner-directed split dated 2026-09-28. This is one unnumbered candidate T in
[Queue](../states/QUEUE.md), not an active task or an implementation approval.
It owns workstream **W09**, formerly successor S9 / T422 S11, under the
[WOW32 program](proposal-wow32-production-completion-002.md). Its S identifiers
below are local to this candidate; historical S references resolve through the
program transfer table. Existing code, stable capability IDs, tests and failures
are inherited rather than reset. No new global source audit is required.

## Complete scope and dependencies

Inherited: conversion, icon/cursor/bitmap loading, menu graph/owner-draw mutation, accelerators, partial-load rollback and release. Consume W02 publication and W07 bitmap services, not duplicate providers.

Expected reuse boundaries: [USER objects and shared view](proposal-wow32-user-objects-shared-view-001.md), [Resource discovery and loading foundation](proposal-wow32-resource-loading-001.md), [Dialogs, input, hooks and timers](proposal-wow32-dialogs-input-hooks-timers-001.md), [GDI identity, DC and drawing](proposal-wow32-gdi-dc-drawing-001.md), [Fonts, text and metafiles](proposal-wow32-fonts-text-metafiles-001.md).
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
| S1 | Resource-consumer boundary | Map icon/cursor/bitmap formats, menu graph ownership and accelerator consumers. | Loading, storage and native bitmap responsibilities stay with their existing owners. |
| S2 | Resource conversion | Complete icon/cursor/bitmap conversion and color-versus-monochrome selection. | Real resource content, bounds and partial-load rollback are tested through one pipeline. |
| S3 | Menus and accelerators | Complete menu mutation, owner-draw callbacks and accelerator dispatch. | Guest interaction, reentry, cancellation and graph/resource release are correct. |
| S4 | Consumer acceptance | Run application menu/resource scenarios and retained regression. | All conversion/menu/accelerator edges close, including failure and destruction. |

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
