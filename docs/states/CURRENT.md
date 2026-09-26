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
Owner's final approval: each launcher and its target are one lifetime pair;
neither may be orphaned when its counterpart exits. Between nesting levels,
retain original OpenNT waiting/re-entry/exit, not a new recursive process-tree
kill policy. Add matching launcher cleanup, not a guest task-kill mechanism.
An unrecoverable guest fault may end its worker and
all tasks inside it, including a DOS ancestor; recoverable interactive parents
outside that worker survive. Normal completion must not be escalated into worker
failure. This supersedes the earlier unconditional same-worker parent-survival
requirement and resolves that decision. Frontend identity, execution ancestry
and worker membership remain distinct; unrelated workers/broker are preserved.
The proposal's revised fault matrix remains an S2 exit gate; /c chain exits alone
do not prove interactive-parent recovery. An empty live worker remains permitted.
Current S2 evidence is retained in the
[boundary ledger](../etc/evidence/m0-t423-s2-console-boundary-ledger.md);
it is the detailed record of superseded attempts, not an additional status.

## S2 Verified Scope And Remaining Gates

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
- Native identity, malformed-request, cancellation, route/channel retirement
  and pair-lifetime fixtures pass. Selected real worker/broker/frontend/inner-
  launcher and middle-native-pair failures pass, including actual interactive
  parent recovery and unrelated-worker output. Middle-pair recovery is tested
  with the descendant DOS task alive, without a second injected fault.
- DOS17, four native/DOS nesting and typeahead routes, and real guest
  keyboard/mouse callback tests pass on the recorded candidates. Actual
  Ctrl+C/Break and direct/nested Console close also pass. These checks do not
  certify physical focus/clipping cleanup, logoff, or Window hotkeys.
- The existing worker-thread keyboard-layout fallback is not equivalent to
  original Console-selected layout. Native historical Console queries fail on
  this host; a disposable thread-local experiment proves the distinction.
  Its ownership decision remains open; no speculative replacement was added.
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

S2 still requires the complete Console-owner and per-layer failure checklist,
and remaining physical pointer/layout dispositions. The first production P
delivers the verified subset, not S2 closure. Hidden native Console remains S3;
display/Window remains S4, Window mouse S5, and final owner audit S6.

O:/winnt now contains the tested coherent six-file S2 package. Its exact hashes,
test prefixes and bounded delivery scope are in the boundary ledger's first
production delivery record. The recoverable S1 set and incremental caches stay
under build. Remaining work continues in S2; do not re-admit S3 yet.

## Current Technical Baseline

S1 production revision c39b9ca0c contains launcher standalone-Console retirement
and non-inherited Ctrl+C/Break completion handling, plus the authorized dtmgr
layout correction. Its [evidence](../etc/evidence/m0-t423-s1-restart-lifecycle.md)
records DOS17 and lifecycle passes and the specific owner build-only exception
for incomplete current verification. That exception is not a WOW runtime pass.
Temporary SYSTEM.INI changes were restored. The first S2 production delivery
supersedes S1 at O:/winnt with protocol-5 BaseSrv and protocol-9 direct Console
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
