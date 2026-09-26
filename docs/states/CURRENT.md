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
No new user-facing frontend capability is claimed at admission.

## Current Technical Baseline

S1 production revision c39b9ca0c contains launcher standalone-Console retirement
and non-inherited Ctrl+C/Break completion handling, plus the authorized dtmgr
layout correction. Its [evidence](../etc/evidence/m0-t423-s1-restart-lifecycle.md)
records DOS17 and lifecycle passes and the specific owner build-only exception
for incomplete current verification. That exception is not a WOW runtime pass.
O:/winnt contains the final six-file S1 build; temporary SYSTEM.INI changes were
restored. S2 has not yet changed production or deployed files.

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
