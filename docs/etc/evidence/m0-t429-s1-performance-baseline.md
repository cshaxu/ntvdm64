# T429 S1 — initial execution-performance baseline

## Question and scope

Which costs are established before changing NTVDM? This is an initial S1
measurement delivery, **not S1 closure or a performance repair**. S1 remains
active for input/IRQ/queue and producer/transfer/presentation attribution.
No production source, guest media, protocol or deployed component changed.

## Inputs and procedure

Source HEAD at measurement: 4582f581d; accepted production: 68e860553.
`build/M0-T429/S1/r003/manifest.json` pins all staged system32 inputs, observer
source/binary identity and host/session metadata. Eight product files match
the sealed T428 S6 r002 runtime and O:/winnt/system32. APP0.0.427/RPC38/I/O25,
MSVC14.43/SDK22621 Win32/x86 /MT CCPU40. Original guest files/configuration
are copied, not rebuilt or patched. No SoftPC runtime comparison was run yet.

The test observer is compiled with the existing x86 environment wrapper:

```powershell
& build/M0-T427/S4/r049/msvc-x86.cmd cl.exe /nologo /TC /std:c11 /MT /W4 /WX /wd4201 /D_CRT_SECURE_NO_WARNINGS /Isrc/ntcon-exe /Fobuild/M0-T429/S1/r001/console-startup-observer.obj /Febuild/M0-T429/S1/r001/console-startup-observer.exe tests/observation/console_startup_observer.c /link kernel32.lib user32.lib dbghelp.lib
```

Reproducible measurement entrypoint:

```powershell
& tests/observation/measure-edit-baseline.ps1 -Observer build/M0-T429/S1/r003/console-startup-observer.exe -RuntimeRoot build/M0-T429/S1/r003/runtime -ReportRoot <fresh-build-report-directory> -Iterations 5
```

Run r003 uses Z: for the isolated runtime. Every sample has a fresh product
process set and private test desktop; product/global endpoint tests remain
serial. The filesystem is warmed, not the worker: one disabled-instrumentation
sample, one enabled warm-up and five measured iterations per mode/case.
Cases are COMMAND exit, COMMAND -> EDIT -> menu exit -> MEM -> exit and
direct Win32 batch exit 7. The last case is the unaffected **no-Window native
control**, even when run in the Window-configured series: it has no scripted
CAF or interactive Window request. It is not a Window interaction pass.

The default disposable Console was observed as buffer 120x9001, viewport
120x30. This is not an assumed 80-column or physical RDP baseline. Pre-input,
EDIT and per-line snapshots retain actual geometry/content. S1/r002 was a
successful timing-mechanism pilot under the historical 80-column short-viewport
configuration, not the r003 ordinary-geometry baseline; do not mix them.

Optional MVDM_OBSERVER_PERFORMANCE records bounded samples in observer memory
and writes them only in the final report. The production package has zero
new measurement code. Origin is immediately after CreateProcess returns:
startup-ready is the first actual visible prompt, route-ready confirms the
existing CAF route, edit-submit precedes its typed input, visible-text uses
the existing rendered EDIT/menu predicates, and direct-completion observes
the actual run16 completion. This includes existing echo/chunk pacing and
observation cadence; it is not pure executor CPU time or physical input latency.

## Measured results

All **42** ordinary-geometry case executions pass their original exit-code,
output, ordering and EDIT interaction assertions. Five measured samples per
row; excluded warm-up/disabled samples remain in the report. Unit: milliseconds.

| Route/case | Median | Min–max | Meaning |
| --- | ---: | ---: | --- |
| Console COMMAND startup | 2390 | 2312–2688 | Post-CreateProcess to prompt; empty case. |
| Window series COMMAND startup | 2375 | 2250–2609 | Prompt before CAF; empty case. |
| Console EDIT submit to visible | 3328 | 3219–3500 | Typing/echo and first rendered Untitled. |
| Window EDIT submit to visible | 1937 | 1672–2156 | Starts after CAF readiness; same guest. |
| Console entire EDIT/MEM receipt | 17015 | 15515–20062 | Post-create to actual direct completion. |
| Window entire EDIT/MEM receipt | 8109 | 7422–8860 | Includes CAF, menu, return and MEM. |
| Native control direct receipt, Console series | 1375 | 1312–1453 | Real Win32 result 7, no CCPU. |
| Native control direct receipt, Window-configured series | 1375 | 1016–1453 | Same no-Window native control. |

