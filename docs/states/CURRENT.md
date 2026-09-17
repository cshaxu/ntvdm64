# Project Status

## Current Work

## Active Packet

**Active: M0 T420 S16**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T420 S16 — Ordinary Mode. |
| Candidate Proposal | [MVDM runtime package completion](../proposals/proposal-mvdm-runtime-package-completion-001.md) |
| Admission And Approval | The owner approved on 2026-09-17: “批准所有推送 请你自动准入每个s任务”. S15 closes in this packet; this admits the next sequential package and retains standing push authority. |
| Objective | Complete the original `suballoc` state, allocation and release lifecycle as one source-first package unit. |
| Non-goals | No CPU30 monitor, guest-media patching, new mirror files, DPMI expansion, renderer redesign, or removal of a demonstrated standalone ABI/guest-memory boundary. |
| Reference Baseline | T420 S1--S15 are closed. The formal x86 package passed direct and interactive COMMAND/MEM/EDIT, Console geometry, mouse and short-window EDIT→MEM regression. |
| Files And ABI Surface | Freeze the complete selected `src/mvdm/suballoc` manifest and all reached suballocator state, allocation and release interfaces. No new file below mirrors. |
| Applicable Rules | Project design/rules, source policy, strict mirror rule and source-recovery ladder. |
| Verification | Freeze manifest/source-diff ledger; compare selected mirror files bytewise and normalized; classify every suballocator state/allocation/release interface; fresh x86 formal build; focused suballocator tests; direct and interactive COMMAND/MEM/EDIT regressions; governance and diff review. |
| Expected Markers | Complete selected suballocator disposition; original allocation and release ordering; source-first external-boundary ledger; separated mirror/adapter/autonomous footprint report. |
| Asset Needs | Existing selected OpenNT MVDM union, formal generator and `O:\winnt` runtime package. No new historical source or guest media import is admitted. |
| Reporting Requirements | Report each manifest source, external boundary, source-first result, exact footprint change, focused and established-product results, limitations and next owner. |
| Stop Conditions | Pause for a required package beyond suballoc, a kernel/CPU30 dependency, a new behavior interface, or COMMAND/MEM/EDIT regression. |
| Exit Criteria | All selected suballocator units and reached interfaces have dispositions; changes are source-first/mirror-compliant; formal x86/focused tests and established regressions pass; evidence/index/status updated; reviewed P pushed. |
| Original Owner Request | “批准所有推送 请你自动准入每个s任务”；“按照最小化diff且不改变功能语义和编译成功的原则来做，所有已经提交的S任务如果不符合这个标准，应该继续减少diff而不是继续走下一个diff。” |
| Similar-Issue Sweep | Compare every selected suballocator source to OpenNT; audit all divergence markers and reached state/allocation/release consumers for duplicate local policy. |

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

## S5 Closure Record

[T420 S5 keymouse package recovery](../etc/evidence/m0-t420-s5-keymouse-package-recovery.md)
closes all five selected keyboard/mouse units.  It removes 86 mirror-side
diagnostic lines while retaining and proving the only three finite source-first
CPU40/guest-memory boundaries.  Fresh x86 linkage, deployed COMMAND/MEM/EDIT,
and the Console mouse/geometry plus five short-window EDIT→MEM matrix passed.

## S6 Closure Record

[T420 S6 BIOS package recovery](../etc/evidence/m0-t420-s6-bios-package-recovery.md)
closes all fourteen selected BIOS units.  It removes 13 EMS-only mirror
observation lines, leaving ten selected units byte-identical to pinned OpenNT
and three finite source-first declaration/CPU40-continuation boundaries.
Fresh x86 linkage, VdmTib ownership audit, deployed direct/interactive
COMMAND/MEM/EDIT, six Console geometry modes, mouse move/click/release and
five short-window EDIT→MEM runs passed.

## S7 Closure Record

[T420 S7 support package recovery](../etc/evidence/m0-t420-s7-support-package-recovery.md)
closes all six selected support units.  It removes 13 phase-only mirror
observations; four units remain byte-identical to pinned OpenNT, while the
two finite remaining source-profile boundaries retain CCPU40 single-state
ownership and original C-VID/CPU startup ordering.  Fresh x86 linkage,
focused CCPU/C-VID contracts, deployed COMMAND/MEM/EDIT, Console geometry,
mouse and five short-window EDIT→MEM runs passed.

