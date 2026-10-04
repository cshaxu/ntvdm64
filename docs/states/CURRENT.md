# Project Status

## Current Work

**Active: M0 T429 S3** (Ordinary Mode).

Owner admitted the performance package after closing T428. S1's bounded
measurement conclusion is recorded in the
[baseline evidence](../etc/evidence/m0-t429-s1-performance-baseline.md).
S2 is delivered at production123c0ad3e/status651efdd7b. Only S3 is now active,
following the owner's continuous performance-optimization direction.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T429 S3; Ordinary Mode. |
| Admission And Approval | Sequential admission after pushed S2 closure; owner directs continuous execution-performance optimization. Measured frame-publication attribution selects only the proven per-cell host-query cost in the existing NTCON cell importer. |
| Candidate Proposal | [Execution-performance recovery](../proposals/proposal-ntvdm-execution-performance-recovery-001.md). |
| Objective | Query the current Console output code page once per copied text frame instead of once per character; preserve exact cells/styles/cursor, failure rollback and every explicit publication. Measure the actual transfer/commit benefit without changing queues or transport. |
| Non-goals | No CCPU/mirror/guest/firmware, lifecycle, wire, helper/channel/component/scheduler change; no permanent code-page cache, display dedup, input batching or speculative transport optimization. NTVWM30ms sampling is unchanged. |
| Reference Baseline | Production123c0ad3e/status651efdd7b; coherent S2 r025/runtime, deployed and verified r027/r028. MSVC14.43/SDK22621/x86 /MT CCPU40; APP0.0.427/RPC38/I/O25. S2 r023 attributes94.36% of cumulative text commit to39120 per-cell code-page queries. |
| Files And ABI Surface | NTCON frontend_session.c frame conversion; production-linked code-page/style/cursor/failure fixture and reproducible measured comparison; evidence/status. No cross-EXE ABI or ownership change. |
| Applicable Rules | Full AGENTS set, source policy, mirror provenance/minimal registered diff, original instruction semantics, build-only artifacts, preserved other-session edits and serial global endpoint/Z:-only tests. |
| Verification | Incremental affected x86 closure with pinned cache/source/toolchain identities; exact code-page call count and next-frame refresh, paired/triple cells, cursor and conversion-failure rollback; retained frontend/handoff/lifecycle tests; alternating real EDIT200 measured variants and unchanged native control; Console17/Window17/WOW, coherent eight-file publication and smoke. Governance, links and diff review. |
| Expected Markers | One output-CP query per imported frame, no permanent cache; next frame uses changed CP, character/style/cursor state and failure atomicity remain; real EDIT queue/order/receipt assertions pass and transfer/commit distributions improve or contrary evidence is reported. |
| Asset Needs | Validated build/M0-T427/S2/r001 dependency cache; immutable S2 r025 runtime/media and S2 r022 measured frontend/worker; new outputs under build/M0-T429/S3. No external runtime dependency. |
| Reporting Requirements | Separate source/object, measured and physical observations; report hashes, commands, failed attempts, median/tails and retained limits, not inferred speed claims. |
| Stop Conditions | Need for new protocol, ownership/lifecycle policy, mirror/guest changes, event filtering or external acceptance dependency pauses for renewed approval. Unexplained cell/input/handoff regression prevents delivery. |
| Exit Criteria | Production frame importer issues one current CP query per frame with tested exact conversion/rollback; causal measured comparison and retained runtime gates reviewed; coherent eight files published, reviewed commit/push delivered. |
| Original Owner Request | Continue execution-performance optimization using evidence; preserve original execution and explicit frame/input semantics rather than concealing issues with dedup. |
| Similar-Issue Sweep | Per-character host queries in frame import/conversion, CP changes between frames and ownership handoffs, concurrent code-page change within a copied frame, rollback on multibyte conversion failure. |

## S1 Closure Record

Measurement-only S1 concludes with [baseline and control evidence](../etc/evidence/m0-t429-s1-performance-baseline.md), including r020 matched variants.
No production speedup is claimed; physical input and matched SoftPC runtime
comparison remain explicitly unproved. Sequential S2 is admitted, not T closure.

## S2 Closure Record

[S2 diagnostic-selection and close-ordering evidence](../etc/evidence/m0-t429-s2-release-decode-diagnostics.md)
records source/object proof, matched distributions, all retained runtime gates,
the approved native-close dependency and coherent eight-file publication.
Production123c0ad3e is pushed. S2 is bounded-closed, not T429 owner closure.

## Current Technical Baseline

