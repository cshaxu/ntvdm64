# T449 S4 — integrated reconciliation ledger

## Purpose and package rule

S4 is the one final, bounded reconciliation of the transferred T447 execution
obligations, T448 lifecycle boundary and T449 S2/S3 deliveries.  It does not
reopen accepted implementations merely because their evidence is historical.
Each row below must instead be re-established against one sealed S4 package,
or be retained as a specifically attributed limitation.  No guest source,
guest binary or guest configuration is a repair surface.

The selected production chain is immutable Win16 media -> `run16.exe` ->
`ntsrv.exe` -> `ntvdm.exe` -> original WOW32/USER conversion and scheduling
bodies -> modern USER.  NTSRV remains the lifecycle authority; management
uses the existing RPC and worker-base event path.  Host HWND/process resources
belong to Windows; project adapters only map identities, call lifetime and
execution boundaries.  This restates the approved ownership boundary rather
than creating a replacement message queue, scheduler or control engine.

## Fixed row matrix

| Row | Production owner and entry | S4 proof entrypoint | Required result / bounded edge |
| --- | --- | --- | --- |
| C01--C03, C09, C10 / M1 | Original `wumsg.c`, `wmsg16.c`, `wmdisp32.c`; public USER bridge | `verify-win16-message-probe.ps1` normal, cross-worker and external-native modes | Same/cross task, post/get/peek/filter/quit and both native directions.  Mode-specific assertions remain distinct. |
| M2 | Original `WU32InSendMessage`/`WU32ReplyMessage`; current-thread public USER facts | Message-probe reply/pending modes plus source review | Explicit/implicit receipt, nested continuation, early/duplicate/no-pending replies without synthetic NT4 SMS. |
| M3 | Original `taskman.c` scheduling and `nt_eoi.c::BlockWOWIdle`; adapter execution scope | Message-probe wait/nesting modes and original exception witness | Return/reentry execution ownership; no replacement scheduler or private USER state. |
| M4 | Original message/DDE converters; call-owned leases/finalizers | construction, classic-DDE and rejected-transfer probes | Exact conversion/result ownership and one cleanup on normal/failure paths; unselected DDE families remain queued, not silently passed. |
| M5 | Original retirement plus project window/task binding cleanup | pending receiver destruction, normal retirement, S3 ticket tests | No stale/reused target, duplicate completion or failed-request success. |
| M6 | Combined real guest, native peers and production provider | sealed-package message/application/lifecycle suite | Two Win16 tasks, both native directions, nested/reply/wait/failure/normal exit. |
| Per-task close | Original `W32HungAppNotifyThread` / `W32DestroyTask`; authenticated NTSRV ticket | `basesrv-service-reservation-test.exe`, `worker-input-watch-test.exe`, recorded desktop two-task witness | Target-only close, sibling survival, reuse, duplicate and distinct worker close. |
| Controls/dialog/menu | Selected original EDIT/dialog/MENU-01 paths; native child resources | existing actual control/dialog probes under all three style modes | Retain selected original owner; do not substitute host controls.  Common-dialog anomaly and raw-fault recovery need explicit disposition. |
| Applications | Original WINMINE/SOL/WRITE guests with host runtime | mine, SOL and WRITE scripts against sealed package | WINMINE/SOL normal use; WRITE edit/save/exit.  Small-canvas SOL and intermittent game exit are only risks unless reproduced/attributed. |
| DOS/lifecycle | T448 NTSRV/worker-base/NTCON control path | published CAF/console suite | Normal, external close and self-delete handoff on the sealed package. |

## Existing evidence and no-inference boundaries

S2's current-package DDE and message evidence is retained by
`m0-t449-s2-dde-ownership-audit.md`,
`m0-t449-s2-message-scheduler-audit.md`, and
`m0-t449-s2-retirement-source-audit.md`.  S3's actual two-task desktop
witness is retained by `m0-t449-s3-targeted-termination-progress.md`.
Their package-specific facts are inputs, not blanket proof for rows they did
not exercise.  The inherited source denominator and each mode's exclusions
remain in `m0-t447-s4-message-source-audit.md` and
`t447-wow-execution-plan.md`.

The following outcomes are deliberately not silently promoted to passes:

