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
Owner's latest admission maps the authenticated root run16 lifetime to the
interactive Console session lifetime. Root normal/abnormal exit or real Console
closure closes only the DOS workers associated with that frontend, through the
original VDM close path and a bounded close fallback. Non-root launcher loss
does not terminate handed-off targets or workers. Display switching is not
session closure. Native descendants and unrelated workers are not tree-killed.
Completed direct-target results must survive later worker cleanup; unfinished
DOS records fail explicitly when their worker closes. No guest changes or new
scheduler. This supersedes the no-root-termination contract and the proposed
input-pump retirement exception below; their observations remain historical.
Implementation and the replacement lifecycle regression are now admitted in S2.

## S2 Verified Scope And Remaining Gates

The preceding unpaired-lifecycle package remains historical evidence, not the
current root-session policy. It removed native kill-on-close Jobs and non-root
launcher-driven worker termination; those removals remain. Root loss now means
interactive session close, as admitted above. The proposed input-pump retirement
exception is withdrawn rather than implemented.

Current implementation calls the unchanged original CntrlHandler on a bound
worker thread when the authenticated root process ends. Unresolved close is
bounded; pipe errors alone do not invoke it. Dead-root command rebinding is
removed. All native Console presentation remains run16-owned.

The original BaseSrv ExitVDM cleanup was also proved to discard results before
the process-death observer could preserve them: two new red fixtures expose
completed 29 becoming zero and unfinished work not returning failure. The
existing parent-result preservation is now shared by orderly DOS-worker ExitVDM
and process rundown, without changing mirror record policy. The final x86
build, 24 service modes, three client-close cases, DOS17, native/DOS nesting,
keymouse, graphics/text return, root/non-root and broker/worker faults pass.
Native CUI/GUI targets and descendants survive launcher loss; associated DOS
workers close on root end, with correct completed/interrupted outcomes.
WOW3 retains its recorded headless modal frontier. The exact tested six-file
set is published at O:/winnt; the prior package remains recoverable in build.
See the boundary ledger's root-session-close section for hashes and commands.

Retained earlier S2 capabilities and source findings:

- Protocol 10 and authenticated single-root channels carry DOS Console input,
  text/cursor/geometry/layout and copied graphics/palette operations. BaseSrv
  authorizes endpoints but never transports frames or input.
- Original guest keyboard/mouse/video and command re-entry remain selected.
  OPENNT-HOST-064 retains a coalesced original native-return notification;
  DIV-312 fixes the original returned-key count/history index defect.
- Native Console-selected keyboard-layout failure is preserved; the incorrect
  thread-HKL fallback is gone. Text and local-graphics pointer counts are
  independent and do not mutate the host text caret.
- DOS17, native/DOS nesting, guest keymouse and graphics/text-return have
  preceding production evidence. Every new production P must rerun affected
  gates and publish the exact tested coherent six-file set.
- The old NOIO test demonstrated an input-thread fatal-modal versus completion
  handshake stall after root loss. Its replacement gate now asserts session
  closure, retained root result and explicit unfinished inner-task failure.
- WINMINE/SOL/WRITE manual foreground/playability is waived for this T only.
  Separate headless frontier comparisons remain required; the NETWORK.DRV
  modal baseline is not a full WOW functionality pass.
- Channel cancellation, peer EOF, malformed requests, worker loss and receipt/
  completion ordering have focused tests. Physical foreground activation and
  pointer clipping/release remain unverified. Owner forbids desktop interaction;
  continue safe background checks and do not claim these gates passed.

The [boundary ledger](../etc/evidence/m0-t423-s2-console-boundary-ledger.md)
retains detailed superseded attempts, exact source/artifact identities and
non-pass outcomes. S2 remains open until its complete source/runtime and
physical-input disposition is resolved; do not re-admit S3 yet. Hidden native
Console is S3, display/Window S4, Window mouse S5 and final owner audit S6.

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
