# WOW32 production closure checklist

This retains the detailed obligations discovered during S40 research.

Current owner: [WOW32 successor](../../proposals/proposal-wow32-complete-runtime-recovery-001.md).
The owner transferred all unfinished gates on 2026-09-21. Historical S40--S48
references below describe earlier plans, not current task assignments. No
unchecked item is accepted by the transfer.

| Retained obligation | Current receiver |
| --- | --- |
| W1.0/W1.1 shared data, client objects and thread publication | Successor S1, using delivered S41 and partial S42 inputs |
| W1.2--W1.5 and all W2 | Successor S2; no fixture-only task/exit closure |
| W3 and W4 | Successor S3 |
| W5/W6 and full registrar | Successor S4 |
| W7, OEM-WOW and immutable WOW16/WRITE | Successor S5 |

Every already implemented slot must be wired immediately; typed explicit
failure placeholders are allowed only for identified unfinished owners.
Aggregate acceptance requires replacing all selected placeholders. This
supersedes the older W7-only-publication wording retained below.
The owner superseded its single-S execution plan with five consecutive WOW32
packets in the [proposal](../../proposals/proposal-mvdm-runtime-package-completion-001.md).
Only [Current](../../states/CURRENT.md) admits the active packet.
The [source and test evidence](../evidence/m0-t420-s40-wow-user-profile.md)
retains earlier recovery results. Historical fixture passes are inputs to this
work, not completed production subcomponents.

## Revised receiver mapping

| Original checklist obligation | New owning S |
| --- | --- |
| W1.0 backing/shared ABI/TEB/clock; W1.1 worker domain | S41; real aggregate USER16 consumer acceptance in S45 |
| W1.2--W1.5 task/scheduling/callback; all W2 windows/classes/messages/dialogs/cleanup | S42, one lifecycle including object publication into S41's data plane |
| All W3 GDI/resources/fonts and W4 clipboard/DDE | S43, including menu/resource publication and release |
| All W5 kernel/DOS/OEM and W6 remaining families/error delivery | S44, including complete original registrar production wiring |
| W7 whole-provider real guest acceptance, OEM-WOW-DIR/DELETE, final cleanup/diff report | S45 |

The detailed gates below retain their original wording as scope inventory.
Where they prescribe S40 execution, W7-only registration or a fixed paging
implementation, the revised proposal supersedes them: S44 wires the complete
table, S45 verifies all real consumers, and S41 first proves the live machine
mapping state. The withdrawn page-map experiment is not an accepted design.
Existing unchecked gates remain unchecked. S40 research closure checks none
of them and does not certify the pending implementation candidates.

## Earlier completion rule (scope inventory)

One unchecked item is the current implementation focus. Finish its complete
production path before starting another independent item. A discovered direct
dependency may be implemented in the same chain, but is named here rather than
left as an unnamed future binding. Do not publish partially populated callback
tables or success stubs to make a startup test pass. Do not copy fixture
implementations into production when the corresponding original owner can be
recovered.

Rows W1--W6 are checked only when their complete selected source package,
worker binding and direct production-boundary tests are closed. They are not
allowed to claim a Win16 pass independently: `UserRegisterWowHandlers`
publishes one all-or-nothing 20-slot contract, and W1's original yield and
teardown bodies directly call W2's message receiver and cleanup owner. W7 is
therefore the sole whole-provider guest-acceptance gate, not deferred cleanup
work. The dependency is explicit so that a checked W1--W6 row means a complete
component ready for that aggregate call, rather than a disconnected fixture.

Every checked W1--W6 item must have all of the following in its evidence:

1. Complete selected-source/entry inventory and original owner, including
   negative, asynchronous and teardown paths; no trace-selected subset.
2. Real production caller -> original body -> modern boundary -> return,
   with required initialization, ABI, ownership and synchronization connected.
3. Normal, failure, reentrant/concurrent where applicable, and exit cleanup
   tests against that same production implementation. No parallel test body.
4. Formal x86 build and all 17 established DOS routes, including direct and
   nested COMMAND/MEM/EDIT, with guest text checked rather than exit code alone.
