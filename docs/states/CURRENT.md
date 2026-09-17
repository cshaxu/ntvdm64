# Project Status

## Current Work

## Active Packet

**Active: M0 T420 S5**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T420 S5 — Ordinary Mode. |
| Candidate Proposal | [MVDM runtime package completion](../proposals/proposal-mvdm-runtime-package-completion-001.md) |
| Admission And Approval | The owner approved on 2026-09-17: “批准所有推送 请你自动准入每个s任务”. This reopens T420 for the next sequential package and authorizes later sequential S admissions and pushes. |
| Objective | Complete the selected original `softpc.new/base/keymouse` package: keyboard controller, mouse controller, original callbacks, public-Console binding and teardown lifecycle, retaining original source behavior through only finite unavailable host bindings. |
| Non-goals | No CPU30 monitor, replacement keyboard/mouse controller, renderer change, WOW expansion or new mirror files. |
| Reference Baseline | T420 S1--S4 are closed; current `O:\winnt` four-EXE x86 package passed direct/interactive COMMAND/MEM/EDIT and video geometry regression. |
| Files And ABI Surface | Freeze the complete selected `src/mvdm/softpc.new/base/keymouse` manifest and every reached Console/8042/INT 33h boundary. No new file below mirrors. |
| Applicable Rules | Project design/rules, source policy, strict mirror rule and source-recovery ladder. |
| Verification | Freeze manifest/source-diff ledger; audit every reached external function/global/callback; fresh x86 formal build; focused controller/callback positive and negative checks; direct and interactive COMMAND/MEM/EDIT regressions; governance and diff review. |
| Expected Markers | Complete selected keymouse disposition; original Console→8042/INT 33h ordering and teardown evidence; retained finite host boundary ledger; separated mirror/adapter/autonomous footprint report. |
| Asset Needs | Existing selected OpenNT MVDM union, current formal generator and `O:\winnt` runtime package. No new historical source or guest media import is admitted. |
| Reporting Requirements | Report each manifest source, external boundary, source-first recovery result, exact footprint change, focused and established-product results, limitations and next owner. |
| Stop Conditions | Pause for a required package beyond `keymouse`, a kernel/CPU30 dependency, a new behavior interface, or COMMAND/MEM/EDIT regression. |
| Exit Criteria | All selected keymouse units and reached interfaces have dispositions; any change is source-first/mirror-compliant; formal x86/focused tests and established regressions pass; evidence/index/status updated; reviewed P pushed. |
| Original Owner Request | “批准所有推送 请你自动准入每个s任务”. |
| Similar-Issue Sweep | Compare every selected system source to OpenNT; audit all divergence markers and current timer/PIC/quick-event consumers for duplicate local policy. |

## S1 Closure Record

[T420 S1 CCPU386 package audit](../etc/evidence/m0-t420-s1-ccpu386-package-audit.md)
was delivered at `2d128cbaa`. It freezes the complete CCPU manifest and its
C-VID state boundary; S2 owns that next package without reopening CCPU
semantics.

## S2 Closure Record

[T420 S2 C-VID package recovery](../etc/evidence/m0-t420-s2-cvidc-package-recovery.md)
closed at the T419-integrated formal product revision. It records all 39
selected C-VID sources, one `Gdp` allocation owner, the profile-null vector
dispositions, focused vector/map proof, the final four-executable x86 build,
and deployed `COMMAND/MEM/EDIT` regression. T420 may next admit S3 (`video`)
only on a new owner request.

## S3 Closure Record

[T420 S3 video package recovery](../etc/evidence/m0-t420-s3-video-package-recovery.md)
closes all 19 selected original video sources. It records the retained CCPU40
and C-VID ABI declarations, the finite worker-local public-Console boundary,
fresh four-program x86 linkage, C-VID contract fixture, six Console geometry
observations and direct/interactive COMMAND/MEM/EDIT regression.

## S4 Closure Record

[T420 S4 system package recovery](../etc/evidence/m0-t420-s4-system-package-recovery.md)
closes the eleven-unit system manifest: 41 mirror-side diagnostic lines were
removed, nine units now match the pinned OpenNT source exactly, and the two
remaining source-first boundaries are the relocated public-header include and
the original ROM-residency body selection. Fresh x86 product linkage,
VdmTib storage audit, HALT/RESET fixture, and deployed COMMAND/MEM/EDIT plus
Console geometry regression passed.

## Current Technical Baseline

- Product package: Win32/x86 CCPU40 `run16.exe`, `basesrv.exe`, `ntvdm.exe`,
  and `dtmgr.exe`; latest selected artifacts are in `O:\winnt`.
- T419 is closed with its owner-accepted post-closure DTMgr record-projection
  correction: four EXE component directories are explicit, `dtmgr.exe`
  replaces `dtaskmgr.exe`, and the task display derives depth from original
  BaseSrv DOS records rather than a parallel stack.
- T420 S1--S4 are delivered: CCPU386, C-VID, video and system are closed
  complete package units. S5 keymouse is auto-admitted by the owner's
  standing sequential-S authority.

## Prior Closure

[T419 Terminal task-manager closure](../history/m0-t419-terminal-task-manager-closure.md)
records the accepted task-manager delivery and its evidence boundary.

## Recent M0 Closures

| T419 | [Terminal task manager](../history/m0-t419-terminal-task-manager-closure.md) | Delivered `dtmgr.exe`, authenticated selected-worker management, compact first-option compatibility, and explicit executable component roots. |

## Recent Governance

No standalone governance delivery is active or pending.

## Next Work

The remaining candidate order is authoritative in [Queue](QUEUE.md); no
numeric packet is active.
