# Project Status

## Current Work

## Active Packet

**Active: M0 T423 S2**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T423 S2, Ordinary Mode. |
| Candidate Proposal | [Console/Window frontend](../proposals/proposal-kvm-window-graphics-presentation-001.md). |
| Admission And Approval | Owner-approved six-stage restart and automatic sequential S admission; owner says continue after S1 delivery. |
| Objective | Move DOS visible-Console input/presentation to run16 through a bounded authenticated worker I/O protocol; preserve original guest devices, frames, execution and native re-entry in ntvdm. |
| Non-goals | No hidden Console (S3), Window/display/library import (S4), guest mutation, second scheduler, or broker frame transport. Native children still use the visible Console directly. |
| Reference Baseline | S1 c39b9ca0c, pushed to main with the owner's one-time exact force-with-lease authorization; prior prototype preserved on codex/t423-original-reference-20260925 at 286d54a30. |
| Files And ABI Surface | run16 frontend/channel; ntvdm worker I/O bindings; minimal registered original Console hooks; basesrv authenticated association only if necessary; build graph and tests. No new directories outside build. |
| Applicable Rules | EXECUTION, source policy, original mirror/ABI and output-hygiene rules; the owner-approved frontend split is the product-boundary exception. |
| Verification | Original-call-to-owner/protocol/assertion ledger; identity/version/range/ordering/disconnect tests; real DOS17, keyboard/mouse/scroll/cursor, nested native return and S1 lifecycle; x86 build and six-file publication plus independent WOW frontier checks for production P. |
| Expected Markers | Only run16 reads/presents user DOS Console I/O; original guest input/video consumers remain selected; input releases/output drain survive native handoff and teardown; actual COMMAND/MEM/EDIT output and behavior pass. |
| Asset Needs | Existing source and immutable media; no imported library in S2. Build/cache/fixtures below build/M0-T423/S2; runtime tests/logs in existing O:/winnt/tests and logs. |
| Reporting Requirements | Finite callsite ledger, source-first recovery exceptions, protocol ownership and failure rules, exact tested artifacts and non-pass outcomes; no research-only capability claims. |
| Stop Conditions | Guest changes, new scheduling policy, unauthenticated endpoint ownership, broadened scope or proven regression. |
| Exit Criteria | Production DOS Console I/O through run16 and original execution/re-entry preserved; full S2 checklist and production P gates met, coherent publication, commit/push and clean worktree. S1 build-only exception does not apply. |
| Original Owner Request | run16 owns all user KVM; ntvdm manages input queues/output events like machine, run16 like UI; first migrate DOS I/O, then hidden native Console, then Window. |
| Similar-Issue Sweep | Original event pump and peek/flush, raw/cooked restoration, text-stream versus video writes, screen buffer/cursor/resize, native handoff, nested frontends, multi-worker isolation, disconnect and final-output drain. |

S2 begins with a bounded source/owner audit, then protocol implementation and
production wiring; declaration-only tests cannot close this packet.
Owner's latest lifecycle revision supersedes launcher/target lifetime pairing.
Execution ancestry and root-frontend I/O association are independent. A root or
inner launcher exit must not actively kill its native target, DOS task, worker
or descendants. Target completion still returns only to its direct launcher:
the native process result or that DOS record's completion/exit code. Worker
failure independently fails its unfinished tasks; launcher loss is not worker
failure. Root exit revokes its I/O capability; a later I/O user receives explicit
frontend-unavailable/disconnect failure through its own original handling, not
host/broker termination of the worker. Retain only pre-handoff startup rollback.
No recursive kill, per-layer worker, injected DOS task termination or new
scheduler is admitted. The proposal's replacement fault matrix is an S2 gate.

On explicit owner request, the prior tested notification repair was committed
and pushed first as 9f9b49346 with a clean synchronized worktree. The next S2 P
implements this new contract: remove long-lived native kill-on-close Jobs,
service_end_abandoned_dos_pair termination and frontend-loss execution kills;
revise their old positive-kill tests into survival/I/O-error/completion tests.
The new six-file package now implements the revised contract. Historical passing
pair-termination tests below are superseded evidence, not new-rule exit gates.

New P: native long-lived Job creation, launcher-driven abandoned-DOS
termination and frontend-failure execution waits/kills have been removed;
authenticated frontend loss returns ERROR_PIPE_NOT_CONNECTED (233), while
actual worker failure remains 1067. Repeated input/output/control failures do
not kill the client or fall back to native Console presentation. Existing
pre-handoff startup rollback and explicit worker termination remain.
MSVC x86 six-file build, 18 service modes, real native CUI/GUI direct-result,
launcher-loss and descendant-survival cases, root/inner/middle nested faults,
DOS17, four native/DOS nesting cases, graphics/text return and guest keymouse
tests pass. WOW3 retains the recorded live NETWORK.DRV modal frontier; this is
headless non-regression evidence, not full application acceptance.
The exact tested six-file set is published to O:/winnt; hashes and reproduction
are in the boundary ledger's unpaired-lifecycle production section. Product
sources change by +22/-187 lines, with no new mirror diff or wire layout.
This P does not close S2: the remaining source-to-runtime checklist, physical
focus/pointer evidence and fault/completion race audit remain explicit gates.
Current S2 evidence is retained in the
[boundary ledger](../etc/evidence/m0-t423-s2-console-boundary-ledger.md);
it is the detailed record of superseded attempts, not an additional status.