The sample size does not establish a reliable p95/p99. Max is the observed
tail, not a promised SLA. Console/Window differ materially in this fixture;
these data contradict assuming a single CPU-only explanation for all delay.
Wall times in measurements.json additionally include script setup/CIM/cleanup;
they are not guest latency. Disabled samples pass unchanged assertions and
emit no timeline. Their wall times overlap the enabled ranges in most cases,
but are not a statistical proof of negligible observer overhead (one disabled
sample and cache/order noise). No improvement is claimed.

## Source and isolated-cost findings

| Location | Established fact | Disposition |
| --- | --- | --- |
| mvdm/softpc.new/base/ccpu386/c_main.c:878–902 | DECODE unconditionally invokes NT-transition diagnostics and checks four additional WOW instruction ranges. | Review the whole added diagnostic block in S2, not only the first call. Original instruction execution remains unchanged. |
| ntvdm-exe/softpc/mvdm_softpc_termination.c:29–59 | Disabled trace still performs TLS/state and GetLastError/SetLastError work per call. | Source-confirmed release-path work, not yet a measured product-speedup claim. |
| Selected build/M0-T427/S2/r001/build.ninja | CCPU cflags shown in the selected graph have no explicit /O optimization switch. | Record effective environment/toolchain before comparing release/compiler alternatives; no flag change is made here. |
| ntvdm-exe/win32/console_compat.c:466–496 | At most five raw records per read, bounded by mouse capacity; Sleep(1) only when downstream capacity is exhausted. | Need actual capacity/batch/backlog measurement; do not call this per-event throttling. |
| ntvdm-exe/softpc/mvdm_softpc_mouse_input.c | Pressure merging begins above 16 queued movements; button/route edges remain distinct. | Preserve original-compatible displacement/order, measure consumption rather than assuming every motion becomes one IRQ. |

`tests/observation/nt_transition_cost.c` links the actual, unmodified diagnostic
provider translation unit, compiled x86 /MT /Gy without added optimization.
Unused provider boundaries have fail-closed test link sentinels, not substitute
CPU/guest implementations. Six 5-million-call trials include one warm-up;
trace path is unset, last-error preservation is asserted. r001 retains output.
Measured whole-call median is about **11.41 ns**, range **10.58–12.04 ns**;
volatile-loop control median about 0.92 ns. This establishes a nonzero isolated
cost, not its fraction of EDIT execution or the cost of the other DECODE
conditions/getters. No product speedup is extrapolated from it.

Provider and fixture build commands:

```powershell
& build/M0-T427/S4/r049/msvc-x86.cmd cl.exe /nologo /TC /std:c11 /MT /Gy /W4 /D_CRT_SECURE_NO_WARNINGS /Isrc /Isrc/ntvdm-exe/softpc/include /Fobuild/M0-T429/S1/r001/nt_transition_provider.obj /c src/ntvdm-exe/softpc/mvdm_softpc_termination.c
& build/M0-T427/S4/r049/msvc-x86.cmd cl.exe /nologo /TC /std:c11 /MT /W4 /WX /Fobuild/M0-T429/S1/r001/nt_transition_cost.obj /Febuild/M0-T429/S1/r001/nt-transition-cost.exe tests/observation/nt_transition_cost.c /link build/M0-T429/S1/r001/nt_transition_provider.obj /OPT:REF kernel32.lib
& build/M0-T429/S1/r001/nt-transition-cost.exe
```

## Negatives, ownership and limitations

`verify-performance-milestones.ps1` in S1/r004 runs with instrumentation off
and on. A direct native target exits 7 without emitting S1-NOT-EMITTED:
both reject readiness; the enabled report explicitly records observed=0
for startup and observed=1 for real completion. Neither target exit nor
input submission becomes a fabricated readiness success. Both negatives pass.
Build attempts with missing includes/link dependencies are retained as failed
attempts; final fixture build and execution succeed with no sentinels reached.

The fixture's existing prompt/echo/menu predicates and product assertions
are unchanged. Optional timeline overflow fails reporting. Authored test-only
measurement has no historical implementation to replace: it reuses current
observer/cleanup mechanisms, does not insert an instruction-loop observer,
and does not own production scheduling/receipt state.

All cleanup stays within pinned isolated package paths/process identities.
Z: is removed after r003 and r004; no production deployment or process takeover
was needed. Other-session proposal changes are preserved, outside this delivery.

