# Project Status

## Current Work

**No active M/T/S packet.** T429 remains open for owner manual acceptance.
S5 has reached bounded delivery; no succeeding S or T is admitted.

Owner admitted the performance package after closing T428. S1's bounded
measurement conclusion is recorded in the
[baseline evidence](../etc/evidence/m0-t429-s1-performance-baseline.md).
S3 is delivered at production0654f5e0b after S2 production123c0ad3e.
The owner closes delivered S4 and admits S5: restore timely software-VGA
publication without synchronous frontend transport in mouse IRQs. Register the
minimal mirror exception; build and run focused checks, publish the coherent
eight-file candidate for owner testing, then run the full regression gate.
Early publication was not S5/P closure. Final S5 delivery is recorded below;
T429 remains open.

## S5 Delivery Record

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T429 S5; Ordinary Mode. |
| Admission And Approval | Owner supersedes and requests reversal of the unpublished heartbeat/FIFO candidate. Explicitly approves a dedicated event-driven VGA publisher, latest-state coalescing, complete-state comparison against the last successful send, maximum50Hz (20ms), and mandatory final-state acknowledgement. Publish after build/focused checks for owner testing, then run the full suite. |
| Candidate Proposal | [Execution-performance recovery](../proposals/proposal-ntvdm-execution-performance-recovery-001.md). |
| Objective | Remove the proven mouse-update-to-video-tick deferral in software Window presentation while preserving original VGA/mouse algorithms and keeping frontend transport outside mouse IRQ handling. |
| Non-goals | No guest/firmware, CCPU algorithm, heartbeat, lifecycle policy, wire, helper/channel/component or generic scheduler change. No dedup in NTCON/worker-base. NTVWM30ms sampling unchanged. One NTVDM-owned publication thread only, no intermediate-frame FIFO, no idle periodic polling. |
| Reference Baseline | Production0654f5e0b; coherent S3 r006/runtime, r009 Console17/Window17/WOW and r010/r011 deployment/smoke. MSVC14.43/SDK22621/x86 /MT CCPU40; APP0.0.427/RPC38/I/O25. S3 matched EDIT200 commit median100350→5512us, scripted workload8924→7958ms. |
| Files And ABI Surface | NTVDM software-video and Console adapters; minimal nt_graph.c and pause/resume nt_event.c hooks. Original guest owner extracts immutable copies; the event-driven publisher compares/sends latest copies outside guest/painter locks. nt_timer.c stays unchanged. Explicit stop/join and final-paint barrier; production-linked tests and registered source exception. No cross-EXE ABI or ownership change. |
| Applicable Rules | Full AGENTS set, source policy, mirror provenance/minimal registered diff, original instruction semantics, build-only artifacts, preserved other-session edits and serial global endpoint/Z:-only tests. |
| Verification | Audit execution/ICA/transport lock ownership, suspension and final-paint barriers before editing. Focused wake/publication/failure fixtures, original EDIT200 and keyboard/Console return, x86 incremental build, coherent early publication/recovery/hash check; then serial Console17/Window17/WOW and affected handoff/lifecycle gates, governance/links and final independent diff review. |
| Expected Markers | No frontend transport in mouse IRQ; timely publication after actual software update; no guest-clock advancement caused by display notification; input conservation, idle cursor behavior, final frame/parent return and orderly teardown retained. Distinguish measured boundaries from physical latency. |
| Asset Needs | Validated T427 S2 r001 dependency cache; exact S3 r006 runtime/media and S3 r010 deployed manifest. Existing private-desktop observers and fixtures; fresh S5 build-only reports, Z:-only serial runtime. |
| Reporting Requirements | Separate source, existing instrumented evidence and fresh measurements; disclose publication before full verification and exact known limits. Record every retained mirror expression and rejected recovery rung. |
| Stop Conditions | Unsafe concurrent VGA access, need for extra threads/protocol/scheduler/lifecycle policy, guest changes or frontend dedup pauses implementation. Do not repurpose heartbeat/input listener or trigger extra guest timer ticks. |
| Exit Criteria | Focused and full affected gates pass, coherent package published, minimal exception/source accounting reviewed, evidence committed/pushed. Owner hand test and T closure remain distinct. |
| Original Owner Request | Close current S, admit a repair S, register diff exception, publish for hand testing before running the full suite. |
| Similar-Issue Sweep | Mouse draw/undraw, cursor/register-only updates, graphics/text changes, Console/Window route, blocked/resumed guest, final publication and shutdown priority; no idle repeated publication. |

## S1 Closure Record

Measurement-only S1 concludes with [baseline and control evidence](../etc/evidence/m0-t429-s1-performance-baseline.md), including r020 matched variants.
No production speedup is claimed; physical input and matched SoftPC runtime
comparison remain explicitly unproved. Sequential S2 is admitted, not T closure.

## S2 Closure Record

[S2 diagnostic-selection and close-ordering evidence](../etc/evidence/m0-t429-s2-release-decode-diagnostics.md)
records source/object proof, matched distributions, all retained runtime gates,
the approved native-close dependency and coherent eight-file publication.
Production123c0ad3e is pushed. S2 is bounded-closed, not T429 owner closure.

