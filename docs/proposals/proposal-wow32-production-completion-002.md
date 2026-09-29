# WOW32 production completion program after initial WINMINE milestone

## Objective and admission

Owner direction dated 2026-09-28 replaces the single 20-S candidate with
20 independently admitted, unnumbered candidate T packages. This document is
the shared requirements, evidence and transfer index; it is not an additional
Queue candidate. Each candidate has its own proposal and local S decomposition.
Only Status may allocate the next global T identifier and admit implementation.
The active packet is unchanged; this planning edit remains uncommitted in the
main working tree as requested.

[Root and application search isolation](proposal-ntvdm-system-root-path-isolation-001.md)
is now first in Queue, followed by the bounded
[CCPU40/V86 contract audit](proposal-ccpu40-v86-guest-contract-audit-001.md), then
the WOW32 packages below. Consume the audit's stable findings and owner mapping
without restarting inherited research. T422 closed only the initial WINMINE
milestone and research/planning preservation, not complete USER/WOW32 or WRITE/SOL.
No code, test, partial implementation, failure or acceptance obligation is lost.

Finish every selected original capability through its production path, ending
with immutable WRITE/WINMINE/SOL startup, use, normal/abnormal cleanup and repeat
runs. Necessary error-response and fatal-cleanup dependencies belong to the
earliest consuming package with one named owner, not automatically to W19 or
the later [general error-response package](proposal-error-dialog-termination-semantics-restoration-001.md).

## Inherited research, code and evidence

- [T422 bounded closure](../history/m0-t422-initial-wow32-closure.md).
- [Predecessor design and research index](proposal-wow32-complete-runtime-recovery-001.md):
  all source/ABI audits, 77 translation units, ten dispatch families,
  21 input/20 output slots, 26 mapped APIs and 47 direct-consumer checks.
- [Coverage ledger](../etc/operations/wow32-recovery-coverage-ledger.md):
  stable capability/source/slot IDs survive this transfer.
- [S1 audit closure](../etc/evidence/m0-t422-s1-wow32-audit-closure.md).
- [S2 lifecycle ledger](../etc/evidence/m0-t422-s2-user-client-lifecycle-ledger.md):
  E1--E95, C01--C12, exact artifacts, source conclusions and rejected hypotheses.
- [S2 handoff](../etc/evidence/m0-t422-s2-planning-handoff.md).
- [Former S3 checklist](../etc/evidence/m0-t422-s3-message-task-checklist.md):
  inherited by W01, not a completed implementation.
- [Run-evidence protocol](../etc/operations/m0-t422-s2-run-evidence-protocol.md):
  use successor task/run roots on admission; do not overwrite prior runs.

E53 records owner-played WINMINE. E91 has bounded native send tests; E92
early-reply execution handoff remains red; E93 is mock ordering evidence.
WRITE low-memory and SOL no-window are reported symptoms requiring exact
current-package attribution, not proof of a specific cause. Reconcile each
application's actual artifact/profile frontier; do not restart the global
audit or assume that earlier successful artifacts equal the current package.

## Complete proposed S sequence and transfer

The former successor S1--S20 sequence is now a historical transfer index:
T422 S3--S22 -> former successor S1--S20 -> W01--W20 below. W keys are stable
workstream references, not numeric T allocations or active S identifiers.
Every old row maps to exactly one candidate; each linked proposal owns its new
local S tasks. Historic S1/S2 research is inherited rather than repeated.
The retained scope and required closure below apply in full to each receiver.