Still **unproved**: actual raw-input-to-IRQ and IRQ-to-present latency/batch
distributions, SoftPC matched-workload comparison, physical RDP behavior and
instrumentation perturbation at those boundaries. The existing mouse-hook ACK
only proves frontend input-sink acceptance; WMS7 completion includes settling
and is not a mouse latency measurement. No 1000-event extreme run was used.
S1 remains open; S2 is not admitted by this initial baseline delivery.

## Worker queue/transport measurements — subsequent S1 delivery

Question: does the retained 200-input workload produce unbounded worker
backlog, and how long do input, video and final handoff transactions take?
No production source, mirror, wire, guest or published package is changed.
Test-only wrapper TUs include the actual project-owned bridge/client, call
the real implementation once, and precede the unchanged production archives
at link. Original ICA ownership still serializes queue push/take. A test-only
SRW lock protects a fixed8192 sample array; recording has no file I/O.
Successful existing quiesced I/O close flushes the report. Overflow is explicit.

Entrypoints (fresh build/report roots required):

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tests/observation/build-worker-performance.ps1 -BuildRoot O:/repos.hobby/ntvdm64/build/M0-T429/S1/r007
powershell -NoProfile -ExecutionPolicy Bypass -File tests/observation/measure-worker-boundaries.ps1 -MeasuredWorker build/M0-T429/S1/r007/ntvdm-performance-observer.exe -Observer build/M0-T429/S1/r003/console-startup-observer.exe -ReportRoot O:/repos.hobby/ntvdm64/build/M0-T429/S1/r009
```

Build reuses manifest-hashed selected x86 /MT CCPU40 link inputs from
build/M0-T427/S2/r001. VDM_TIB storage validation passes. The /OUT warning
identifies the retained provider export name versus test executable filename.
r007 measured worker SHA256:
8A5FEA9519578DD01004C5C4B6C3C27900E43FC6E6A61EAE0DFCD577DD706FD4.
Observer AC9A7AF8D74F5DD5EF55D4C3F13736FE697F6D76C7EC9F92A69F9D59C8761188;
WMS7.COM A60EDF938A866810CC3BD3D92800E72A5BEB73369C12580A14F1BEA8BA337E6A;
S7MOUSE.dll EE490315CF28E428FEF9BDE854D89CBCA4FB48CF249D205CDE16C167C8418A80.
Only the test-linked worker differs in the isolated accepted-package copy.
Only Z: is mapped, then removed; pinned cleanup is not proof of normal retirement.

The private-desktop hook feeds the actual frontend input sink, not hardware
RawInput. WMS7 is an authored graphics fixture, **not EDIT**: actual guest
callbacks assert movement, one press/release and final released buttons.
Each measured run reads205 records in batches up to5, including route edges
and203 injected movements/button samples. Counts below establish consumption
of every enqueued item, not one IRQ per merged movement.

| r009 iteration | Enqueued / merged / consumed | High-water | Queue-age median / max (us) | Read median / max (us) | Video transaction median / max (us) |
| --- | --- | --- | --- | --- | --- |
| 1 | 65 / 140 / 65 | 17 | 4339 / 21501 | 52 / 367 | 1196 / 350193 |
| 2 | 53 / 152 / 53 | 17 | 5815 / 141282 | 43 / 431 | 1474 / 355766 |
| 3 | 63 / 142 / 63 | 17 | 4959 / 18922 | 64 / 235 | 1489 / 353011 |

No sample overflow, unmatched consumption, rejected input or transport error.
Queue age starts at the oldest enqueue in a merged slot and ends at actual
queue take by the original CPU/IRQ consumer; it is not host-to-render latency.
Video timing covers the synchronous chunked transaction and peer replies,
not separately identified copy/render phases. Seven transactions mix graphics,
mode changes and final text; their maximum is not steady EDIT text latency.

Final handoff barriers take57/33/36us; actual broker-authorized I/O-close ACKs
take1882/1814/2244us. Existing guest/direct-exit assertions pass. Disabled total
11048ms versus measured10403/10523/10685ms includes fixed probe settling,
mode changes and startup, not mouse latency or evidence that measurement is free.
Earlier r005/r006 exploratory inputs are retained separately. Final labels
measure actual handoff/close, not a nonexistent publication message in this caller.

`worker_performance_test.c` links the actual pure queue plus measured support;
it is a unit, not mock guest acceptance. Build with the recorded x86 environment
wrapper /MT /W4 /WX /TC /Isrc, linking r007/worker_performance.obj and the
actual mvdm_softpc_mouse_input.c object. r008 enabled/disabled both preserve
displacement200, seventeen queue entries and the last-error sentinel.
Enabled has217 samples/high-water17; disabled writes no report. Deliberate
sample8193 produces samples8192/overflow1: detection, not silent truncation.
Initial mistyped /Fo placed one test object at repository root; it was
immediately moved by exact path to r008/worker_performance_test.obj.
No stray intermediate remains and no recursive removal was used.

The final runner also enforces enqueue+merge>=203, consumed==enqueued, zero
errors and actual handoff/close markers. Existing r009 reports are rechecked
against these added assertions; measured binary/observer are unchanged.
Later build-manifest source-hash metadata does not change r007 executable code.

Interpretation: this workload proves bounded, conserving mouse batching, not
that EDIT has no latency issue. Approximately141ms consumer tail remains
observed, without queue explosion. Still open: matched EDIT mouse measurement,
raw-input/producer/frontend attribution, matched SoftPC comparison and meaningful
instrumentation-overhead measurement. S1 remains active; S2 is not admitted.

## Actual EDIT producer/frontend measurement — r010-r019

This continuation adds only test-link wrappers around unchanged project-owned
`console_text.c` and `frontend_session.c`. No production source, original CCPU,
input/IRQ scheduling, guest media or wire contract changes. The worker producer
is selected ahead of its original archive member; the frontend link replaces
precisely its direct session object. r016/manifest.json pins measurement sources,
both outputs, the graph and every original link input. VDM_TIB ownership passes.
These binaries are test-only, not published to O:/winnt.

Reproduction:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tests/observation/build-worker-performance.ps1 -BuildRoot <fresh-build-root>
powershell -NoProfile -ExecutionPolicy Bypass -File tests/observation/measure-worker-boundaries.ps1 -MeasuredWorker build/M0-T429/S1/r016/ntvdm-performance-observer.exe -MeasuredFrontend build/M0-T429/S1/r016/ntcon-performance-observer.exe -Observer build/M0-T429/S1/r012/console-startup-observer.exe -ReportRoot <fresh-build-report-root> -Edit -Iterations 3
```

