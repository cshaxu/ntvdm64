# WOW32 production completion after initial WINMINE milestone

## Objective and admission

Owner-requested successor candidate dated 2026-09-24. It is queue head,
unadmitted and has no numeric T identifier. T422 closes only as the initial
WINMINE milestone plus research/planning preservation; it does not certify
complete USER/WOW32, WRITE or SOL. The owner must admit this candidate before
implementation. All outstanding former T422 S3--S22 work transfers below.
No test, partial implementation or known failure is discarded or reset to zero.

Finish the selected original WOW32 capabilities through their production
paths and end with immutable WRITE.EXE, WINMINE.EXE and SOL.EXE startup, use,
normal/abnormal cleanup and repeat runs. Keep minimal OpenNT diffs and one
owner per mechanism; original guest binaries remain immutable.

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
  inherited by successor S1, not a completed implementation.
- [Run-evidence protocol](../etc/operations/m0-t422-s2-run-evidence-protocol.md):
  use successor task/run roots on admission; do not overwrite prior runs.

E53 records owner-played WINMINE. E91 has bounded native send tests; E92
early-reply execution handoff remains red; E93 is mock ordering evidence.
WRITE low-memory and SOL no-window are reported symptoms requiring exact
current-package attribution, not proof of a specific cause. Reconcile each
application's actual artifact/profile frontier; do not restart the global
audit or assume that earlier successful artifacts equal the current package.

## Complete proposed S sequence and transfer

The mapping is exact: former T422 S3 becomes successor S1, through former
S22 becoming successor S20. Former S1/S2 research is inherited, not repeated.
Each row includes its positive/negative/reentry/cancellation/cleanup contract.

| S | Complete scope | Required closure |
| --- | --- | --- |
| S1 | Messages, callbacks and task execution | C01--C03 plus C09 and execution-related C10: native send/post/reply, early/duplicate reply, waits, cancellation, reentry, original task init/yield/hung registration, task loss and callback frame/lease cleanup. Modern USER alone owns transport. Prove real guest and native-peer interactions, not only the E92/E93 fixtures. |
| S2 | USER objects and shared view | C04--C07 plus object-related C10: desktop/WND/CLS/handles, registration, publication/update/direct reads, stale reuse, geometry, mutation, nested destruction and rollback. Own-created, system/native and other-task object contracts are explicit. Objects do not close without their cleanup. |
| S3 | Modules, memory and resource aliases | Inherited: loader/memory slice: original module/loader-facing services, allocation/rollback and alias lifetimes, module release without live-task destruction. Consume S1 execution and S2 identities. |
| S4 | Files, directories, environment and OEM | Inherited: file/OEM slice: original operations, both OEM-WOW-DIR directions and OEM-WOW-DELETE branches, rename/delete rollback, retained-file/font fallback and real non-ASCII guest consumers. |
| S5 | Resource discovery and loading foundation | Original lookup/load/lock/unlock/free and module association; finite aliases owned by S3. Prove original guest names/IDs, bounds, missing/partial loads and lifetime before GDI/menu consumers. No resource-specific drawing or duplicate memory manager. |
| S6 | Dialogs, input, hooks and timers | C08 and inherited USER families: real dialog init/control/procedure/cancel/release; selected input, caret, keyboard, hook and timer operation families, callbacks and task-loss cleanup. Reuse S1/S2, never a second input router or scheduler. |
| S7 | GDI identity, DC and drawing | Inherited: bounded original-shape handle carrier, stock/select/delete/release, transforms, drawing, bitmap/DIB and palette. Verify original current-slot reuse semantics, positive/negative content and resource cleanup; no invented generation/type policy. |
| S8 | Fonts, text and metafiles | Inherited: font/text conversion and drawing, font lifetime, metafile creation/playback/release and failure paths; consume S7 identities. |
| S9 | Resource conversion, menus and accelerators | Inherited: conversion, icon/cursor/bitmap loading, menu graph/owner-draw mutation, accelerators, partial-load rollback and release. Consume S2 publication and S7 bitmap services, not duplicate providers. |
| S10 | Clipboard | Inherited: clipboard slice: format conversion, delayed rendering, ownership transfer, rejected/abandoned data and task loss with real guest/native peers. |
| S11 | DDE | Inherited: DDE slice: conversation, data/reply, reentry, cancellation, peer/module/task death and release. Original FreeDDEData alone is not acceptance. |
| S12 | Shell and layered Registry | Inherited: Shell family: all selected Shell thunks; shared NTVDM.REG read/create/set/enum/delete/close, original error conversion/recursive delete, tombstones, atomic persistence, cross-worker serialization and cleanup; never write system Registry. |
| S13 | Winsock | Inherited: network family: selected socket operation/conversion, async notification, failure/cancel/peer loss and task cleanup, real guest/local controlled peer tests. |
| S14 | COMM | Inherited: serial family: selected open/config/read/write/status/events/close, cancellation and task cleanup. Hardware absence requires exact retained unavailable result, TODO and protocol-accurate mock, not a success stub. |
| S15 | Printing and spool | Inherited: print family: selected guest print/spool operations, conversions, job/DC lifecycle, failure/cancel/cleanup using S7/S8; document any unavailable external device with mock evidence. |
| S16 | Sound and multimedia | Inherited: multimedia family: selected operation families, callbacks, buffers, async completion/cancel and device/task cleanup. Track family-specific tests rather than link-only acceptance. |
| S17 | ToolHelp and WOW debugger interfaces | Inherited: ToolHelp/debugger slice: enumerate/query and selected callbacks/notifications, OEM-DBG-PATH WOW receiver, stale identity, task loss and release; preserve completed non-WOW debugger evidence. |
| S18 | Common dialogs and related OLE interfaces | Inherited: related UI/OLE slice: selected conversion, callback, cancel/error and ownership contracts. Use S6 dialogs and completed resource/service owners; enumerate both families separately and close every selected edge. |
| S19 | WOW hard-error responses | Inherited: ERROR-01: original response mapping, cancellation, fatal task/worker tail and cleanup. Reconcile the queued general error-dialog proposal without duplicating its provider or silently excluding WOW callers. |
| S20 | Whole-provider and three-application acceptance | Inherited: final same-artifact WRITE/WINMINE/SOL scenarios, repeated/concurrent tasks, abnormal cleanup, integrated OEM and subsequent DOS usability. Reconcile C11/C12 and all registration/dispatch/direct-data edges; no implementation backlog is deferred here. |

