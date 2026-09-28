# Project Status

## Current Work

T423 remains open. S11 mouse repair is complete; its validated seven-file
package is published at O:/winnt. Stop here for owner verification.
Do not automatically start S12 or close T423.

**No active M/T/S packet.**

The [T423 proposal](../proposals/proposal-kvm-window-graphics-presentation-001.md)
defines subsequent S12 NTCON native-text backend work and S13 RDP pointer
capture. Native screen continuity and actual-member retirement are transferred
to S12, not claimed as fixed by S11.

## S11 Closure Record

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

## Next Work, Not Active

[S12 migration ledger](../etc/evidence/m0-t423-s12-ntcon-backend.md):
one authenticated NTCON backend per character frontend, real Console state,
native creation/completion, screen handoff, actual membership and retirement,
NTSRV registration and NTMON management. Preserve and reuse current research.
Await owner instructions after S11 verification. S13 owns RDP capture.

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
