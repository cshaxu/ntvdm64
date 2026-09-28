# Project Status

## Current Work

T423 remains open. The owner accepted S11 on 2026-09-28 and admitted S12.
S11 commit 965083eec is pushed and its seven-file package remains published
at O:/winnt. S12 begins from that verified baseline, preserving native research.

**Active: M0 T423 S12**

The [T423 proposal](../proposals/proposal-kvm-window-graphics-presentation-001.md)
defines subsequent S12 NTCON native-text backend work and S13 RDP pointer
capture. Native screen continuity and actual-member retirement are transferred
to S12, not claimed as fixed by S11.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T423 S12, Ordinary Mode. |
| Candidate Proposal | [Console/Window frontend](../proposals/proposal-kvm-window-graphics-presentation-001.md). |
| Admission And Approval | Owner: 验收通过，可以收口S11, 准入S12. S11 owner acceptance is recorded; the approved NTCON replacement is now active. |
| Objective | Deliver one registered NTCON native-text backend per character frontend, reused across DOS intervals, with real screen/cursor transfer, actual member lifetime and monitor management. |
| Non-goals | No guest/shared-lib modification, scheduler, recursive process-tree kill, Job/observer lifetime surrogate, private CSR server, RDP repair or automatic T closure. |
| Reference Baseline | Pushed S11 965083eec and its sealed seven-file O:/winnt publication; retained S8 Console mechanics, S9 ConPTY and S11 counterexamples/WIP. |
| Files And ABI Surface | New src/ntcon-exe; ntkvm native backend and copied direct control protocol; ntsrv authenticated registration/management; ntmon display/control; bounded run16 submission binding; build manifests and tests. No DOS/WOW record policy migration. |
| Applicable Rules | EXECUTION, architecture/coding/document rules, source policy and source-first reuse; outputs under build/M0-T423/S12, runtime logs O:/winnt/Logs2. |
| Verification | x86 /MT build; real Console readback and bidirectional handoff; auth/version/instance negatives; actual-member retirement and admission races; redirected streams, exact target results and fault isolation; Console17/Window17, DDWWDDWW, both twelve-target chains, mouse pressure, NTMon and three separate WOW headless frontiers. |
| Expected Markers | One frontend/NTCON across DOS intervals; no stale cells/cursor or duplicate input; no premature session close while attached users remain; idle session retires; registered-session stop confirmed; unrelated sessions survive. |
| Asset Needs | Existing source and retained tests/caches; no new guest, firmware or external import. Source/provenance review before selectively reusing historical mechanics. |
| Reporting Requirements | Maintain [S12 checklist](../etc/evidence/m0-t423-s12-ntcon-backend.md), exact source/build/run/publication identities and true limitations; separate fixture evidence from production acceptance. |
| Stop Conditions | Boundary expansion, required private API/guest mutation, unverified publication, shared-library change, fabricated member count or unexplained regression. Preserve the last verified O:/winnt package. |
| Exit Criteria | All S12 production checklist rows pass normal/negative/lifecycle tests; retained regressions pass; coherent eight-file backup/publication and published-path verification; governance, review, commit/push, then owner verification. |
| Original Owner Request | Create NTCON and integrate with NTKVM, run16, NTSRV and NTMON following existing authenticated backend patterns; owner now accepts S11 and admits S12. |
| Similar-Issue Sweep | Console ownership versus execution ancestry, resource versus attached-client lifetime, direct result versus session completion, stale/PID-reused identities, cancellation/rollback, screen transfer and input ordering. |

## S11 Closure Record

Owner acceptance received 2026-09-28; delivered commit 965083eec is on main
and origin/main. Mouse usability is accepted; S12 is not an S11 mouse deferral.

[S11 evidence](../etc/evidence/m0-t423-s11-interaction-retirement.md) records the
x86 build, restored five-record input batching, bounded mouse FIFO pressure,
IRQ cancellation versus source retirement, and successful DOS handoff reset.
The original mouse IRQ/EOI timing and guest semantics remain unchanged.
The mirror change against the preceding main is +15/-148 lines; key translation
was relocated to the worker adapter rather than counted as deleted functionality.

Verified: Console17/Window17, three real-guest pressure cases (1000/1000/200
records), five mouse cases, ten lifecycle cases, both twelve-target chains,
NTMon in both modes and three inherited WOW frontiers. Published-path DOS,
mouse/pressure and WOW checks also pass; all seven published hashes match.
WINMINE reaches its main window; SOL and WRITE retain original OOM frontiers,
not full application acceptance. Physical desktop focus/capture remains waived,
not tested. No guest or shared-library changes were made.

Publication backup and hash manifest:
build/M0-T423/S11/publication-backup-r1.
Build selection used sealed committed NTKVM source to exclude unfinished
native research. The working tree intentionally retains S12 research and
side-chat Queue/WOW proposal edits; these are not part of S11 production
acceptance and must not be erased or described as a clean working tree.

## Current Technical Baseline

The published product contains run16.exe, ntsrv.exe, ntvdm.exe, ntkvm.exe,
ntmon.exe, WOW32.DLL and VDMREDIR.DLL. The S11 ledger identifies exact hashes
and the sealed committed frontend source selection. NTKVM owns presentation;
NTVDM retains original guest input/video semantics. Deferred NTCON changes
must not be confused with the current executable graph.

## Current S12 Work

[S12 migration ledger](../etc/evidence/m0-t423-s12-ntcon-backend.md):
one authenticated NTCON backend per character frontend, real Console state,
native creation/completion, screen handoff, actual membership and retirement,
NTSRV registration and NTMON management. Preserve and reuse current research.
Begin selective reuse and backend contract implementation under the active
packet above. S13 remains subsequent and owns RDP capture.

## S10 Closure Record

S10 f98825653 delivered bounded component cleanup, not the unresolved native
screen continuity capability; [S10 audit](../etc/evidence/m0-t423-s10-component-minimization-audit.md).

## S9 Closure Record

S9 3b40345f8 delivered the ConPTY baseline;
[S9 ledger](../etc/evidence/m0-t423-s9-conpty-migration.md).
Horizontal wheel is an owner-accepted host limitation, not a passing test.

## S8 Closure Record

S8 d253e55af and subsequent 6ef96410d delivered GUI launch/wait policy;
[S8 evidence](../etc/evidence/m0-t423-s8-gui-launch-wait.md).

## S7 Closure Record

S7 99276d68d delivered Window mouse and product renames;
[S7 evidence](../etc/evidence/m0-t423-s7-window-mouse.md).

## S6 Closure Record

Delivered Console/Window display; [S6 ledger](../etc/evidence/m0-t423-s6-window-display.md).

## S5 Closure Record

Delivered hidden backend; [S5 acceptance](../etc/evidence/m0-t423-s5-hidden-backend-acceptance.md).

## S4 Closure Record

Delivered independent frontend; [S4 evidence](../etc/evidence/m0-t423-s3-hidden-console-ledger.md).

## S3 Closure Record

Preserved and replanned, not functional closure; [S3 handoff](../etc/evidence/m0-t423-s3-hidden-console-ledger.md).

## S2 Closure Record

Delivered copied I/O boundary; [S2 ledger](../etc/evidence/m0-t423-s2-console-boundary-ledger.md).

## S1 Closure Record

Delivered lifecycle baseline; [S1 evidence](../etc/evidence/m0-t423-s1-restart-lifecycle.md).

## Recent M0 Closures

S11 is the latest bounded subtask closure; T423 stays open for owner acceptance.

## Recent Governance

T422 remains owner-closed. T423 cannot close before owner acceptance.
Source/test delivery must preserve unrelated and deferred work.