C01--C03/C09 and execution C10 transfer to S1; C04--C07 and object C10 to
S2; C08 to S6. Every owner applies C11 edge/diff review and C12 build/test/
delivery to its own slice. S20 repeats aggregate verification, not unfinished
implementation. Source files shared by several rows use function-level
ownership; each imported algorithm has one implementation and multiple users.

## Dependency and unique-owner contract

- S1 messages/callback execution and S2 objects use proven existing S2-era
  foundations initially. Verify those exact prerequisites before acceptance.
  If missing, promote the named original capability; do not build a temporary
  provider or silently absorb the whole downstream package.
- S3 owns module/memory and shared alias allocation/lifetime; S4 owns file/
  directory/environment/OEM; S5 owns resource find/load/lock/unlock/free policy.
- S6 owns dialog/input/hook/timer semantics and cleanup, consuming S1 callback
  mechanics, S2 objects and S5 template loading.
- S7 owns the native GDI handle carrier, DC/drawing/bitmap/DIB/palette boundary.
  S8 consumes it for fonts/text/metafiles; S9 consumes it and S5 loading for
  menu/icon/cursor/accelerator behavior. No duplicate bitmap converter/loader.
- S10 owns source-proven common clipboard format/data-transfer primitives;
  S11 owns DDE conversation/reply and consumes shared primitives where valid.
- S12--S19 own their respective service failure and cleanup behavior. S19's
  hard-error family cannot delay error handling required by earlier owners.
- Network/COMM/printing/multimedia remain selected capability obligations,
  not assumed prerequisites for the three apps without a real call edge.
  Three-app frontier checks run throughout, not only at S20.

## Architecture and immutable inputs

Modern Windows USER is the sole delivery/reply transport for Win16/Win16 and
both Win16/Win32 directions. Retain original MVDM thunks and composable
opennt-host algorithms; bounded wow32-dll bindings join native calls to
original WOW task scheduling. No local SMS delivery queue, guessed native
private identity, second scheduler/CCPU owner or recursive USER/CSRSS import.

Preserve original PMODE32 layout/rebasing and live producer/update/withdrawal
contracts as well as ordinary thunk and callback paths. S2 must prove direct
reads during guest execution without thunk-only refresh, first callback
visibility, external/native objects, stale reuse and lifetime. Native pointers
are not guest objects merely because they fit 32 bits.

S7 retains the original 14-bit current-slot GDI identity rule: zero gives NULL,
successful native release retires, later allocation can reuse; do not invent
generation/type/task-destruction rules. Native GDI validates its own objects.
Guest original defects are registered with tests/TODO, not patched or passed.

Installed paths derive from the package location, never hard-coded O:/winnt.
Preserve original WIN16DIR/child environment and network.drv failure behavior;
temporary approved SYSTEM.INI controls must be restored. S12 reuses the
existing layered Registry owner: admitted host keys read-only, NTVDM.REG
override/persistent writes with tombstones, atomic commit, serialization and
original Shell error/recursive-delete semantics; never write host Registry.

S7 owns WINMINE monochrome/DC/bitmap investigation; S9 owns color-versus-
monochrome resource selection, using one pipeline. S2 owns window/client
geometry. Existing owner-accepted mojibake is not a waiver of S4 OEM behavior.

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

Record per app: source/diff and all six binary hashes, guest/config hashes,
command/profile, initialization, visible content, actual interaction,
normal/abnormal exit, exact error/failing boundary or wait, baseline run IDs,
expected/actual result and advanced/unchanged/regressed/unverified verdict.
An inherited failure stays open; new regressions or missing comparisons block
production delivery. Never lower the baseline or equate a window with playability.

Use valid incremental build caches with exact toolchain/flags/dependency
identity; rebuild affected closure, not blindly all sources. Preserve sealed
evidence independently. Publish the tested dtmgr.exe, run16.exe, basesrv.exe,
ntvdm.exe, WOW32.DLL and VDMREDIR.DLL plus original needed media and reviewed
config to O:/winnt. Verify all final hashes and keep a recoverable coherent
baseline; protect owner sessions/data and do not leave mixed deployments.

Pure documentation P commits require governance/link/diff checks only, not
runtime tests or redeployment. No GUI interference while the owner uses the
desktop; coordinate visible acceptance rather than fabricate passes.

## Final acceptance and footprint report

S20 runs the following on one identified ordinary package, without diagnostic
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
Owner final acceptance is required to close the successor T.
