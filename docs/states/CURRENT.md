# Project Status

## Current Work

**Active: M0 T433 S2** (Ordinary Mode; delivered, awaiting next-stage instruction).

The owner authorizes closing the delivered single-worker/dual-Hook package
without manual acceptance and admitting the next Queue candidate. This is
owner-directed closure on automated evidence, not a claim of hands-on testing.
[T432 closure](../history/m0-t432-single-worker-dual-hook-closure.md) retains
the delivered baseline, superseded research and known limitations.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T433 S2; Ordinary Mode. |
| Candidate Proposal | [Native x64 launcher/frontend/monitor](../proposals/proposal-project-components-native-x64-001.md). |
| Admission And Approval | Owner fixes S2 NTVWM / S3 NTCON / S4 NTMON / S5 RUN16, accepts the presented single-worker, mirror-zero-diff migration design and says “批准开始实施！完成后汇报。” S1 reaches an owner-approved bounded design handoff, not four-component native runtime closure. |
| Objective | Migrate the single ntvwm.exe to AMD64 with architecture-local worker-base/common/RPC/installer dependencies; retain native32/64 targets, existing x86 NTSRV/NTCON/run16, hidden Console, actual handles/waits/results and NTSRV-authorized I/O/lifecycle. Verify, coherently publish, review/commit/push and report. |
| Non-goals | No second NTVWM, no x64 run16/NTCON/NTMON in S2, no x64 NTSRV/NTVDM/WOW32/VDMREDIR; no original mirror/guest rewrite, new protocol/helper/scheduler/registry or target replacement. |
| Reference Baseline | Production b97041e748772dce813fb963e7527781c7321909; documentation31140866907fccd35aaddf12ccca13b8c54a0517. Sealed build/M0-T432/S6/r048-runtime and r029-publication match O:/winnt/system32: ten images, APP0.0.432/RPC41/I/O25. Only Hook64 is AMD64; other images remain x86 /MT CCPU40. |
| Files And ABI Surface | Own ntvwm-exe, selected worker-base/common/native client adaptations, native_launch capability text, isolated native build/packaging/verification tools and tests. Rebuild original RTL error.c unchanged through its existing native adapter. Shared x86 dependents rebuild as needed. Wire remains RPC41/I/O25 unless a real wire change is separately justified. |
| Applicable Rules | AGENTS authorities, source-first provenance, unchanged guest media, mirror-minimality, owner-local resource policy, fixed-width wire contracts and one active S. Preserve other-session changes. |
| Verification | build/M0-T433/S2/r001 onward; fresh AMD64 NTVWM/client/MIDL closure, x86 formal cache and Hook64 affected input rebuild for APP0.0.433. Focused ABI/worker lifetime/Hook/native32/64/context/receipt/failure/reentry/handoff/isolation checks; retained Console17/Window17/WOW frontiers; coherent ten-image manifest/recovery/publication and deployed smoke; governance/link/diff review. |
| Expected Markers | Exactly one AMD64 ntvwm.exe and nthook64.dll; remaining eight images I386. x86 service/frontend/launcher interoperable, actual native32/64 target Hook/results, double-ACK I/O release, no original MVDM/OpenNT-host source diff, tested/deployed hashes identical. |
| Asset Needs | Existing MSVC14.43/SDK22621 native tools, T432 r048 baseline/current x86 graph/maps, audited Hook64/error adapter and immutable guest media. No new external import. |
| Reporting Requirements | Distinguish compile/fixture/real-process proof, published state and remaining limits. Report original mirror diff explicitly. Retain failed evidence and do not call tests or publication complete before actual results. |
| Stop Conditions | Unknown provenance, original mirror rewrite, broad source import, changed CLI/search/native child identity/waits/suspension, unapproved helper/worker/protocol, or unexplained baseline regression; preserve coherent old publication. |
| Exit Criteria | Affected builds and positive/negative/lifecycle/native32/64 handoff checks plus retained product gates pass; coherent ten-image package published and verified; reviewed production diff committed/pushed and worktree clean. Do not auto-admit S3. |
| Original Owner Request | “S2: NTVWM; S3: NTCON; S4: NTMON; S5: RUN16”，then accepts the NTVWM migration design and instructs “批准开始实施！完成后汇报。” |
| Similar-Issue Sweep | Native resource widths/text locators, cross-width incoming/outgoing attachments, Hook32/64/context-only x86 run16, HWND/COORD/frame structs, MIDL native ABI, System32/Sysnative selection, publisher/input waits, final I/O, nested parent restoration, broker/frontend death and exact cleanup. |

## Plan and retained boundaries

[Native component migration plan](../etc/operations/t433-native-components-x64-plan.md)
owns S1 audit/design, then the owner's order: S2 NTVWM, S3 NTCON, S4 NTMON,
S5 RUN16, followed by S6 integrated delivery. S2 alone is admitted;
S3 and later remain planned. Each stage retains its verification/publication gate.

