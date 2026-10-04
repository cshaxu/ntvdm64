# Project Status

## Current Work

**Active: M0 T429 S2** (Ordinary Mode).

Owner admitted the performance package after closing T428. S1's bounded
measurement conclusion is recorded in the
[baseline evidence](../etc/evidence/m0-t429-s1-performance-baseline.md).
Only S2 is now active, following the approved sequential scope.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T429 S2; Ordinary Mode. |
| Admission And Approval | Sequential admission after S1's baseline/control/measurement exit; owner directs continuous execution-performance optimization. |
| Candidate Proposal | [Execution-performance recovery](../proposals/proposal-ntvdm-execution-performance-recovery-001.md). |
| Objective | Remove project-added per-instruction DECODE diagnostics from the normal selected CCPU40 build; retain explicitly selected diagnostic capability and prove behavior and measured effect. |
| Non-goals | No CCPU algorithm, guest/firmware, CPU30, lifecycle/wire, helper/channel/component/scheduler change; no NTCON/worker-base display dedup or speculative input/transport optimization. |
| Reference Baseline | Production68e860553, T428 S6/r002/runtime/system32; main3b9d01a08 S1 measurement. MSVC14.43/SDK22621/x86 /MT CCPU40; APP0.0.427/RPC38/I/O25. Published eight files match accepted baseline. |
| Files And ABI Surface | c_main.c project diagnostic selection and registered mirror README; reproducible object-selection/performance tests and evidence. No cross-EXE ABI change. |
| Applicable Rules | Full AGENTS set, source policy, mirror provenance/minimal registered diff, original instruction semantics, build-only artifacts, preserved other-session edits and serial global endpoint/Z:-only tests. |
| Verification | Incremental affected x86 closure with source/toolchain/input identity; normal and diagnostic object checks; identical-package benchmark/control comparison; affected CCPU/input/video tests; Console17/Window17/retained WOW/lifecycle, coherent recovery/publication and published smoke before production P. Governance, links and diff review. |
| Expected Markers | Normal DECODE has no diagnostic call or replacement per-instruction branch; diagnostic build retains selected observer references; real guest/output/order/receipt and existing frontiers do not regress. Report measured improvement or contrary result. |
| Asset Needs | Validated build/M0-T427/S2/r001 dependency cache; immutable T428 S6 runtime/media; new S2 outputs under build/M0-T429/S2. Read-only SoftPC/upstream comparison only. |
| Reporting Requirements | Separate source/object, measured and physical observations; report hashes, commands, failed attempts, median/tails and retained limits, not inferred speed claims. |
| Stop Conditions | Need for instruction/guest semantics, new wire/lifecycle/helper or comparison runtime dependency pauses repair for renewed approval. Unexplained regression prevents delivery. |
| Exit Criteria | Selected normal object excludes per-decode diagnostics, separate diagnostic object retains them; measured effect and semantic/production gates reviewed, coherent eight files published, commit/push delivered. |
| Original Owner Request | Improve execution performance while preserving original OpenNT/CCPU semantics and frontend/worker boundaries; first remove the confirmed added release hot-loop diagnostic work. |
| Similar-Issue Sweep | All project-added WOW observers within DECODE, other diagnostic calls at coarse boundaries, false claims from startup/measurement overhead and unchanged native controls. |

## S1 Closure Record

Measurement-only S1 concludes with [baseline and control evidence](../etc/evidence/m0-t429-s1-performance-baseline.md), including r020 matched variants.
No production speedup is claimed; physical input and matched SoftPC runtime
comparison remain explicitly unproved. Sequential S2 is admitted, not T closure.

## Current Technical Baseline

T428 is owner-closed at production68e860553/statusb1b77e129; its
[closure](../history/m0-t428-worker-interface-unification-closure.md)
retains S1-S6 results. O:/winnt/system32 contains the coherent six EXEs and
WOW32.DLL/VDMREDIR.DLL matching build/M0-T428/S6/r002/runtime/system32.
Recovery is S6/r011/recovery. S1 test-only measured binaries were never published.

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
S3/S4 remain later scopes, not active work. Native30ms polling is unchanged.
Other-session proposal chronology is preserved and excluded from this delivery.

## Recent M0 Closures

| Task | Outcome and evidence |
| --- | --- |
| T428 | Owner-closed worker interface unification, production68e860553/statusb1b77e129; [closure](../history/m0-t428-worker-interface-unification-closure.md). |
| T427 | Owner-accepted system-root/search isolation; [closure](../history/m0-t427-system-root-search-isolation-closure.md). |
| T426 | Owner-accepted monitor tree; [closure](../history/m0-t426-console-root-monitor-tree-closure.md). |

## Recent Governance

S1 concludes as measurement-only; S2 begins with one bounded active packet.
This admission/control delivery changes no production binary, guest or process
policy. Governance, relative links and diff checks precede commit/push.
