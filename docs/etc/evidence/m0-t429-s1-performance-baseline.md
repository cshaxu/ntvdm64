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
