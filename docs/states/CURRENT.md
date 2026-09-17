# Project Status

## Current Work

## Active Packet

**Active: M0 T420 S2** — C-VID complete-package recovery.

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T420 S2, Ordinary Mode. |
| Admission And Approval | T420 S1 reached its bounded research delivery at `2d128cbaa`; the already-approved dependency order now admits the next package unit. |
| Candidate Proposal | [MVDM runtime package completion](../proposals/proposal-mvdm-runtime-package-completion-001.md). |
| Objective | Recover the complete selected original `mvdm/softpc.new/base/cvidc` package: one state/vector provider per reached slot, source-shaped generated-table composition, original early/complete publication ordering, reset and teardown rules, and removal or minimization of duplicate binding policy. |
| Non-goals | No CPU30 monitor, x64/HAXM profile, renderer/video-package rewrite, speculative WOW/WRITE repair, generic state manager, or CCPU event/DPMI/host-SAS ownership change. |
| Reference Baseline | S1 proved the CCPU package's only shared-state boundary is C-VID, and recorded the 130-member CCPU selection plus its finite C-VID dependency in [S1 evidence](../etc/evidence/m0-t420-s1-ccpu386-package-audit.md). Earlier T411 vector work is evidence, not whole-package certification. |
| Files And ABI Surface | `src/mvdm/softpc.new/base/cvidc`, its original manifest/generated inputs and direct CCPU consumers; bounded binding code under `src/ntvdm-exe/softpc` may change only after original-generated-source disposition. No unrelated package source is admitted. |
| Applicable Rules | Goal, architecture, coding, execution, source policy and mirror rules. Original generated carrier first; then smallest typed binding seam outside mirror; then a registered exception. |
| Verification | Freeze every selected C-VID source and generated input; map every public/private CPU, SAS, Video and GDP state/slot to one provider; prove early bind, full bind, repeat bind, reset and teardown; run focused positive/negative vector checks, formal x86 build, and direct plus interactive COMMAND/MEM/EDIT regressions. |
| Expected Markers | No selected C-VID member or reached vector slot lacks a provider/disposition; no duplicate common/tentative state substitutes for ownership; each lifecycle phase has a source owner and verification; changed binding code has an original-generated-source rung ledger. |
| Asset Needs | Existing selected OpenNT mirror, C-VID generated headers/tables, current formal x86 graph/map, existing C-VID fixture and no new source/media acquisition. |
| Reporting Requirements | Record exact manifests/hashes, every provider and state owner, all removed/retained mirror and adapter lines, commands/results, confidence, exclusions and limitations in indexed evidence. |
| Stop Conditions | Missing/ambiguous generated-source provenance, a required provider outside the frozen package without a finite existing boundary, an unavailable original carrier requiring a new ABI, or material package-order change pauses for owner decision. |
| Exit Criteria | Complete indexed C-VID ledger and source-first implementation disposition; focused positive/negative vector proof, fresh formal x86 build and required product regressions pass; governance/diff review, commit and push complete the S delivery. |
| Original Owner Request | “准入下一个t任务。” |
| Similar-Issue Sweep | Check every generated/public/private slot, state declaration, early/full bind and reset/teardown path; sweep CCPU and video consumers for the same duplicate or self-rebinding pattern. |

## S1 Closure Record

[T420 S1 CCPU386 package audit](../etc/evidence/m0-t420-s1-ccpu386-package-audit.md)
was delivered at `2d128cbaa`. It freezes the complete CCPU manifest and its
C-VID state boundary; S2 owns that next package without reopening CCPU
semantics.

## Current Technical Baseline

- Product package: Win32/x86 CCPU40 `run16.exe`, `basesrv.exe`, `ntvdm.exe`,
  and `dtmgr.exe`; latest selected artifacts are in `O:\winnt`.
- T419 is closed at `e769ac104`: four EXE component directories are explicit,
  `dtmgr.exe` replaces `dtaskmgr.exe`, and the clean-Console plus
  COMMAND/MEM/EDIT regressions passed.
- T420 S1 is delivered at `2d128cbaa`; its evidence freezes all 130 selected
  CCPU compile nodes, the `ntstubs`/`localfm` profile split, the 120/10
  final-map extraction split and finite external owners. S2 now audits and
  recovers its only shared-state boundary, C-VID; no package is called
  complete merely because it links or has a focused fixture.

## Prior Closure

[T419 Terminal task-manager closure](../history/m0-t419-terminal-task-manager-closure.md)
records the accepted task-manager delivery and its evidence boundary.

## Recent M0 Closures

| T419 | [Terminal task manager](../history/m0-t419-terminal-task-manager-closure.md) | Delivered `dtmgr.exe`, authenticated selected-worker management, compact first-option compatibility, and explicit executable component roots. |

## Recent Governance

No standalone governance delivery is active or pending.

## Next Work

The remaining candidate order is authoritative in [Queue](QUEUE.md). T420 S2
must finish C-VID package recovery before the following original package S is
admitted.