r012 observer uses the existing x86 `/std:c11 /MT /W4 /WX /wd4201` recipe,
with explicit /Fo and /Fe paths. `-Edit` retains ordinary actual menu exit,
MEM text and exit1 assertions. It injects200 alternating net-zero relative
moves after EDIT's document-ready milestone; a Window-thread FIFO marker
acknowledges sink acceptance, not guest consumption. No settle sleep or click
changes the EDIT document. The graphics probe retains its movement/click/release
sequence and guest assertions. Frontend identity is verified against the actual
run16 EXE's sibling file, not guessed from the working/package-root argument.

r017 passes disabled control plus three enabled EDIT runs, whole-case
elapsed10499/8933/8352/8960ms. Startup, scripted input/menu and cleanup are
included; their ordering is not evidence of instrumentation speedup/overhead.
Buffers have no overflow, rejected input or unmatched consumption. Five barriers
and five close acknowledgements per run succeed. Frontend decode/present timings
cover real library calls, not physical display scan-out.

| Run | Enqueued / merged / consumed | Queue peak | IRQ age median / max us | Assembly median / max us | Successful transfer median / max us | Decode median / max us | Present median / max us |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | 51 / 153 / 51 | 17 | 7040 / 68322 | 48 / 73 | 106473 / 128717 | 108 / 184 | 78 / 408581 |
| 2 | 46 / 159 / 46 | 17 | 6678 / 8741 | 41 / 72 | 65674 / 86845 | 105 / 210 | 63 / 366787 |
| 3 | 49 / 156 / 49 | 17 | 6234 / 97950 | 45 / 101 | 99221 / 132011 | 92 / 190 | 68 / 57357 |

IRQ age is oldest retained enqueue to the actual CPU/IRQ owner's queue take,
not guest callback completion. Counts include existing route edges, hence204/
205 submissions versus200 injected moves. Every enqueued item is consumed;
menu/MEM/exit evidence prevents sink ACK alone from masquerading as completion.
Successful video transactions number18/17/20; text frames are20900bytes.
Synchronous transfer includes serialization/import/peer work, not just copying.
Worker read transaction medians63/61/56us, maxima723/439/883us. No unbounded
mouse backlog is demonstrated by this workload.