T428 is owner-closed at production68e860553/statusb1b77e129; its
[closure](../history/m0-t428-worker-interface-unification-closure.md)
retains S1-S6 results. The last fully accepted set remains
build/M0-T428/S6/r002/runtime/system32. Owner subsequently requests immediate
S2 candidate publication before full regression. The initial publication matched
build/M0-T429/S2/r002/runtime/system32, with only ntvdm.exe changed; all eight
hashes pass. Recovery is build/M0-T429/S2/r007/recovery. This explicit early
publication is not a reviewed production P or S2 closure.
[S2 evidence](../etc/evidence/m0-t429-s2-release-decode-diagnostics.md)
records passed normal/diagnostic objects, all48 paired comparison cases,
actual EDIT200, service29 and input/video/shutdown fixtures. Console EDIT
aggregate median improves9.9%; Window is essentially unchanged, not a universal
speedup. Full product r013 and deployed Console/Window smoke pass. Management
frontend-close failed in r014; fixed paired r018 also reproduced it in the
unchanged accepted baseline (worker exits1067 while CMD survives). The owner
approved its bounded close-ordering dependency repair. r025 now passes four
fixed real close samples, unexpected-worker-loss target survival and independent
Console isolation. Production priority fixture and existing shutdown37 pass.
The original failed samples remain evidence, not reclassified as passes.
r026 repeats Console17/Window17 and all retained WOW frontiers successfully
with the repaired coherent package. r027 publishes its eight matching files
to O:/winnt/system32; recovery is r027/recovery. NTVWM is the only additional
changed binary beyond the release DECODE repair. r028 published Console/Window
COMMAND/MEM/EDIT/native smoke and all eight hashes pass. Production123c0ad3e
is committed and pushed to main; S2 reaches its bounded closure. Owner manual
acceptance and overall T429 closure remain distinct and pending.
Test-only r021/r023 narrow the remaining transaction cost to per-cell Console
code-page queries (94% of cumulative text commit in r023). Sequential S3 now
repairs exactly that query cost; its full runtime/publication gates now pass.
Original frame/input assertions pass.
S1/S2 measured wrapper
binaries were never published.

S1 delivered measurements at c8754f75d,b7178e53d,3b9d01a08 and its final
control review.42 baseline cases, missing-marker negatives, actual EDIT200
and graphics200, producer/transfer/frontend timing, queue conservation and
instrumentation units pass. r020 original/disabled/enabled comparisons pass;
run-order variability does not establish zero measurement overhead. Actual
transfer cost is larger than assembly, but its cause remains unisolated.

NTSRV remains relationship/lifecycle authority; original DOS/WOW execution,
records, scheduling, blocking/resume and cleanup remain in their mirrors.
NTCON is presentation; NTVWM owns native targets and hidden Console.
Root/system32 lookup and ordinary application search remain unchanged.
Shared GUI carriers reside/reuse; no exclusive GUI option is introduced.

Physical RDP/RawInput/focus is waived or unobserved, not passed. Matched SoftPC
runtime comparison remains unperformed; S4 must disclose it. WOW keeps retained
frontier nonregression, not broader usability. Host scrollback is not promised.
S3 now owns only the measured text-frame conversion cost; S4 remains later.
Native30ms polling is unchanged. [S3 evidence](../etc/evidence/m0-t429-s3-frame-codepage-snapshot.md)
records r005 importer/rollback/public-channel passes and r008's fixed six-sample
EDIT200 comparison. Commit median falls100350→5512us; whole scripted workload
median8924→7958ms. These are injected private-desktop observations, not physical
RDP latency. r007's wrong-observer rejection remains evidence. S3 r009 formal
Console17/Window17/WOW passes (total219979ms). r010 publishes the coherent eight
files to O:/winnt/system32 with only NTCON changed; r011's published Console/
Window smoke and all hashes pass. S3 final diff review is complete; its reviewed
production commit/push and sequential S4 admission are the remaining handoff.
Other-session proposal chronology is preserved and excluded from this delivery.

## Recent M0 Closures

| Task | Outcome and evidence |
| --- | --- |
| T428 | Owner-closed worker interface unification, production68e860553/statusb1b77e129; [closure](../history/m0-t428-worker-interface-unification-closure.md). |
| T427 | Owner-accepted system-root/search isolation; [closure](../history/m0-t427-system-root-search-isolation-closure.md). |
| T426 | Owner-accepted monitor tree; [closure](../history/m0-t426-console-root-monitor-tree-closure.md). |

## Recent Governance

S1 concludes as measurement-only; S2 is bounded-closed at production123c0ad3e.
S3 is admitted sequentially as the only active bounded repair.
Guest and lifecycle policy remain unchanged; the approved close dependency
repairs ordering only. T429 remains open and other-session work is preserved.
