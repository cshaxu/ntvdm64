# Project Status

## Current Work

**Active: M0 T406 S2**

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

Owner approved upstream observation: “批准执行” for EMS configuration,
initialization and service ingress; WRITE's earliest exit and WOW reachability;
then a real guest EMS allocation/map/switch/free workload using an isolated PIF
if default EMS is disabled. S1 may add minimal registered observational hooks
and test media, with build root build/M0-T406/S1/r003-upstream-observation.
Keep normal CLI, defaults and mapping behavior unchanged. Report the first
blocked edge and restoration design before functional repair; no CPU change.
The prior P4 push remains pending destination approval after safety rejection.

## S1 Closure Record

[S1 research and observation](../etc/evidence/m0-t406-s1-physical-mapping-design.md)
is delivered through pushed a03a6b4e3. Owner now approves restoration. This
closes bounded research/observation, not the T or its unresolved contracts;
remaining width classification and mapping proofs transfer explicitly to S2.

## S2 Approval And Group Gates

Owner: “批准修复为opennt原始实现  并测试通过”, followed by
“你把可以恢复的原始代码分组，每次恢复一组便跑一次编译测试 不要划分太细 要有效率”.
The preceding S1 paragraphs are historical scope; this approval supersedes their
research-only restrictions. S1's bounded findings and P4/P5 are delivered at
pushed a03a6b4e3; wider per-hunk review continues as S2 implementation work.
Only S2 is active. The one agent implements, then switches to reviewer role.

Groups: (1) paired original x86-width declarations/definitions/callers, retaining
necessary Win32 ABI repairs; (2) original external mapping add/alignment/remove
contract and retirement of extra publication protocol; (3) source-proven remaining
SAS/bulk/cross-page consistency. Each group builds and tests before the next.
No lower function is labelled original unless its original body is identified.
Build root: build/M0-T406/S2/r001-original-restoration. Real runtime logs remain
under O:\ntvdm64\logs\. Frozen reference is a03a6b4e3 and its S1 EMS/WRITE traces.
Original source stays in its mirror; only unavailable lower mechanics stay in
an explicitly reviewed adapter. Mapping group tests must use original-shaped
callers without fixture-only preregistration, verify shared bytes and backing
restoration, and retain positive EMS guest coverage. Width group tests include
paired source checks, x86 compile/link and ordinary DOS/WOW-frontier regression.
This admission does not declare S1's unverified candidates already correct.

## Active Packet

Group 2 implemented and verified: original add/remove function bodies match
OpenNT; extra publication protocol removed. Real native DIB/CCPU shared
storage, alignment, allocation failure and ordinary-backing restoration
tests pass, as does the EMS guest regression. See
[P2 evidence](../etc/evidence/m0-t406-s2-original-restoration.md).
Latest published EXE: 3,235,328 bytes; SHA-256
2f4d823e1ef823c7f1253c3fd2b3de09b240359dcd364ad11b79126737389587.
Earlier artifact identities below are historical. Group 3 and remaining
width disposition are still open; full WRITE is not accepted.

Group 2 working root: build/M0-T406/S2/r002-dib-restoration. Approved lower
binding design: allocate the finite per-session physical-page translation
storage during InitIntelMemory, before publishing successful initialization.
VdmSetPhysRecStructs then performs no allocation, matching its original void
call contract. Restore original VdmAddVirtualMemory/VdmRemoveVirtualMemory
bodies and remove publish/prepare/cancel. This lower table is explicitly an
unavailable-implementation adapter, not an imported original function body.

Group 2 working root: build/M0-T406/S2/r002-dib-restoration. Approved lower
binding design: allocate the finite per-session physical-page translation
storage during InitIntelMemory, before publishing successful initialization.
VdmSetPhysRecStructs then performs no allocation, matching its original void
call contract. Restore original VdmAddVirtualMemory/VdmRemoveVirtualMemory
bodies and remove publish/prepare/cancel. This lower table is explicitly an
unavailable-implementation adapter, not an imported original function body.

S2 progress: the first 14-file width cohort and original external-memory
add/remove contract are restored. [Group evidence](../etc/evidence/m0-t406-s2-original-restoration.md)
records formal builds, tests and limits. Further width/access work remains
open; the current deployed identity is in Current Technical Baseline below.

