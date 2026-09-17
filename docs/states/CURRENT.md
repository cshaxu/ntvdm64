# Project Status

## Current Work

## Active Packet

**Active: M0 T420 S1** — CCPU386 complete-package audit and recovery design.

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T420 S1, Ordinary Mode. |
| Admission And Approval | The owner accepted T419 and directed admission of the next queue-head T package. This admits its first dependency-ordered package unit only. |
| Candidate Proposal | [MVDM runtime package completion](../proposals/proposal-mvdm-runtime-package-completion-001.md). |
| Objective | Produce a complete, source-first recovery design for the selected original `mvdm/softpc.new/base/ccpu386` manifest: direct members, all reached interfaces and globals, CPU/event/exception/interrupt/descriptor lifecycle, finite external owners, duplicate local policy, and a full S2 implementation/verification plan. |
| Non-goals | No source behavior change, x64 profile, CPU30 monitor, trace-led opcode patch, C-VID/system/video/keymouse work, WOW/WRITE recovery, new executor, or broad repository re-audit. |
| Reference Baseline | Formal x86 CCPU40 product is buildable; previous focused CCPU/C-VID restoration and T419 closure records are historical evidence, not package-completion certification. The proposal records zero of 19 formal-linked units currently certified complete. |
| Files And ABI Surface | Read-only audit of `src/mvdm/softpc.new/base/ccpu386`, its original build manifest/headers and direct selected callers; package-owned build graph and existing CCPU fixtures may be inspected. No ABI or production source change is admitted in S1. |
| Applicable Rules | Goal, architecture, coding, execution, source policy and mirror rules. Complete package closure precedes individual symbol repair; original source, then finite same-shaped adapter, then registered exception. |
| Verification | Freeze the actual selected manifest and hashes; inspect every member and direct external interface; compare current source/build selection against the original package; classify every gap and duplicate provider; run only read-only manifest/build-graph consistency checks. |
| Expected Markers | A per-member ledger with no unclassified selected member; each external edge has one original owner or named finite boundary; each lifecycle phase has a positive/negative verification owner; and every proposed S2 change has a source-first rung disposition. |
| Asset Needs | Existing selected OpenNT mirror, current x86 build graph, comparison evidence, and no new source/media acquisition. |
| Reporting Requirements | Record exact inputs, commands, observations, confidence, all exclusions, remaining limitations, prospective diff reduction, and the one-package S2 plan in indexed evidence. |
| Stop Conditions | Any source-dependent package outside the frozen boundary, ambiguity about manifest provenance, an unavailable original dependency requiring a new replacement ABI, or a finding that materially changes package order pauses for owner decision. |
| Exit Criteria | Indexed S1 evidence and an implementation-ready S2 brief account for the complete CCPU386 package with no unknown member/interface/lifecycle item; governance and diff checks pass; then commit and push the design delivery. |
| Original Owner Request | “准入下一个t任务。” |
| Similar-Issue Sweep | Check complete-manifest versus selected-subset mismatches, duplicate current policy, lifecycle omissions, and source-path/build graph drift across all CCPU386 members, not only the last traced instruction. |

## Current Technical Baseline

- Product package: Win32/x86 CCPU40 `run16.exe`, `basesrv.exe`, `ntvdm.exe`,
  and `dtmgr.exe`; latest selected artifacts are in `O:\winnt`.
- T419 is closed at `e769ac104`: four EXE component directories are explicit,
  `dtmgr.exe` replaces `dtaskmgr.exe`, and the clean-Console plus
  COMMAND/MEM/EDIT regressions passed.
- The next package is an audit/design S only. No package is called complete
  merely because it links or has a focused fixture.

## Prior Closure

[T419 Terminal task-manager closure](../history/m0-t419-terminal-task-manager-closure.md)
records the accepted task-manager delivery and its evidence boundary.

## Next Work

The remaining candidate order is authoritative in [Queue](QUEUE.md). T420 S1
must finish its complete-package audit before a CCPU386 recovery implementation
S is admitted.
