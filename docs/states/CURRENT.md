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
or new runtime acceptance is claimed. S1 follow-up research continues under
the owner amendment below; S2 has not started.

Owner amendment: extend this T to audit and subsequently remove unnecessary
x64-width patterns beyond the U02 files, with paired original/current evidence
and x86 ABI/regression proof. First research the original-vs-local history,
MONITOR/CCPU branch differences, empty-stub significance and COMMAND/EDIT
runtime coverage. This extends S1 research now, not permission to implement
an unreviewed lower mapping substitute. Width-only cleanup stays in this T;
unrelated functional repairs retain their existing candidate owners.

Owner observation amendment (2026-09-12): “在保留它们当前行为的基础上，
加一些logging或者弹窗，这样我跑的时候可以确认它们有在用”. S1 now
includes bounded mapping call/hit logging, focused unchanged-behavior tests,
a fresh x86 formal build at build/M0-T406/S1/r001-mapping-observation and
deployment to the existing runtime EXE. No mapping repair is admitted.
Observation is implemented and verified: fresh x86 link, baseline/observed
adapter tests and denied-log test pass; command.com /c exit returns 0 and
records observer-active only. See the linked S1 evidence P3 section for log
events, limitations and build identity. Actual EDIT/EMS/DIB coverage remains open.
Owner follow-up: “我要的是咱们跑集成测试 比如mem.exe command.com edit.com
还有write.exe 看看是否有过命中”. Run these four deployed ordinary CLI
workloads with bounded real-Console observation, automated welcome dismissal
and command exit, PID-correlated mapping logs and test-owned process cleanup.
Only a test harness may be added under tests/observation; its fresh build root
is build/M0-T406/S1/r002-workload-observation. No product change is needed.
Four-program integration completed and repeated with Job PID tracking: MEM
and COMMAND exit 0; EDIT reaches welcome/editor and accepts Esc before timed
cleanup; WRITE exits 255 before visible UI. All four logs contain observer-active
only, no mapping call/hit. See S1 evidence P4; this does not prove mapping
unnecessary or WRITE/DIB correctness. MEM's anomalous size output is debt.

## Active Packet

### M0 T406 S1 - Original physical mapping research and restoration design

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T406 S1, Ordinary Mode, one agent executing then reviewing. |
| Admission And Approval | Owner: “准入队列第一个T任务，完成彻底调研和设计，并报告可以减少多少不需要的diff；哪些diff和overlay或者自主实现是依然必要的、原始opennt代码无法提供准确内容的；理想情况是完全0自主实现，全面复通原始代码。” |
| Candidate Proposal | [Physical mapping](../proposals/proposal-physical-memory-mapping-restoration-001.md). |
| Objective | Establish U02 original owners, complete selected EMS/DIB/access boundaries and a quantified source-first restoration design, preferring zero autonomous algorithms. |
| Non-goals | No mapping repair, source import, guest/CPU semantic change, broker or unrelated U03 redesign. Diagnostic build/deployment only under the observation amendment. |
| Reference Baseline | f2646d80b; T405 audit and current proposal; source/runtime acceptance remain distinct. |
| Files And ABI Surface | Physical mapping adapter observation and focused tests; Status and indexed S1 evidence. No public ABI, mapping-state or return-contract change. |
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
- Latest deployed executable: O:\ntvdm64\ntvdm32.exe, T406 S1 P3 mapping
  observation, 3,235,328 bytes; SHA-256
  e952e78f1202ed1f95427bd7d6178b722d8b64bf839edf14c5717d489e612e6d.
  Source behavior remains the prior mapping baseline plus bounded observation;
  no new WRITE acceptance. Previous EXE retained in the S1 run root.
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