| Workstream | Former successor S | Candidate T proposal | Required closure |
| --- | --- | --- | --- |
| W01 | S1 | [Messages, callbacks and task execution](proposal-wow32-message-task-execution-001.md) | C01--C03 plus C09 and execution-related C10: native send/post/reply, early/duplicate reply, waits, cancellation, reentry, original task init/yield/hung registration, task loss and callback frame/lease cleanup. Modern USER alone owns transport. Prove real guest and native-peer interactions, not only the E92/E93 fixtures. |
| W02 | S2 | [USER objects and shared view](proposal-wow32-user-objects-shared-view-001.md) | C04--C07 plus object-related C10: desktop/WND/CLS/handles, registration, publication/update/direct reads, stale reuse, geometry, mutation, nested destruction and rollback. Own-created, system/native and other-task object contracts are explicit. Objects do not close without their cleanup. |
| W03 | S3 | [Modules, memory and resource aliases](proposal-wow32-modules-memory-aliases-001.md) | Inherited: loader/memory slice: original module/loader-facing services, allocation/rollback and alias lifetimes, module release without live-task destruction. Consume W01 execution and W02 identities. |
| W04 | S4 | [Files, directories, environment and OEM](proposal-wow32-files-environment-oem-001.md) | Inherited: file/OEM slice: original operations, both OEM-WOW-DIR directions and OEM-WOW-DELETE branches, rename/delete rollback, retained-file/font fallback and real non-ASCII guest consumers. |
| W05 | S5 | [Resource discovery and loading foundation](proposal-wow32-resource-loading-001.md) | Original lookup/load/lock/unlock/free and module association; finite aliases owned by W03. Prove original guest names/IDs, bounds, missing/partial loads and lifetime before GDI/menu consumers. No resource-specific drawing or duplicate memory manager. |
| W06 | S6 | [Dialogs, input, hooks and timers](proposal-wow32-dialogs-input-hooks-timers-001.md) | C08 and inherited USER families: real dialog init/control/procedure/cancel/release; selected input, caret, keyboard, hook and timer operation families, callbacks and task-loss cleanup. Reuse W01/W02, never a second input router or scheduler. |
| W07 | S7 | [GDI identity, DC and drawing](proposal-wow32-gdi-dc-drawing-001.md) | Inherited: bounded original-shape handle carrier, stock/select/delete/release, transforms, drawing, bitmap/DIB and palette. Verify original current-slot reuse semantics, positive/negative content and resource cleanup; no invented generation/type policy. |
| W08 | S8 | [Fonts, text and metafiles](proposal-wow32-fonts-text-metafiles-001.md) | Inherited: font/text conversion and drawing, font lifetime, metafile creation/playback/release and failure paths; consume W07 identities. |
| W09 | S9 | [Resource conversion, menus and accelerators](proposal-wow32-menus-resource-conversion-001.md) | Inherited: conversion, icon/cursor/bitmap loading, menu graph/owner-draw mutation, accelerators, partial-load rollback and release. Consume W02 publication and W07 bitmap services, not duplicate providers. |
| W10 | S10 | [Clipboard](proposal-wow32-clipboard-001.md) | Inherited: clipboard slice: format conversion, delayed rendering, ownership transfer, rejected/abandoned data and task loss with real guest/native peers. |
| W11 | S11 | [DDE](proposal-wow32-dde-001.md) | Inherited: DDE slice: conversation, data/reply, reentry, cancellation, peer/module/task death and release. Original FreeDDEData alone is not acceptance. |
| W12 | S12 | [Shell and layered Registry](proposal-wow32-shell-registry-001.md) | Inherited: Shell family: all selected Shell thunks; shared NTVDM.REG read/create/set/enum/delete/close, original error conversion/recursive delete, tombstones, atomic persistence, cross-worker serialization and cleanup; never write system Registry. |
| W13 | S13 | [Winsock](proposal-wow32-winsock-001.md) | Inherited: network family: selected socket operation/conversion, async notification, failure/cancel/peer loss and task cleanup, real guest/local controlled peer tests. |
| W14 | S14 | [COMM](proposal-wow32-comm-001.md) | Inherited: serial family: selected open/config/read/write/status/events/close, cancellation and task cleanup. Hardware absence requires exact retained unavailable result, TODO and protocol-accurate mock, not a success stub. |
| W15 | S15 | [Printing and spool](proposal-wow32-printing-spool-001.md) | Inherited: print family: selected guest print/spool operations, conversions, job/DC lifecycle, failure/cancel/cleanup using W07/W08; document any unavailable external device with mock evidence. |
| W16 | S16 | [Sound and multimedia](proposal-wow32-sound-multimedia-001.md) | Inherited: multimedia family: selected operation families, callbacks, buffers, async completion/cancel and device/task cleanup. Track family-specific tests rather than link-only acceptance. |
| W17 | S17 | [ToolHelp and WOW debugger interfaces](proposal-wow32-toolhelp-debugger-001.md) | Inherited: ToolHelp/debugger slice: enumerate/query and selected callbacks/notifications, OEM-DBG-PATH WOW receiver, stale identity, task loss and release; preserve completed non-WOW debugger evidence. |
| W18 | S18 | [Common dialogs and related OLE interfaces](proposal-wow32-common-dialogs-ole-001.md) | Inherited: related UI/OLE slice: selected conversion, callback, cancel/error and ownership contracts. Use W06 dialogs and completed resource/service owners; enumerate both families separately and close every selected edge. |
| W19 | S19 | [WOW hard-error responses](proposal-wow32-hard-error-responses-001.md) | Inherited: ERROR-01: original response mapping, cancellation, fatal task/worker tail and cleanup. Reconcile the queued general error-dialog proposal without duplicating its provider or silently excluding WOW callers. |
| W20 | S20 | [Whole-provider and three-application acceptance](proposal-wow32-integrated-acceptance-001.md) | Inherited: final same-artifact WRITE/WINMINE/SOL scenarios, repeated/concurrent tasks, abnormal cleanup, integrated OEM and subsequent DOS usability. Reconcile C11/C12 and all registration/dispatch/direct-data edges; no implementation backlog is deferred here. |