Runs1 and3 each record one video ERROR_NOT_READY (21). The actual producer
explicitly accepts a queued paint crossing ownership handoff (`console_text.c`,
the comment after its publication call). It remains a rejected publication,
excluded from successful-transfer latency, not a delivered frame. All other
errors fail, including input/barrier/close. Graphics retains its zero-error
assertion. r019 rechecks graphics with the new observer: disabled and enabled
200-move/click/release cases pass (11616/10189ms).

### Instrumentation cost and failed attempts

r018 links the actual queue with the measurement unit. Enabled/disabled200-input
conservation and last-error checks pass. Overflow emits8192samples/overflow1;
disabled creates no report. Cost cases each run32 conserving200-move bursts
(6944 operations), including the wrapper's InitOnce/disabled branch and excluding
flush/file I/O. Five enabled runs take1300700..1319700ns, median1310000;
five disabled runs184300..185800ns, median185000. Difference is approximately
162ns/operation in this uncontended unit. This is measurement cost, not the
full multithreaded product's perturbation or original-release versus disabled
wrapper cost. Disabled real cases prove behavior/count/receipt equivalence,
not a statistically bounded wall-time overhead. Private sink injection bypasses
physical/RDP RawInput, which remains unobserved.

r011 fails before injection (burst0): argv2 is the root, not binary directory.
Its timeout and cleanup access-denied diagnostic are retained, not passed.
r012 corrects sibling identity. r013 product assertions pass, but its strict
measurement checker rejects video21. The checker now classifies precisely the
producer's accepted EDIT result, retaining all input/completion assertions.
r014 lacks frontend C11/include settings; r015 lacks the renamed forward
declaration. Both compile failures remain. r016 links successfully without
force-multiple or substitute provider. Its existing export /OUT warning is the
descriptive test EXE name mismatch already recorded in r007.

Read-only SoftPC comparison checks its
`src/app-softpc/softpc.new/base/ccpu386/c_main.c:901`: DECODE fetches opcode
without this project's transition/WOW observers. SHA256:
23BD97F2DEE8ED68171735AE2AAB04BB737C903AC5B65DB1654CDCE92E61368F.
No source import, runtime/build/acceptance dependency is introduced. Matched
SoftPC runtime comparison remains unperformed rather than passed.

Assembly median41..48us is small beside successful-transfer median65..106ms.
This identifies a boundary for investigation after release hot-loop repair,
not a proven particular lock/import/transport culprit or permission to batch.
Some present maxima include initial Window setup. Three samples do not prove
physical mouse smoothness or an end-to-end SLA. S1 remains open for final
baseline/perturbation disposition; S2 is not admitted. All eight published hashes
still match T428 S6 r002; no test broker or Z: remains after serial runs.

## S1 final comparison and bounded conclusion — r020

`measure-worker-boundaries.ps1 -Edit -CompareControl -Iterations 3` uses
the same r016 measured worker/frontend and r012 observer as above. After one
excluded warmup it serially alternates accepted original, disabled wrappers,
enabled wrappers, three times. Each case pins actual binary hashes after the
previous identity-scoped cleanup; no product package is modified.

| Variant | Whole-case milliseconds, in run order | Median ms |
| --- | --- | --- |
| Accepted original | 11030,10425,9013 | 10425 |
| Disabled wrappers | 11102,9347,9577 | 9577 |
| Enabled wrappers | 11270,9018,8771 | 9018 |

All ten cases (including warmup) pass actual EDIT readiness,200 continuous
movements, menu exit, MEM output and direct exit1. Enabled runs retain two
bounded reports and queue conservation/overflow/error/barrier assertions;
original and disabled runs write no worker measurement report. This proves
control behavior and provides actual perturbation comparison, not a claim that
instrumentation accelerates the product or has zero cost. The downward run-order
trend and three samples do not establish a tight statistical overhead bound;
r018 separately measures approximately162ns per uncontended queue operation.

S1's bounded baseline exit is satisfied: pinned ordinary build/runtime and
workload, startup distributions, actual EDIT producer/transfer/presentation
and queue/batch distributions, unaffected native control, disabled behavior,
cost and overflow negatives. No production repair or speedup is claimed.
Physical/RDP RawInput and matched SoftPC runtime comparison remain unproved;
S4 must retain that distinction. Full-product timing includes fixed test work
and is not an end-to-end mouse SLA. S2 may now test the source-proven release
DECODE diagnostic cost; S3 may change transport/input only after causal evidence.