## S8 Closure Record

[T420 S8 disks package recovery](../etc/evidence/m0-t420-s8-disks-package-recovery.md)
closes all ten selected disks units as byte-exact pinned OpenNT. Fresh x86
linkage, deployed direct/interactive COMMAND/MEM/EDIT, Console geometry,
mouse and five short-window EDIT→MEM runs passed. The preceding S8 P restored
all proven S1--S8 format-only drift and made that audit mandatory for later S
closures.

## S9 Closure Record

[T420 S9 comms package recovery](../etc/evidence/m0-t420-s9-comms-package-recovery.md)
closes all five selected communications units as byte-exact pinned OpenNT.
Fresh x86 linkage, deployed direct/interactive COMMAND/MEM/EDIT, Console
geometry, mouse and five short-window EDIT→MEM runs passed.

## S10 Closure Record

[T420 S10 DOS package recovery](../etc/evidence/m0-t420-s10-dos-package-recovery.md)
closes the one-unit DOS manifest. It restores its pinned OpenNT line endings,
retains only the allocator declaration and original CCPU40 LIM-writeback
selection, and records direct x86 compilation plus freshly linked four-EXE
and deployed COMMAND/Console regression.

## S11 Closure Record

[T420 S11 debug package recovery](../etc/evidence/m0-t420-s11-debug-package-recovery.md)
closes all eight selected original SoftPC debug units, restores `trace.c` to
upstream formatting, and records x86 compilation, fresh product linkage and
deployed regression.

## S12 Closure Record

[T420 S12 host package recovery](../etc/evidence/m0-t420-s12-host-package-recovery.md)
closes the selected original host package: six source files are restored to
pinned OpenNT bytes, `stubs.c` retains only its established VDM_TIB storage
binding, and the owner-supplied BaseSrv grace constant is included in the
fresh x86 four-program and deployed regression evidence.

## S13 Closure Record

[T420 S13 DEM package recovery](../etc/evidence/m0-t420-s13-dem-and-residual-diff-recovery.md)
closes the DEM package and its source-first residual boundary ledger.

## S14 Closure Record

[T420 S14 COMMAND package recovery](../etc/evidence/m0-t420-s14-command-package-recovery.md)
closes COMMAND as a source-first package.  It removes both discovered empty
x86 pointer/handle carriers, preserves the necessary guest-memory, async and
cross-process resource bindings, passes fresh formal x86 linkage and the
owner-accepted direct/interactive COMMAND, MEM and EDIT route.

## S15 Closure Record

[T420 S15 XMS package recovery](../etc/evidence/m0-t420-s15-xms-package-recovery.md)
closes all eight selected XMS units. Six now match the pinned OpenNT sources;
the two remaining deltas are the source-proven `xmsMoveMemory` callback ABI
and a bounded CPU40 guest-memory lease in the original move-block path.
Focused allocation/mapping tests, fresh x86 linkage, and deployed direct
`MEM` plus `COMMAND /C MEM` passed.

## Current Technical Baseline

- Product package: Win32/x86 CCPU40 `run16.exe`, `basesrv.exe`, `ntvdm.exe`,
  and `dtmgr.exe`; latest selected artifacts are in `O:\winnt`.
- T419 is closed with its owner-accepted post-closure DTMgr record-projection
  correction: four EXE component directories are explicit, `dtmgr.exe`
  replaces `dtaskmgr.exe`, and the task display derives depth from original
  BaseSrv DOS records rather than a parallel stack.
- T420 S1--S15 are delivered: CCPU386, C-VID, video, system, keymouse, BIOS,
  support, disks, comms, DOS, debug, host and DEM are closed complete package
  units.  S14 COMMAND restored two empty x86 adapter carriers to their
  original OpenNT forms; S15 closes XMS and S16 suballoc is active.

## Prior Closure

[T419 Terminal task-manager closure](../history/m0-t419-terminal-task-manager-closure.md)
records the accepted task-manager delivery and its evidence boundary.

## Recent M0 Closures

| T419 | [Terminal task manager](../history/m0-t419-terminal-task-manager-closure.md) | Delivered `dtmgr.exe`, authenticated selected-worker management, compact first-option compatibility, and explicit executable component roots. |

## Recent Governance

No standalone governance delivery is active or pending.

## Next Work

M0 T420 S16 is active. The remaining candidate order is authoritative in
[Queue](QUEUE.md) after T420 closure.