5. Recorded source revision, artifact hashes, commands, logs, remaining
   limitations and mirror/adapter diff counts. Completed delivery is committed
   and pushed; an unchecked dependency cannot be hidden in its closure record.

W7 additionally requires real immutable Win16 guest reachability and
observable task/callback/return/teardown results for every W1--W6 component on
the same complete 20-slot table. Host fixtures supplement difficult negative
paths; they never replace that aggregate acceptance.

Original guest limitations require source-proven attribution and the owner's
standing disposition; they are not functional passes. Other exclusions require
the existing packet's owner-approved disposition. Guest media stays immutable.

## Ordered production units

- [ ] **W1 — USER domain and task lifecycle (current focus).**
  Complete one worker-local USER process/current-thread domain, task creation,
  scheduling/wait, directed yield, hung-app registration and callback
  enter/return. Bind actual worker main and additional WOW threads; preserve
  original release/callout/reacquire and borrowed-object lifetime. Finish
  startup failure, task exit, DLL teardown and worker termination. This row
  owns output slots `pfnDirectedYield`, `pfnInitTask`,
  `pfnRegisterUserHungAppHandlers`, `pfnWowWaitForMsgAndEvent` and
  `pfnYieldTask`. Its direct W2 dependencies are the original message receiver
  reached by `xxxUserYield` and `pfnWOWCleanup` reached by task destruction;
  W1 cannot be marked ready while either has a synthetic/no-op implementation.
   W7 witnesses an original task entering, waiting/yielding, exiting and a
   subsequent DOS workload remaining usable. Shared data correctness is tested
   from the guest at W7, not inferred from native pointers.

  W1 is one complete unit, but it has the following ordered, separately
  evidenced closure gates. A gate may be ticked only when it uses production
  source (not a copied fixture body), covers normal and failure cleanup, and
  passes the established DOS regression matrix. Ticking a gate does not publish
  any part of `PFNWOWHANDLERSOUT`; publication remains W7's all-or-nothing
  operation.

  - [ ] **W1.0 — PMODE32 shared-data plane (prerequisite to W1.1).** Publish
    the immutable USER16-selected `SHAREDINFO`, `SERVERINFO`, 16-byte handle
    entries, thread `CLIENTINFO`/`DESKTOPINFO`, and shared clock through actual
    guest-visible linear mappings.  Publish the original flat-data/TEB
    descriptors for every WOW thread and withdraw them on task/worker exit.
    Verify the pinned USER.EXE's 47 byte-level consumer accesses against this
    production mapping, including invalid/stale handle rejection and
    per-thread TEB changes.  A host pointer, a zero-filled structure, or a
    CCPU fixture mapping is not a substitute: this gate must be reachable
    from the real PMODE32 guest before any `UserRegisterWowHandlers` result
    is stored in guest memory.

    W1.0 is deliberately one finished data-plane unit, not a collection of
    host fixtures.  Its gates below are ordered because each later gate uses
    the exact backing and lifetime established by the preceding one.  None
    authorizes callback-table publication on its own.

    - [ ] **W1.0.a — CCPU paging ownership and physical backing.** Reuse the
      selected original `VdmAddVirtualMemory`/`VdmRemoveVirtualMemory` and
      CCPU page-table walker to publish native-owned backing at the fixed
      shared-clock linear page and a bounded USER shared range.  Preserve
      identity access to ordinary SAS RAM, enforce user read-only clock
      access, and suspend/re-establish the mapping in the actual DOSX
      real/protected transitions without replacing a CCPU instruction or
      physical-access routine.
    - [ ] **W1.0.b — exact PMODE32 shared ABI plane.** Materialize the pinned
      original `SHAREDINFO`, `SERVERINFO`, DEBUG-layout 16-byte
      `HANDLEENTRY` table, clock and `CallCsrFlag` storage in that same guest
      range.  Populate only source-defined values and reject unallocated or
      stale handles; no host pointer, zero-success structure or compiler
      `sizeof` inference may cross this boundary.
    - [ ] **W1.0.c — per-WOW-thread TEB and descriptor publication.** Use the
      existing original CCPU GDT/LDT descriptor path to publish the required
      flat-data/TEB selectors, `CLIENTINFO` and `DESKTOPINFO` for the worker
      main and every original `W32Thread`.  Prove bind failure rollback,
      distinct concurrent thread views, task exit and worker exit withdrawal.
    - [ ] **W1.0.d — production PMODE32 consumption.** Reach the plane from
      the immutable USER.EXE before registration results are written, verify
      all pinned consumer families (including clock reads during uninterrupted
      guest execution), then repeat the existing direct/nested DOS matrix.
      This gate must prove the real production wiring, not merely the bounded
      CCPU backing fixture.
  - [ ] **W1.1 — one worker USER domain.** Bind the original worker main,
    every `W32Thread`-created worker thread and worker teardown to exactly one
    runtime/domain instance. Prove duplicate bind, failed bind, callout
    release/re-entry and worker termination do not retain the domain.
  - [ ] **W1.2 — original process registration.** Drive
    `xxxRegisterUserHungAppHandlers` through the production carrier, retain
    and release the real WOWEXEC event, and prove duplicate registration,
    startup failure and worker teardown.
  - [ ] **W1.3 — original task construction/destruction.** Drive
    `xxxInitTask` and `DestroyTask` for each actual WOW worker thread, with
    task TDB identity, native queue membership, class/object roots and full
    rollback on each allocation or initialization failure.
  - [ ] **W1.4 — original scheduling/wait.** Drive `xxxUserYield`,
    `xxxDirectedYield` and `xxxSleepTask` against the one real USER32 queue
    plus the original event set. Prove cross-task delivery/reply, alert or
    cancellation, task exit and broker/worker loss without a shadow scheduler.
  - [ ] **W1.5 — callback machine boundary.** Prove the production
    `CallBack16` frame lease, recursive CCPU `host_simulate`, original WOW16
    `BOP 51` ingress and `BOP_UNSIMULATE` callback return, and restoration of
    the calling task frame on success, callback failure and task teardown. A
    fixture that replaces `CallBack16` is regression-only and cannot satisfy
    this gate.