### M0 T406 S2 - Original mapping and x86-width restoration

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T406 S2, Ordinary Mode, one agent executing then reviewing. |
| Admission And Approval | Owner: “准入队列第一个T任务，完成彻底调研和设计，并报告可以减少多少不需要的diff；哪些diff和overlay或者自主实现是依然必要的、原始opennt代码无法提供准确内容的；理想情况是完全0自主实现，全面复通原始代码。” |
| Candidate Proposal | [Physical mapping](../proposals/proposal-physical-memory-mapping-restoration-001.md). |
| Objective | Implement approved original mapping contracts and grouped x86-width restoration; verify EMS/DIB/access boundaries and report retained lower mechanics honestly. |
| Non-goals | No CPU instruction changes, kernel/CSR shell, broker or unrelated U03 redesign. No claim of full WRITE acceptance. |
| Reference Baseline | f2646d80b; T405 audit and current proposal; source/runtime acceptance remain distinct. |
| Files And ABI Surface | Physical mapping adapter observation and focused tests; Status and indexed S1 evidence. No public ABI, mapping-state or return-contract change. |
| Applicable Rules | Repository source, architecture, coding, execution and documentation rules; corresponding governance skills. |
| Verification | Bounded original/current source searches and hashes; per-file numstat and hunk disposition; selected compile macros/callers; governance, links, diff review, commit/push. |
| Expected Markers | Original owner and four-rung disposition per interface; proved versus conditional removal counts; necessary bindings and unresolved original-source gaps; positive/negative S2 test design. |
| Asset Needs | Existing local OpenNT/OpenNT-4.5 originals, selected mirrors and retained build inputs; comparisons read-only. No new source/media. |
| Reporting Requirements | Separate mirror diff, overlay, autonomous code, declarations and test footprint. Report bounded search limits; zero autonomous code is a target, not assumed feasibility. |
| Stop Conditions | Missing provenance or unavailable original implementation is explicit uncertainty; no invented fix. New source/runtime authority or expanded boundary requires review. |
| Exit Criteria | Restore the approved source contracts and x86-width cohorts; pass group-level formal build, focused tests and integration; review, commit and push. Unknown original lower bodies remain explicit. |
| Original Owner Request | Full request retained in Admission And Approval above. |
| Similar-Issue Sweep | EMS, external DIB, CPU/SAS byte and bulk/cross-page accesses, backing lifetime and fixture-only publication bypasses. |

## Current Technical Baseline

- Sole product: Win32/x86 ntvdm32.exe with original CCPU40; native x64 and
  CPU30 are not product/build/acceptance targets.
- Audited source: 3d127962c82133cb6adad50f52733250bac5f11a, including
  unaccepted 1daff0ace WIP. T405 changed no product behavior and proves no new
  WRITE execution. [T404 closure](../history/m0-t404-owner-directed-closure.md)
  preserves prior runtime limitations.
- Latest deployed executable: O:\ntvdm64\ntvdm32.exe, T406 S2 P2 external
  memory restoration plus working EMS/allocator/host-word cohorts, 3,235,328 bytes;
  SHA-256 2fba77c21709104319017fb239b5b020597ea654daf6927f670faa754f013ce6.
  Formal build, native DIB sharing and isolated EMS guest tests pass. MEM and
  COMMAND exit normally; EDIT reaches its editor after Esc and bounded cleanup.
  WRITE still reaches W32Init FALSE, exit 255; no full WRITE acceptance.
  [S2 evidence](../etc/evidence/m0-t406-s2-original-restoration.md) owns details.
- Extended EMS conventional-window regression fails with c0000005 in host
  copying; previous logical-page EMS passes do not cover it. Native alias
  experiments pass but expose incompatible decommit semantics. The proposed
  alternative access binding awaits owner approval; no new mapping repair
  is implemented. Approved width-only restoration continues independently.
- S1, S2 P1/P2 and the grouped restoration at 78d492964 are pushed to main.
  The owner explicitly approved permanent main pushes. S2 remains active for the
  remaining width cohorts and access-boundary audit. The latest test-only
  extension verifies original SAS loads/stores/move across reversed pages;
  it does not yet prove EMS bulk-copy caller contracts.
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