## S3 Closure Record

[S3 frame-local code-page evidence](../etc/evidence/m0-t429-s3-frame-codepage-snapshot.md)
records importer/failure/channel tests, six matched EDIT200 samples, full product
and coherent deployment/smoke. Production0654f5e0b is pushed; S3 is bounded-closed.
Its succeeding S4 final integration is delivered below; S5 is now active.

## S4 Closure Record

[S4 integration audit](../etc/evidence/m0-t429-s4-integration-audit.md) records
the final eight actual integration/lifecycle passes at unchanged published
identity, retained failed combined run, native environment diagnostic and
minimal test-only flag-restoration fix. Production0654f5e0b remains deployed.
Verification/test-only973a671bf is committed and pushed. S1–S4 are bounded-closed;
T429 stays open for owner acceptance, not additional automatic implementation.
The owner's subsequent direction closes this S4 delivery and admits only the
bounded S5 software-video publication repair above. Existing S4 evidence and
production identity are retained; this is not T429 closure.

## S5 Closure Record

[S5 software-VGA publication evidence](../etc/evidence/m0-t429-s5-software-video-publication.md)
records the event-driven copied publisher, source exception DIV-326, final
x86 dependency build, production fixtures and50 handle-clean teardown cycles.
The final r019 package passes Product r020 and r028 (Console17/Window17 and
each retained WOW frontier), plus all eight handoff/lifecycle cases r026/r029.
r018 passes Console4/Window4 and actual EDIT200. r017 publishes all eight
matching files to O:/winnt/system32 with recovery; r021 passes published
Console/Window smoke and every hash. Source review preserves original guest
ownership and clock, final-frame acknowledgement and cancellation after reopen.
NTCON/worker-base have no dedup or production changes. The failed r012 attempt
and causal uncertainty remain explicit evidence, not a passed or waived test.
Test environment restoration is repaired without weakening assertions.
S5 reaches its bounded closure; T429 owner acceptance remains pending.

## Current Technical Baseline

[S5 source audit and delivery](../etc/evidence/m0-t429-s5-software-video-publication.md)
record the approved independent event-driven publisher, latest-state/full-copy
comparison and20ms cap. Guest-clock timing, original VGA and mouse algorithms
remain unchanged. Unpublished heartbeat/FIFO edits were reverted; no
guest-timer wake is authorized. Final r019/runtime is published coherently at
O:/winnt/system32, with NTVDM SHA256
F3589AA37BB3FFB4621B3C580B5E637C693196637AEC9AAD286B80B2C17B3C5A.
Dependent VDMREDIR relinks; the other six artifacts are unchanged. r017 pins
all eight hashes/recovery, r021 verifies actual publication and smoke. S5 is
bounded-closed, not T429 acceptance. The early r005 candidate/recovery remains
historical evidence. The other session's proposal edits are preserved outside
this delivery; no new package or task is automatically admitted.

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
S3 delivers only the measured text-frame conversion cost; S4 delivery is complete.
Native30ms polling is unchanged. [S3 evidence](../etc/evidence/m0-t429-s3-frame-codepage-snapshot.md)
records r005 importer/rollback/public-channel passes and r008's fixed six-sample
EDIT200 comparison. Commit median falls100350→5512us; whole scripted workload
median8924→7958ms. These are injected private-desktop observations, not physical
RDP latency. r007's wrong-observer rejection remains evidence. S3 r009 formal
Console17/Window17/WOW passes (total219979ms). r010 publishes the coherent eight
files to O:/winnt/system32 with only NTCON changed; r011's published Console/
Window smoke and all hashes pass. S3 final diff review is complete and production
0654f5e0b is pushed. [S4 audit](../etc/evidence/m0-t429-s4-integration-audit.md)
records final integration and explicit limits, not T closure. S4 r004's eight
cases and r005's native environment diagnostic pass; the preceding r001 failure
is retained. Only test flag cleanup changed, not the eight deployed binaries.
Other-session proposal chronology is preserved and excluded from this delivery.

## Recent M0 Closures

| Task | Outcome and evidence |
| --- | --- |
| T428 | Owner-closed worker interface unification, production68e860553/statusb1b77e129; [closure](../history/m0-t428-worker-interface-unification-closure.md). |
| T427 | Owner-accepted system-root/search isolation; [closure](../history/m0-t427-system-root-search-isolation-closure.md). |
| T426 | Owner-accepted monitor tree; [closure](../history/m0-t426-console-root-monitor-tree-closure.md). |

## Recent Governance

S1 concludes as measurement-only; S2 is bounded-closed at production123c0ad3e.
S3 is bounded-closed at production0654f5e0b; S4 integration is owner-closed.
S5 reaches its bounded delivery with the final coherent published package.
No active packet remains; overall T429 acceptance is still pending.
Guest and lifecycle policy remain unchanged; the approved close dependency
repairs ordering only. T429 remains open and other-session work is preserved.