- [ ] **W2 — Classes, windows, messages and dialogs.**
  Original registration/unregistration, creation, subclass/procedure dispatch,
  message send/post/reentrancy, private WW/WC access, dialog operation and
  window/task/module cleanup. Native object enrollment, failure rollback and
  class/window references must form one lifecycle. It supplies W1's direct
  message-receive and task-cleanup dependencies as part of that same lifecycle;
  no records or callbacks survive their owner. W7 witnesses actual Win16
  create/use/destroy and nested callback return, including failure cleanup.
  `taskman.c` reaches the original `userk.h::xxxReceiveMessages` macro, whose
  concrete target is `kernel/sendmsg.c::xxxReceiveMessage`.  That translation
  unit is part of the excluded NT4 USER server boundary: it depends on private
  SMS lists, server PWNDs, client callbacks and kernel cleanup.  It must not be
  imported recursively or replaced with a successful no-op.  W2 instead owns a
  bounded, source-recorded bridge to the real modern USER32 queue: it must
  preserve synchronous-send and posted-dispatch ordering, nested callout,
  cancellation and task/worker teardown using native queue operations.  W1 may
  call that bridge only after its production failure and cleanup behavior is
  tested.  This is a finite public-USER boundary, not permission to recreate a
  USER server.
  This row owns
   `pfnCsCreateWindowEx`, `pfnGetClassWOWWords`, `pfnRegisterClassWOWA`,
   `pfnServerCreateDialog`, `pfnWOWCleanup`, `pfnWOWFindWindow` and
   `pfnGetFullUserHandle`.

  W2's finite public-USER boundary is split below so each complete original
  owner is recovered before the next is claimed. Every gate requires its
  production caller -> original body -> boundary -> return path, its failure
  rollback and owner teardown, formal x86 linkage and the established DOS
  regression matrix. These gates intentionally do not publish an individual
  handler slot.

  - [ ] **W2.1 — class registration and private class words.** Complete
    original `RegisterClassWOWA`, `GetClassWOWWords`, class lookup and
    unregister paths, including duplicate class, bad atom/name, module
    ownership and task/module teardown.
  - [ ] **W2.2 — window creation and private window words.** Complete the
    exact `PFNCSCREATEWINDOWEX` entry shape, original window-word acquisition
    and publication, class target resolution, creation-message ordering,
    native failure rollback and `WM_NCDESTROY` release.
  - [ ] **W2.3 — messages, subclass and nested callbacks.** Complete original
    `W32Win16WndProcEx`/subclass dispatch through W1.5's real guest callback,
    including synchronous send, posted dispatch, nested callback return,
    cancellation and destroyed-window behavior.
  - [ ] **W2.4 — dialog creation and procedure lifetime.** Complete
    `ServerCreateDialog`, original `W32InitDlg`, `WM_INITDIALOG`, procedure
    replacement, control creation, failure rollback and final destruction
    through the real callback boundary.
  - [ ] **W2.5 — find/handle/cleanup contract.** Complete `WOWFindWindow`,
    `GetFullUserHandle` and `WOWCleanup` against the same class/window/object
    ownership graph; prove cross-task lookup rejection, stale handle rejection
    and no retained object/class/window after task or worker exit.