NTSRV remains the sole lifecycle/task/I/O connection authority. NTCON stays
worker-neutral, holding zero or one authorized direct I/O pipe. NTVDM keeps
the original x86 CCPU40/DOS/WOW execution boundary; NTVWM remains one worker
executing both native target widths, with its x64 migration added by owner.
Hook32/64 are retained. The intended final package still has ten images:
four migrated EXEs plus Hook64 are AMD64, the remaining five images are x86.
Current published NTVWM is AMD64. Names/system32-relative paths stay.

Each shared dependency is compiled for its actual consumer ABI; no mixed
object/library architecture or CRT in one image. Copied wire records retain
fixed-width types and authenticated recipient-local resource attachments.
No global WOW64-redirection disable, executable-directory search priority,
launcher syntax change, bitness-only worker selection or private transition.

## Current Technical Baseline

T432 is closed by owner direction without hands-on acceptance. Its ten-image
package remains independently recoverable. S2's verified r010-runtime is now
published at O:/winnt/system32, identity0.0.433/RPC41/I/O25. NTVWM and Hook64
are AMD64; the other eight images stay I386. Guest/configuration are unchanged.

[Final S6 evidence](../etc/evidence/m0-t432-s6-single-worker-reconstruction.md)
retains Hook147/147, metadata402, RPC220, native lifetime1084, service29,
Console17/Window17, independent WOW frontiers, same-worker cross-width reentry,
both-way DOS/native handoff, scoped version negatives and deployed DOS/32/64
smoke. Full-run failed observations remain recorded; corrected Control gates
and unchanged-image product gates are separate results. WINMINE visible UI is
not gameplay; SOL/WRITE retain known error frontiers. Default private-desktop
geometry and arbitrary large-environment behavior are not claimed repaired.

Formal x86 reusable cache: build/M0-T427/S2/r001. Hook64 cache:
build/M0-T432/S6/r002-hook64. Keep these as evidenced inputs, not architecture-
mixed output roots. Superseded dual-worker candidate remains archive-only at
evidence/t432-dual-worker-superseded-20261005; it is not a migration baseline.

## Recent M0 Closures

| Task | Outcome and evidence |
| --- | --- |
| T432 | Single x86 NTVWM, dual Hook32/64, native cross-width execution and NTSRV I/O release authority delivered; owner closes without manual testing. [Closure](../history/m0-t432-single-worker-dual-hook-closure.md). |
| T431 | Owner-accepted Hook32 baseline, preserved independently. [Closure](../history/m0-t431-native-hook32-closure.md). |
| T430 | Accepted non-WOW contract repairs/proofs. [Closure](../history/m0-t430-non-wow-contract-closure.md). |
| T429 | Accepted performance/shared worker I/O. [Closure](../history/m0-t429-performance-worker-io-closure.md). |

## S1 Closure Record

Owner accepts the presented NTVWM design and authorizes implementation.
[Initial four-consumer ledger](../etc/evidence/m0-t433-s1-native-width-audit.md)
and [migration plan](../etc/operations/t433-native-components-x64-plan.md)
supply the bounded design handoff: actual image-matched dependencies, native
resource/wire separation, one worker and original-error source reuse.
No four-component native compile/runtime closure is claimed. Remaining
run16 local capture/RTL environment and frontend/monitor native probes are
explicit obligations of their later component stages, not silently passed.
S1's design checkpoint9c0301ad7 and sequencing d9f8ba065 are pushed. The
earlier remote500 failure is resolved without a force push.

## Recent Governance

S2 implementation is closed at production P1
14796f8ba56a51075f098a532d251531a9937db2, pushed to main with clean worktree.
This documentation-only P2 records the observed delivery; no S3 admission.

[S2 evidence](../etc/evidence/m0-t433-s2-native-worker-migration.md) records
the isolated AMD64 build, RPC220/lifetime1084 unit passes and actual native32
and64 projection. A proven requester/worker file-view gap is corrected by
final DOS/UNC file identity, preserving command text/search; metadata424 and
client receipt/I/O negatives pass. r009's strict CMD version marker fails
with extended-path resource lookup; final short DOS/UNC spelling and the
owned Z: fixture pass the unchanged assertion. r013 Full completes all14 groups;
r015/16/17 prove both-width legacy/Win16 routes, actual same-AMD64-carrier
reentry and GUI management. Current Hook147/147 and final four build-input
negatives pass. r018 publishes r010 and r019 deployed DOS/32/64 CMD VER/output/
receipt smoke passes with matching hashes. Official build/import has no x86
worker fallback and regenerates the same tested bytes. Review/commit/push
are complete; S2 is delivered. S3 is not admitted and T433 remains open.