C01--C03/C09 and execution C10 belong to W01; C04--C07 and object C10 to
W02; C08 to W06. C11 edge/diff review and C12 build/test/delivery apply to
every candidate. W20 repeats aggregate verification, never receives unfinished
implementation. Shared files have function-level ownership; each original
algorithm has one implementation and multiple consumers. Earlier-stage error
work is reused by W19 and the later general error package through stable IDs.
Old ledgers using successor S numbers must resolve through this table before
interpreting a new candidate's local S number.

## Dependency and unique-owner contract

- W01 messages/callback execution and W02 objects use proven existing T422 S2-era
  foundations initially. Verify those exact prerequisites before acceptance.
  If missing, promote the named original capability; do not build a temporary
  provider or silently absorb the whole downstream package.
- W03 owns module/memory and shared alias allocation/lifetime; W04 owns file/
  directory/environment/OEM; W05 owns resource find/load/lock/unlock/free policy.
- W06 owns dialog/input/hook/timer semantics and cleanup, consuming W01 callback
  mechanics, W02 objects and W05 template loading.
- W07 owns the native GDI handle carrier, DC/drawing/bitmap/DIB/palette boundary.
  W08 consumes it for fonts/text/metafiles; W09 consumes it and W05 loading for
  menu/icon/cursor/accelerator behavior. No duplicate bitmap converter/loader.
- W10 owns source-proven common clipboard format/data-transfer primitives;
  W11 owns DDE conversation/reply and consumes shared primitives where valid.
- W12--W19 own their respective service failure and cleanup behavior. W19's
  hard-error family cannot delay error handling required by earlier owners.
- Network/COMM/printing/multimedia remain selected capability obligations,
  not assumed prerequisites for the three apps without a real call edge.
  Three-app frontier checks run throughout, not only at W20.

## Architecture and immutable inputs

Modern Windows USER is the sole delivery/reply transport for Win16/Win16 and
both Win16/Win32 directions. Retain original MVDM thunks and composable
opennt-host algorithms; bounded wow32-dll bindings join native calls to
original WOW task scheduling. No local SMS delivery queue, guessed native
private identity, second scheduler/CCPU owner or recursive USER/CSRSS import.

Preserve original PMODE32 layout/rebasing and live producer/update/withdrawal
contracts as well as ordinary thunk and callback paths. W02 must prove direct
reads during guest execution without thunk-only refresh, first callback
visibility, external/native objects, stale reuse and lifetime. Native pointers
are not guest objects merely because they fit 32 bits.

W07 retains the original 14-bit current-slot GDI identity rule: zero gives NULL,
successful native release retires, later allocation can reuse; do not invent
generation/type/task-destruction rules. Native GDI validates its own objects.
Guest original defects are registered with tests/TODO, not patched or passed.

Installed paths derive from the package location, never hard-coded O:/winnt.
Preserve original WIN16DIR/child environment and network.drv failure behavior;
temporary approved SYSTEM.INI controls must be restored. W12 reuses the
existing layered Registry owner: admitted host keys read-only, NTVDM.REG
override/persistent writes with tombstones, atomic commit, serialization and
original Shell error/recursive-delete semantics; never write host Registry.