- [ ] **W3 — GDI, fonts and USER resources.**
  Complete selected GDI thunk families, DIB/bitmap conversion and mappings,
  font/glyph contracts, menus, icons and cursors, resource alias ownership and
  release. Witness: real Win16 drawing/resource consumers with checked content
  and repeated load/use/free; malformed input and allocation failure are
  supplemental production-boundary tests. No resource producer is exempt from
  the object/lifetime contract established by W1/W2. This row owns
  `pfnServerLoadCreateCursorIcon`, `pfnServerLoadCreateMenu`,
  `pfnWOWGetIdFromDirectory`, `pfnWOWLoadBitmapA`, `pfnGetMenuIndex`,
  `pfnWowGetDefWindowProcBits` and `pfnFillWindow`.
- [ ] **W4 — Clipboard and DDE.**
  Complete selected format conversion, ownership transfer, original DDE data
  release, messaging and peer/task teardown. Witness: real guest transfer and
  return with both success and rejected/abandoned exchanges; verify no double
  free, stale ownership or retained object after exit. This row owns
  `pfnFreeDDEData`.
- [ ] **W5 — Kernel/DOS services and OEM integration.**
  Complete selected WOW32 kernel/DOS thunk families, memory/module/task-facing
  services and file/directory/environment behavior. Include OEM-WOW-DIR and
  OEM-WOW-DELETE in full: non-ASCII guest paths, ordinary and retained-file
  deletion, font fallback, rollback and temporary-file cleanup. Existing DOS
  service acceptance does not stand in for these Win16 consumers.
- [ ] **W6 — Remaining selected thunk families and error delivery.**
  Reconcile the complete WOW32 build manifest and dispatch tables against
  W1-W5; assign every remaining selected entry here before its implementation
  begins, including COMM, printing, hooks, sound or other selected families.
  Recover each family's complete provider/lifecycle or apply an explicit
  source-proven, owner-approved exclusion. Validate private hard-error display,
  response, cancellation and task termination in a real worker. This is not
  permission to drop a family merely because WRITE does not call it.
- [ ] **W7 — Whole-provider acceptance and durable delivery.**
  No unresolved selected entry, unbound operation or fixture-only replacement
  in W1-W6. At this point, and not earlier, compile the recovered original
  `client.c::UserRegisterWowHandlers` and publish its complete 20-slot table
  from the completed subcomponents; verify its shared-data return from the
  guest. Run real WRITE startup/use/exit, repeated Win16 task and callback
  teardown, all 17 DOS routes and the complete retained production tests on
  the same artifact set. Publish only verified artifacts to O:/winnt, record
  logs under O:/winnt/logs, commit/push and report exact diff reductions and
  necessary remaining boundaries. S40 completion does not close T420; the
  owner retains final T acceptance.

## Baseline and first action

All rows remain unchecked. The current formal graph compiles recovered USER
cores and native bindings, but UserRegisterWowHandlers still imports modern
USER32; linked symbols do not establish a usable local output table. W1 starts
with the shared production domain/task lifecycle; W2-W4 then finish every
registered callback owner; W7 alone publishes the complete original table.
Existing fixtures are reused as regression tests of production code. A failing
dependent contract keeps its owning row open until resolved.
