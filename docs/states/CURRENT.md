# Project Status

## Current Work

**Active: M0 T406 S1**

M0 T405 S3 and T405 closed by owner direction on 2026-09-12:
static audit completed; restoration and verification transferred to approved
candidates, not declared implemented. See the
[closure and receiver table](../history/m0-t405-source-diff-audit.md).
The [original proposal](../history/m0-t405-source-diff-audit-proposal.md) and
[combined audit](../etc/evidence/opennt-replacement-audit-20260912.md) remain
preserved. The owner admitted the physical-memory mapping package on
2026-09-12; only research/design S1 is active. Report before S2 repair.

S1 findings are ready in the [research/design report](../etc/evidence/m0-t406-s1-physical-mapping-design.md):
224 changed mirror lines in the selected cohort; 159 gross rollback candidates
(128 x86-width cohort, 31 conditional remap protocol), plus 85 gross
adapter/header protocol-retirement lines. These are not tested net savings.
Lower original mapping bodies remain unlocated in searched inputs; no repair
or new runtime acceptance is claimed. Await owner decision before S2.

## Active Packet

### M0 T406 S1 - Original physical mapping research and restoration design

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T406 S1, Ordinary Mode, one agent executing then reviewing. |
| Admission And Approval | Owner: “准入队列第一个T任务，完成彻底调研和设计，并报告可以减少多少不需要的diff；哪些diff和overlay或者自主实现是依然必要的、原始opennt代码无法提供准确内容的；理想情况是完全0自主实现，全面复通原始代码。” |
| Candidate Proposal | [Physical mapping](../proposals/proposal-physical-memory-mapping-restoration-001.md). |
| Objective | Establish U02 original owners, complete selected EMS/DIB/access boundaries and a quantified source-first restoration design, preferring zero autonomous algorithms. |
| Non-goals | No product repair, source import, executable deployment, guest/CPU semantic change, broker or unrelated U03 redesign in S1. |
| Reference Baseline | f2646d80b; T405 audit and current proposal; source/runtime acceptance remain distinct. |
| Files And ABI Surface | Read-only source/build/artifact comparison; Status, Queue, existing proposal and indexed S1 evidence only. No ABI change. |
| Applicable Rules | Repository source, architecture, coding, execution and documentation rules; corresponding governance skills. |
| Verification | Bounded original/current source searches and hashes; per-file numstat and hunk disposition; selected compile macros/callers; governance, links, diff review, commit/push. |
| Expected Markers | Original owner and four-rung disposition per interface; proved versus conditional removal counts; necessary bindings and unresolved original-source gaps; positive/negative S2 test design. |
| Asset Needs | Existing local OpenNT/OpenNT-4.5 originals, selected mirrors and retained build inputs; comparisons read-only. No new source/media. |
| Reporting Requirements | Separate mirror diff, overlay, autonomous code, declarations and test footprint. Report bounded search limits; zero autonomous code is a target, not assumed feasibility. |
| Stop Conditions | Missing provenance or unavailable original implementation is explicit uncertainty; no invented fix. New source/runtime authority or expanded boundary requires review. |
| Exit Criteria | Complete bounded research/design with quantified findings and original-source limitations reported to owner; S2 does not auto-start. |
| Original Owner Request | Full request retained in Admission And Approval above. |
| Similar-Issue Sweep | EMS, external DIB, CPU/SAS byte and bulk/cross-page accesses, backing lifetime and fixture-only publication bypasses. |

## Current Technical Baseline

- Sole product: Win32/x86 ntvdm32.exe with original CCPU40; native x64 and
  CPU30 are not product/build/acceptance targets.
- Audited source: 3d127962c82133cb6adad50f52733250bac5f11a, including
  unaccepted 1daff0ace WIP. T405 changed no product behavior and proves no new
  WRITE execution. [T404 closure](../history/m0-t404-owner-directed-closure.md)
  preserves prior runtime limitations.
- Latest existing deployed executable: O:\ntvdm64\ntvdm32.exe,
  2026-09-12 09:15, 3,232,768 bytes; SHA-256
  27F8D7B5BB074838E9484877954A38DCBCFA0BE64282A4A2D05C776867655107.
  This closure builds, runs and deploys no executable.
- [Queue](QUEUE.md) owns candidate order and pending repairs; [debt](TODO.md)
  retains full WRITE limitations. Ignored builds and runtime logs are preserved,
  not deleted to obtain Git cleanliness. Commit/push and status verification
  follow the closure record; an unavailable push remains pending delivery.

## Recent M0 Closures

| Task | Compact result |
| --- | --- |
| T405 | Static diff/overlay audit complete; 37 overlap rows and eight unresolved families transferred to candidate owners. No restoration/runtime acceptance claim. |
| T404 | Owner-directed closure with unaccepted WIP preserved; mouse acceptance retained, WRITE incomplete. |
| T403 | x64 mapping/overlay residue retired; formal x86 build and owner test passed. |

## Recent Governance

- **M0 Td S10 P1:** 8f7f6ec3d assigned U03-U08 owners and acceptance in existing proposals; pushed.
- **M0 Td S9 P1:** de5c4f969 placed physical mapping first and CCPU40 event recovery before broker.
- **M0 Td S8 P1:** recorded DPMI/XMS memory and transition restoration.
- **M0 Td S7 P1:** consolidated RTL, Console and WOW restoration with broker-before-WOW sequencing.

## Historical Status Ledger

The pre-compaction ledger remains in the
[archived status snapshot](../../artifacts/documentation-archive/20260910/etc/evidence/status-ledgers/status-ledger-snapshot-20260910.md).
It is historical evidence, not current authority. Closed-task detail belongs
in history; candidate plans remain in their linked proposals.