W07 owns WINMINE monochrome/DC/bitmap investigation; W09 owns color-versus-
monochrome resource selection, using one pipeline. W02 owns window/client
geometry. Existing owner-accepted mojibake is not a waiver of W04 OEM behavior.

## Mandatory executable tests for each implementation S

Every implementation S delivers checked-in test source/scripts under tests/,
not only research notes, disposable build fixtures or a list of commands.
Reuse and extend existing tests when valid. Generated binaries remain under
build/ and runtime tests/logs under O:/winnt/tests and O:/winnt/logs.

Every capability checklist row links its exact repository test entrypoint,
case/arguments, prerequisite/media identity, expected assertion, last observed
result and source/artifact/run identities. Missing tests or unnamed assertions
keep the row open. Tests must fail/report non-pass for a missing provider,
wrong output, timeout, cancellation/cleanup failure or replaced test double.

Acceptance exercises the selected built/deployed production caller and
provider, including actual guest-to-host and callback/direct-data edges where
applicable. A test-only substitute, manually called unused wrapper, imported
symbol, trace hit, compile or process exit alone proves no working capability.
Verify normal behavior, representative errors, reentry/asynchrony when relevant
and task/worker release. Native fixtures/mocks supplement, never replace real
guest acceptance. Unavailable external prerequisites and approved original
guest limitations remain explicitly non-pass with TODO and mock coverage.

Each S maintains one indexed ledger with separate source/build/wiring/focused/
guest/teardown/delivery states. Commit tests with the implementation they
verify. Reopen only on changed inputs or counterevidence; no repeated global
research and no unbound completed edge. Research/planning-only deliveries are
labelled accordingly, not functional closures.

## Every production-code P: regression and owner side testing

Follow [Execution Rules](../rules/EXECUTION.md#every-p-regression-and-side-test-publication-gate).
Each production-code P passes affected tests and all 17 text-gated DOS routes
(direct, interactive/nested COMMAND/MEM/EDIT and EDIT then MEM), plus independent
WINMINE/SOL/WRITE frontier comparisons against the previous P and each historic
best verified behavior. One app's progress cannot offset another's regression.

Record per app: source/diff and all selected runtime binary hashes, guest/config hashes,
command/profile, initialization, visible content, actual interaction,
normal/abnormal exit, exact error/failing boundary or wait, baseline run IDs,
expected/actual result and advanced/unchanged/regressed/unverified verdict.
An inherited failure stays open; new regressions or missing comparisons block
production delivery. Never lower the baseline or equate a window with playability.

Use valid incremental build caches with exact toolchain/flags/dependency
identity; rebuild affected closure, not blindly all sources. Preserve sealed
evidence independently. Publish the complete package selected at admission: currently ntmon.exe,
run16.exe, ntsrv.exe, ntvdm.exe, ntkvm.exe, WOW32.DLL and VDMREDIR.DLL, plus
original needed media and reviewed config to O:/winnt. Verify all final hashes and keep a recoverable coherent
baseline; protect owner sessions/data and do not leave mixed deployments.

Pure documentation P commits require governance/link/diff checks only, not
runtime tests or redeployment. No GUI interference while the owner uses the
desktop; coordinate visible acceptance rather than fabricate passes.

## Final acceptance and footprint report

W20 runs the following on one identified ordinary package, without diagnostic
bypasses, preserving the immutable guest:
- WRITE: interactive document, edit/select, menu/dialog, save and reopen,
  repaint/resize, save failure/cancel, close/restart.
- WINMINE: real board, reveal/flag/unflag, timer/counter, repaint, menu/
  difficulty/new game, close/restart without stuck input or callbacks.
- SOL: deal, legal card movement, draw/new game/options, repaint/resize,
  close/restart without stale cards/resources/callbacks.
Repeat tasks, supported concurrent interaction and abnormal cleanup, then
DOS regression. Other selected families require their own tests even if these
apps never call them. Include integrated OEM and dispatch/registration/
direct-data coverage with no unexplained placeholder or fixture-only pass.

Report mvdm and opennt-host original-mirror additions/deletions separately,
restore format-only drift and quantify retained/removed autonomous code.
Importing original source is not the same metric as deleting a duplicate.
Owner final acceptance is required to close the integrated-acceptance candidate
and the complete WOW32 program. Individual package closure does not certify
untested downstream capabilities.