## S2 Verified Scope And Remaining Gates

Follow-on P preserves an already-completed DOS result across later worker
cleanup: the deterministic red test returned 0 instead of 29; the service now
collects the original result before cleanup and retains its authenticated reply
until the direct parent consumes it. No mirror, guest or wire-layout change.
All 22 service modes, DOS17, four nesting cases, graphics/text, keymouse and
worker/root/inner-launcher failure regressions pass. WOW3 retains its known
headless NETWORK.DRV frontier. The tested six-file candidate is published at
O:/winnt; exact identities are in the boundary ledger's completed-result section.
This closes the named result-loss sequence, not the entire S2 race checklist.

The unchanged published result-fix package also passes two-worker broker loss,
native-root inner-target loss, innermost launcher loss and middle-launcher loss
with live descendant recovery (no second injected fault). Outer interactive
CMD returns 23; unfinished descendants survive root completion; fresh MEM
prints its actual report. Detailed switches and limits are in the boundary
ledger's post-result-fix matrix. Physical pointer/focus remains unverified.

Channel terminal-path coverage now adds malformed-request shutdown and a real
process-death wait: exact error, readiness, actual peer EOF, safe stop/join and
stable handle count pass twice across 85 cases each. Broker attachment alone
is substituted. This test-only update does not change the published binaries
or certify physical pointer/focus behavior.

Detailed runs, failed attempts and artifact identities remain in the linked
boundary ledger; this status records only the current disposition.

- The first S2 production delivery implements authenticated root-frontend association, copied
  Console operations, original-command rebinding and per-layer launcher/target
  cleanup. Nested run16 instances do not acquire a second frontend. BaseSrv
  carries association/capabilities, never input records or video frames.
- Protocol 9 covers input, text/stream/cell operations, code pages, title,
  font, display-mode and pointer boundaries. Original graphics painters retain
  worker-local bitmap/mutex backing and send copied indexed frames/palettes.
  Real guest pixel/palette and immediate graphics-to-text final-output tests
  pass. This does not implement kvm-window rendering or the display flag.
- Native identity, malformed-request, cancellation and route/channel retirement
  fixtures pass. The latest unpaired-lifecycle tests supersede earlier
  pair-kill expectations. Middle-target recovery is tested with the descendant
  DOS task alive, without a second injected fault.
- DOS17, four native/DOS nesting and typeahead routes, and real guest
  keyboard/mouse callback tests pass on the recorded candidates. Actual
  Ctrl+C/Break and direct/nested Console close also pass. These checks do not
  certify physical focus/clipping cleanup, logoff, or Window hotkeys.
- The worker-thread keyboard-layout fallback was proved non-equivalent to
  the original Console-selected layout. The latest tested package removes
  it, queries the exact native Console API through the frontend (protocol 10),
  and preserves the original NoInstallkb16 branch on failure. Native transport,
  mock success/error and original-reader tests pass. Formal build, DOS17,
  nesting, keymouse, graphics/text return and headless WOW comparison pass;
  the coherent six-file protocol-10 set is now published at O:/winnt.
- The layout candidate passes DOS17 and native API/reader fixtures, but the
  first no-line-delay nested typeahead test lost the x in exit and timed out.
  Separate baseline and candidate repeats pass; the initial failure remains
  not yet uniquely attributed. The count-defect repair below passes the complete
  nesting run and two further unchanged-timing typeahead repetitions. The
  prior failure remains recorded, not retroactively reclassified as a pass.
- A deterministic original host key-return defect is now repaired as DIV-312:
  N copied records were submitted as N+1, with incorrect short-history indexing.
  Exact-function red/green tests prove the repair; formal x86 builds and four
  real nesting/typeahead routes pass. This is not yet proof that it was the
  sole cause of the intermittent lost key. The affected production delivery
  gates pass; no guest or input pacing changes were made. S2 remains open.
- Follow-up fixes a proved graphics-output cursor-counter rejection: the old
  GetConsoleMode validation returned error 6 for locally backed graphics,
  preventing the original show loop from reaching a nonnegative count. Native
  red/green and recreation coverage pass. The formal x86 build, DOS17,
  nesting, keymouse, real graphics/text-return and headless WOW regressions
  pass. The coherent six-file repair is published at O:/winnt; the preceding
  production package remains recoverable under build.
- Owner waives manual/foreground/playability acceptance for WINMINE, SOL and
  WRITE in T423 only. Separate headless launch/liveness/fault comparisons remain
  required. Their known NETWORK.DRV modals are recorded baseline frontiers,
  not full WOW functionality passes. DOS and native lifecycle gates remain.
