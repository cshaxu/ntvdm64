# Project Status

## Current Work

**Active: M0 T410 S3.**

## Active Packet

### M0 T410 S3 — native Console palette and buffer capability recovery

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T410 S3, Ordinary Mode. |
| Admission And Approval | Owner approved continuation: “准入”. This is the remaining D34 slice of the already admitted T410 candidate. |
| Candidate Proposal | [Console input, display and resource restoration](../proposals/proposal-console-input-display-resource-restoration-001.md). |
| Objective | Recover D34's useful original per-buffer palette/state policy through supported native Console capability, or establish the exact unavailable Console-Server boundary and source-shaped caller failure behavior. Retire any independent presentation fallback and avoid changing guest video semantics. |
| Non-goals | No replacement GUI backend, Console Server/CSRSS reconstruction, broker/CLI/DPMI/WOW repair, host mutation, fake-success capability stub, guest video emulation rewrite, or general CPU change. |
| Reference Baseline | S1 retired the isolated app window. S2 completed D09/D24/D25/D28--D30 source/build recovery through P1--P5: pointer/text-cursor separation, returned-input prepend/NOWAIT/NOREMOVE, Alt+Enter pairing, character normalization, and embedded-ROM ownership. S3 P6 restores D34's original graphics-buffer call, removes the unpresented DIB/palette substitute, and restores the original x86 two-byte VDM text-cell layout. Automated real-conhost input/text acceptance and the layout-ownership limitation are recorded in the indexed evidence. |
| Files And ABI Surface | Original SoftPC graphics callers, the private Console palette adapter and presentation capability boundary, focused tests and indexed evidence; public CLI, accepted mouse model and guest video ABI remain unchanged. |
| Applicable Rules | Source policy; goal, architecture, coding, execution and documentation authorities; the source-first historical-recovery gate. |
| Verification | Original/current palette-call and buffer-state ledger; focused capability/failure fixtures; formal x86 build/link; conhost and Terminal observations separately if a reachable graphics workload exists; governance, links and diff review. |
| Expected Markers | Original palette callers retain their order and guest semantics; supported host behavior is bounded and observable, or unsupported behavior fails rather than pretending success; no independent-window fallback. |
| Asset Needs | Existing selected OpenNT mirrors and build graph; local formal x86 toolchain; existing `O:\ntvdm64` package and `O:\ntvdm64\logs\` observation location. No new source or host component. |
| Reporting Requirements | Name original caller, Console Server owner and modern binding; distinguish conhost, Terminal and unavailable graphical capability evidence; report any exact unavailable boundary without treating a fixture as graphics acceptance. |
| Stop Conditions | Required behavior requires Console Server/private GUI mechanics, a proposed repair changes guest palette semantics without source support, or a different provider is required. Pause and name the receiver rather than invent one. |
| Exit Criteria | D34 has original/current disposition, focused positive/negative evidence and an explicit supported or unavailable host capability; no independent window route is restored. T410 then reconciles all eight rows and either closes or records a source-proven receiver. |
| Original Owner Request | “准入”. |
| Similar-Issue Sweep | `SetConsolePalette`, palette allocation/realization, active/output buffer identity, presentation snapshots/events, buffer close/reset and any private-window fallback. |

## M0 T410 S1 Closure Record

S1 removed 1,033 lines of isolated project-owned window/input-presentation
code and stale test/build wiring, without adding a replacement GUI. The final
x86 product linked and was staged at `O:\ntvdm64\ntvdm32.exe` (3,229,696
bytes, SHA-256 `c0771b414f93b1f1644765447f89439860e23e6f9fb67a1319222955b267652f`).
The native text Console path remains selected. The automated disposable-stage
COMMAND observations stopped at exit `0x40` before guest input, so no new
runtime keyboard/mouse acceptance is claimed. Full source/build and runtime
evidence is [recorded here](../etc/evidence/m0-t410-s1-native-console-window-retirement.md).

## T406 Closure Record

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
contract and retirement of extra publication protocol; (3) approved minimal
guest-memory lease access binding for EMS cross-window spans, then source-proven
remaining SAS/bulk/cross-page consistency. Each group builds and tests before the next.
No lower function is labelled original unless its original body is identified.
Build root: build/M0-T406/S2/r001-original-restoration. Real runtime logs remain
under O:\ntvdm64\logs\. Frozen reference is a03a6b4e3 and its S1 EMS/WRITE traces.
Original source stays in its mirror; only unavailable lower mechanics stay in
an explicitly reviewed adapter. Mapping group tests must use original-shaped
callers without fixture-only preregistration, verify shared bytes and backing
restoration, and retain positive EMS guest coverage. Width group tests include
paired source checks, x86 compile/link and ordinary DOS/WOW-frontier regression.
This admission does not declare S1's unverified candidates already correct.

## Historical T406 Packet

Group 2 implemented and verified: original add/remove function bodies match
OpenNT; extra publication protocol removed. Real native DIB/CCPU shared
storage, alignment, allocation failure and ordinary-backing restoration
tests pass, as does the EMS guest regression. See
[P2 evidence](../etc/evidence/m0-t406-s2-original-restoration.md).
Latest published EXE: 3,236,864 bytes; SHA-256
653ec3875887ed10a4d24ef7e264560d986c30c458d6b77648f0a9ce765a9aac.
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

T406 closure: paired original x86-width cohorts and the external-memory
add/remove contract are restored; final candidate disposition and retained
access limitations are in the [closure evidence](../etc/evidence/m0-t406-s2-original-restoration.md#s2-final-x86-width-and-access-boundary-disposition).
No further T406 implementation is active.

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

Owner verification amendment: every T406 semantic binding retained because an
original implementation is unavailable must have its registered divergence,
a bounded first-hit trace at its semantic boundary, and a PID-correlated
real-program run that records whether it hit. A formal fixture proves its
contract but does not substitute for this runtime-hit evidence. No-hit rows
remain explicitly unverified rather than inferred correct from compilation.

## Current Technical Baseline

- Sole product: Win32/x86 ntvdm32.exe with original CCPU40; native x64 and
  CPU30 are not product/build/acceptance targets.
- Audited source: 3d127962c82133cb6adad50f52733250bac5f11a, including
  unaccepted 1daff0ace WIP. T405 changed no product behavior and proves no new
  WRITE execution. [T404 closure](../history/m0-t404-owner-directed-closure.md)
  preserves prior runtime limitations.
- Latest deployed executable: O:\ntvdm64\ntvdm32.exe, T406 S2 external
  memory restoration plus working EMS/allocator/host-word and C-VID x86
  carrier cohorts, original current-process pseudo-handle spelling, restored
  original COMMAND standard-handle carrier, and the approved EMS cross-window
  lease binding and its bounded trace, 3,236,864 bytes; SHA-256
  14e94e2091a9d837f6485cd20a68f6a7c591579bc77891dd91629110d5802a16.
  Formal build, native DIB sharing and isolated EMS guest tests pass. MEM and
  COMMAND exit normally; EDIT reaches its editor after Esc and bounded cleanup.
  WRITE still reaches W32Init FALSE, exit 255; no full WRITE acceptance.
  [S2 evidence](../etc/evidence/m0-t406-s2-original-restoration.md) owns details.
- Extended EMS conventional-window regression formerly failed with c0000005
  in host copying. The owner-approved minimal lease binding now passes both
  direct original `nt_emm` and real INT 67h guest coverage for a reversed
  two-window span. It preserves original EMS dispatch/public functions and
  uses the existing session lease only for alias-overlapping spans; no CPU
  change or generic mapping manager is admitted. Native alias experiments
  remain only evidence because their decommit semantics are incompatible.
- S1, S2 P1/P2 and the grouped restoration at 78d492964 are pushed to main.
  The owner explicitly approved permanent main pushes. T406's final grouped
  width/access disposition is included in the final T406 delivery; the latest EMS repair
  verifies original `nt_emm` bulk-copy callers and real guest INT 67h coverage
  across reversed pages. Full WRITE and unrelated access contracts remain
  assigned to their named future owners, not accepted by this closure.
- [Queue](QUEUE.md) owns candidate order and pending repairs; [debt](TODO.md)
  retains full WRITE limitations. Ignored builds and runtime logs are preserved,
  not deleted to obtain Git cleanliness. Commit/push and status verification
  follow the closure record; an unavailable push remains pending delivery.

## Recent M0 Closures

| Task | Compact result |
| --- | --- |
| T409 | Original DEM/VDD handle lifecycle restored and standalone fast I/O now takes the original guest CF slow-path fallback; formal build and source proofs pass, while the inherited-console MEM watchdog limitation remains explicitly unaccepted. |
| T407 | Original DPMI/XMS and immediate DOSX transition recovery closed; `FSTI` reaches BOP `FDh`, MEM/COMMAND pass, while WRITE remains assigned to WOW lifecycle recovery. |
| T405 | Static diff/overlay audit complete; 37 overlap rows and eight unresolved families transferred to candidate owners. No restoration/runtime acceptance claim. |
| T406 | Original mapping contracts, bounded EMS lease binding and admitted x86-width cohorts restored; final remaining candidates are either registered seams or named future-owner revalidation. |
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