* historical common-dialog identity anomaly;
* styled raw-fault recovery;
* WRITE edit/save/normal exit (startup is insufficient);
* direct-game intermittent exit; and
* the owner-reported SOL geometry, whose assertion must reflect actual host
  geometry rather than impose a small-canvas requirement.

## S4 execution order

1. Build one complete cached thirteen-image package and hash it before any
   runtime conclusion.
2. Run the fixed message/conversion/lifecycle groups with their exact
   prerequisites and modes; record every package hash and negative result.
3. Run the real application and DOS witnesses, including WRITE edit/save/exit.
4. Sweep selected original sources, adapters and divergence register for each
   changed or exercised chain; repair only source-traced host defects as a
   coherent group.
5. Publish the resulting package to both destinations, rerun the final
   selected acceptance matrix, perform an independent diff review, then
   commit/push.  A retained limitation is reported with its cause and scope;
   it never becomes a passing row.

## First sealed-package results — 2026-10-23

`build/t449-s4/build-r1` passed its complete cached thirteen-image build and
second no-work pass (1,470 tracked intermediate files).  Its `package-r1`
stage is a fresh copy of the immutable `winnt.zip` baseline with exactly those
thirteen host images hash-checked into `SYSTEM32`.

The S4 candidate passed the actual message groups below.  They are retained
as mode-specific evidence, not a claim for message families excluded by their
scripts.

| Group | Result |
| --- | --- |
| external native | local, independent x86 peer and independent x64 peer × Off/Win16/All: 9/9 pass (`message-external-r3`) |
| cross worker | local and cross-worker × Off/Win16/All: 6/6 pass (`message-cross-r1`) |
| pending send / destruction | local and pending × Off/Win16/All: 6/6 pass, plus required nonblocking-host negative (`message-pending-r1`) |
| S3 ticket/service edge | `basesrv-service-reservation-test.exe` pass, including opaque task ticket retry/retire; `worker-input-watch-test.exe` pass with 641 assertions |
| WINMINE | actual flag/counter, reset, open-cell, `WM_CLOSE` and normal Run16 completion pass (`mine-off-r3`) |
| SOL | actual stock draw, Deal/waste clear, second stock draw, `WM_CLOSE` and normal Run16 completion pass (`sol-off-r5`) |
| controls/dialog/common dialog/hook | 12/12 actual immutable-guest probes pass: each of controls, dialog, common dialog (16 identity/reuse rounds) and hook across Off/Win16/All (`guest-contracts-r1`) |
| CAF console handoff | self-delete cmd/command/ntmon 3/3; natural cmd/command 2/2; external-close cmd/command/ntmon 3/3.  Every row proves restored echo before Enter (`caf-self-r3`, `caf-natural-r1`, `caf-external-r1`) |

`sol-off-r1`'s former close failure was an observer bug, not a product result:
after `WM_CLOSE`, the old numerical HWND remained valid/reusable, while no
visible same-PID `Solitaire` window remained.  The SOL probe now re-enumerates
the visible class/PID identity on each close poll; `sol-off-r5` then proves
normal closure and Run16 completion.  This is the M5 rule applied to the
test oracle rather than a product repair.

## Remaining bounded S4 item

WRITE startup is retained and the current 12-case common-dialog matrix proves
the selected WOW/common-dialog bridge, but full real WRITE edit/save/exit is
not yet a pass.  Source tracing established why the first owned probe was not
a valid substitute: WRITE's original main loop consumes `WM_CHAR` from its
own `MSWRITE_DOC` queue (`wow16/write/cmd.c::KcAlphaKeyMessage` and
`mdoc.c::EatOneCharacter`), whereas its Save As flow crosses the original
`wcommdlg.c::WCD32GetFileName` thunk and a modal native dialog/hook.  A probe
on an unswitched private desktop can enumerate these windows, but cannot use
the desktop's physical input queue; synchronous delivery bypasses WRITE's
queue algorithm and the modal dialog remains open.  That proves a test-driver
boundary only.  It is neither evidence that immutable WRITE cannot save nor
evidence of a host-side product regression.

The finite next step is an owned input-capable private-desktop harness (or an
equivalent visible-desktop, isolated test session) that sends normal input
through the existing frontend path, then proves document content, Save As,
normal Exit and Run16 completion.  No source repair is admitted unless that
witness shows an actual bridge failure.  Therefore S4 is not ready for final
publication/closure solely because this one required application witness is
unproved.