- The latest incremental x86 build succeeds. The refreshed DOS17 run uses
  m0-t423-s2-delivery-reconcile-dos17 and passes all 17 actual-output cases.
  Win32-to-DOS-to-Win32 inner-target and inner-launcher loss both pass:
  DOS runs MEM and finishes, outer interactive CMD accepts new input, and the
  root returns 23. Other unexecuted fault cases are not counted as passes.
- Root native-target loss under the revised contract now returns FFFFFFFF
  through its direct root run16 while middle/inner launchers, native CMD and
  the DOS worker survive. Fresh MEM produces real output and exits 0.
  Earlier forced-1067 descendant results are superseded, not current behavior.
- Injected focus loss/Ctrl release/focus regain now passes twice through the
  real guest keymouse path, with an unchanged-mode control run. BIOS modifier
  release, callback teardown, MEM output and COMMAND exit are asserted.
  Physical desktop focus and hidden-pointer clipping are not inferred from it.
- Production channel cancellation now passes two 51-case native test runs:
  pending pipe read, empty Console read, and acknowledged barrier/EOF/stop
  competition all join, signal EOF readiness and leave no handle growth.
  Only broker attachment is substituted. This closes the channel-lifetime
  subcase, not broker task-completion races or physical pointer acceptance.
- The original service now has passing normal-completion-before-launcher-loss
  tests both before and after exit-code collection. A completed task returns
  29 and its idle worker/GetNext wait survives late launcher rundown; the two
  revised unfinished-task tests preserve the worker after launcher loss and
  complete its original record with 29. These are service/native-process tests,
  distinct from the real guest topology tests above.

The compiled Console-owner ledger is reconciled with receiver/test and
explicit unavailable/local-owner dispositions. Its object guard passes 366
mirror entries without direct Console/pointer imports except the original
process-local control handler. This is not whole-program runtime proof.
S2 still requires the complete per-layer failure checklist and physical
pointer/focus disposition. Console layout now preserves the original failure
contract; modern native availability is not claimed. The first production P
delivers the verified subset, not S2 closure. Hidden native Console remains S3;
display/Window remains S4, Window mouse S5, and final owner audit S6.

The DOS-root -> native CMD -> nested DOS middle-pair hang is now repaired:
OPENNT-HOST-064 preserves one coalesced native-return notification in the
original Console record until an eligible no-command Get consumes it. The
deterministic early/late, startup-before-increment, nested-count, command-priority
and one-shot checks pass under the revised lifecycle contract. Both real
middle-target and middle-launcher loss return from inner DOS, run outer MEM
and finish; the identical no-fault control also passes. Original queue priority,
event reset, count and task results remain; no guest or new scheduler is added.
Formal x86 builds, DOS17, four nesting/typeahead routes, real keymouse and
graphics/text return pass. WOW3 headless retains its known modal/live baseline.
This repairs the named failure, not the remaining full S2 exit checklist.

O:/winnt now contains the tested coherent six-file protocol-10 S2 package. Its
exact hashes, test prefixes and bounded scope are in the boundary ledger's
unpaired-lifecycle production delivery record. The preceding protocol-10 set,
protocol-9 set, S1 set and incremental caches stay
under build. Remaining work continues in S2; do not re-admit S3 yet.

## Current Technical Baseline

S1 production revision c39b9ca0c contains launcher standalone-Console retirement
and non-inherited Ctrl+C/Break completion handling, plus the authorized dtmgr
layout correction. Its [evidence](../etc/evidence/m0-t423-s1-restart-lifecycle.md)
records DOS17 and lifecycle passes and the specific owner build-only exception
for incomplete current verification. That exception is not a WOW runtime pass.
Temporary SYSTEM.INI changes were restored. The first S2 production delivery
supersedes S1 at O:/winnt; the latest set uses protocol-5 BaseSrv and protocol-10 direct Console
transport; ordinary configuration and guest media remain unchanged. S1 is
retained as the recoverable pre-migration package, not the current deployment.

## S1 Closure Record

S1 bounded implementation/verification and P delivery are complete. On owner
one-time authorization, remote main 7e581d8ea was replaced by c39b9ca0c using
an explicit expected-tip lease; local/remote comparison was 0/0 and the
worktree was clean. The reference branch retains the old remote history.
The force-push authorization is consumed; ordinary pushes remain authorized.
T423 remains open and requires final owner acceptance.
Detailed outcomes and the bounded exception remain in the
[S1 closure evidence](../etc/evidence/m0-t423-s1-restart-lifecycle.md).

## Recent M0 Closures

Prior status chronology is retained in
[restart evidence](../etc/evidence/m0-t423-restart-prior-status.md).
T422 remains closed; historical packets are not reopened.

## Recent Governance

The six-stage plan remains S1 lifecycle, S2 DOS I/O, S3 hidden Console,
S4 display/Window, S5 Window mouse, S6 final audit.
