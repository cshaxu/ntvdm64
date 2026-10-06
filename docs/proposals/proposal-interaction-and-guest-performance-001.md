# Proposal — Text responsiveness and DOS/Win16 execution performance

## Status and owner request

Owner planning on2026-10-06 places this unnumbered candidate at the head of
[Queue](../states/QUEUE.md): “加入队首T任务，看看如何消除文本卡顿感；还有审计当前真实执行的性能瓶颈（dos, win16启动和执行速度都明显感觉不够流畅)”.
This is next-package planning only. It neither admits a numeric T nor interrupts
the active task in [CURRENT](../states/CURRENT.md). Owner subsequently requests
incorporating the performance investigation into this proposal. The Build-only
measurements below are pre-admission research, not production implementation,
package acceptance or publication. This revision changes documents only.

Later owner planning on2026-10-06 inserts the
[Windows 1.01 color EGA / InPort mouse repair](proposal-windows-101-ega-inport-runtime-001.md)
before this candidate. This performance package is now at position 2; its
measurement and optimization scope is unchanged. Windows 1.01 hardware input,
color interaction and exit acceptance belong to the new head, not this package.

## Objective and boundaries

Reduce perceptible text interaction stalls in native CMD and DOS COMMAND/EDIT,
and identify actual bottlenecks in DOS and Win16 startup and steady execution.
Separate input acceptance, guest/target consumption, execution, screen capture,
publication, transport and presentation. A late picture does not prove slow
execution; fast CPU throughput does not prove responsive input.

Preserve NTSRV control/lifecycle authority, worker-neutral NTCON, current
worker-base boundaries, real native handles/results, original CCPU40/DOS/WOW
semantics and immutable guest media. No new helper, process, component, scheduler
or speculative architecture replacement is selected. Win16 GUI uses its actual
WOW/Windows front end; its stalls cannot be attributed to NTCON's text path
without a proved dependency.

Reuse T429's measured performance work and delivered remedies as evidence,
not an instruction to repeat its completed repairs or reuse obsolete numbers
as a current baseline. This candidate examines the product delivered at its
future admission, including any newly delivered Hook/observation overhead.

## Pre-admission measurements, 2026-10-06

Baseline: sealed `build/M0-T433/S7/r030-final-runtime`, APP433/RPC41/I/O25;
one AMD64 NTVWM handles both native widths with matching Hooks, NTVDM is I386
CCPU40. Ryzen AI9 HX370,12 cores/24 threads, Windows11 Pro10.0.26200, Balanced;
private unswitched desktop and accepted short-history geometry. Frequency,
affinity and other host load were not fixed. Current T434 observation changes
are not claimed profiled or delivered. Refresh identities at admission.

The local report and raw evidence remain in
`build/performance-research-20261006/`: `report.md`, `inputs.json`,
`source-inputs.json`, `hardware.json`, `phase-summary.json`,
`optimization-summary.json`, per-case reports/metrics/CPU/IP records and
`artifacts.json`. These are disposable research evidence, not product inputs
or substitutes for tracked acceptance tests. Conclusions are retained below
so this proposal does not depend on that local directory surviving.

| Exact owner/finding | Measured result | Required disposition |
| --- | --- | --- |
| Selected CCPU and ordinary native worker/frontend C flags omit `/O2`; Hook already uses it. | Only `c_main` optimized, base/o2/o2/base/base/o2 order, three samples each: without sampling, guest ticks median136→102 (25% less loop time), real task10265→8172ms (20.4% less). Sampled controls: ticks135→102, task10688→8375ms, NTVDM CPU8812.5→7125ms. | First finite optimization experiment to carry forward. Validate CCPU/adapter/native profiles separately; no blanket flag change or general EDIT/WOW speed claim. Initial linking exposed original stubs/trace `force_yoda` duplication; research-only check-stub selection is not a production linker repair. Resolve original owner/link selection before formal optimization. |
| `c_main.c:276` event snapshot, instruction completion at4618. | Long base-loop active-IP hits within the project image: ccpu88, c_cpu_event_snapshot77, ADD32, c_cpu_take_event9, LOOP16 9. Snapshot uses locked `InterlockedCompareExchange(0,0)`. | Hot location, not CPU-time percentage. Audit project DIV-214 and equivalent atomic-read options while preserving timer/reset/PIC notifications, concurrent consume/raise and memory order. Never blindly delete atomics. |
| NTCON `frontend_session.c:498–555`, controller present/poll/apply. | Lock-instrumented EDIT200: first present345567us, io_lock hold345801us, another thread wait354263us. Earlier first-present samples29/46ms. | Source proves public I/O lock covers synchronous window creation/switch/presentation. Decompose create/route/publish/destroy, then design copied snapshots and owner/generation/epoch confirmation to shorten lock spans. Retain FIFO, stale-owner exclusion and final-paint/close barriers. One tail is not stable p99. |
| Shared publisher and native acquisition. | Native acquisition remains20ms. EDIT200 publication waits median about22–24ms, observed maxima28–34ms, with few wait samples. The20ms publication interval starts after successful send completion. | Retain50Hz controls. Compare deadline definition, timer overshoot and acquisition/publication phase before changing caps; cancellation is not predetermined. |
| Actual EDIT200 queue-to-mouse-IRQ. | Two200-event library-input bursts pass; high-water17;63/52 consumption samples after merging. Median6976/9136.5us, sampled p95 about10–12ms; maxima71803/59453us early in the workloads. | Separate initial route/IRQ activation from sustained movement. These are merged queue lifetimes, not200 independent raw-event or physical-display latencies. Preserve displacement/buttons/order. |
| EDIT frame costs. | Text assembly median47/62us; full send2990/2730.5us; NTCON commit2573/2398us; decode75/79us; Window raster2010/1876us over33/32 frames. | Prioritize wait/lock tails. Sender/commit/raster overlap or nest: never sum their percentiles. Raster/copy/transaction optimization remains measurable work, not permission to filter received frames. |
| NTVWM native CMD input/capture/send. | Two short real native-return workloads: capture median1015/1049.5us, send3781/3726us, input2566.5/2842us. Extra native-channel lock test48 waits: median0us,max1us; hold median606.5us,p95 about4.7ms. | No sustained private-channel competition proved here. Outer membership lock is not separately measured. Add continuous typing, long output and history-height controls; changed-row comparison already exists. |
| DOS/native/frontend locks. | EDIT200 DOS-channel wait median/p950us,max6786us; hold median116us,p953480us,max290303us. NTCON wait median0us,p95217us plus the354ms first-window tail above. | Attribute long holds to exact operations/handoffs. Recursive intervals overlap; aggregate hold sums are not wall time. Do not remove necessary serialization. |

The self-authored COM executes65,535,000 ADD/LOOP pairs, prints start/end and
BIOS-tick witnesses and actually exits7. All optimized/control cases pass
those assertions. Three samples and BIOS tick resolution limit precision;
this is not an optimized product package. Default-off test-link wrappers
preserve operations and final acknowledgements; saved phase buffers have
overflow0/error0. Complete enabled/disabled pairing remains an admission gate.

### Startup coverage and research limitations

- Empty COMMAND startup-ready: unsampled control2125ms versus2578ms with the
  corrected sampler. This disproves zero perturbation, not a universal cost.
- Direct cold MEM completes1937–3328ms across three runs, with NTVDM CPU about
  1.61–1.78s; this includes cold worker bootstrap. Resident COMMAND produces
  two real MEM reports/parent returns. Observed547/515ms prompt cycles include
  input pacing/polling, not pure resident loading. Exact cold/resident EXEC
  boundaries remain open.
- WINMINE startup-only confirmation4046/4078/4031ms. Rough processCPU about4s
  includes startup plus another1s observation/cleanup; do not charge it all to
  startup or treat startup confirmation as exit. This investigation does not
  prove first usable UI, gameplay, steady throughput or loader/USER/GDI/wait
  attribution. These remain requirements here.
- WPR CPU policy could not be enabled (0xc5585011); host policy was unchanged.
  Thread/IP sampling is bounded, incomplete and perturbs scheduling. Requested
  10ms is not guaranteed100Hz; waiting IP counts cannot establish CPU/wait
  ownership. Initial sampler missed short native processes, not zeroCPU.
- A Window native-return case failed the existing contiguous-snapshot overlap
  assertion; no pass or diagnosed deadlock is claimed. Initial EDIT200 used an
  obsolete x86 test Hook and failed193 against x64 observer/frontend. The same
  test source rebuilt as x64 passed two200-event runs. Preserve failed evidence
  and select matching observers/test Hooks.
- No physical/RDP, large-history, complete optimized DOS/DPMI/WOW or stable
  p99 claim. Published ten-image hashes stayed equal to the sealed baseline;
  test processes were cleaned up and the research-owned W: removed.

## Evidence-driven priority at admission

After baseline refresh, carry the optimization-build and NTCON public-lock
findings into bounded repair first. Then compare deadlines/timer precision
with50Hz retained controls; audit hot atomic event reads separately; optimize
capture/raster/copy/transport where measured cost justifies it. Complete
cold/resident DOS EXEC and Win16 first-usable-UI/steady profiling rather than
closing them from COMMAND/MEM success or an arithmetic benchmark.

Review and promote necessary research sources into `tests/` at admission so
fresh checkouts can reproduce assertions. Build-only wrappers, diagnostic
binaries and temporary stub selections do not become production mechanisms
merely because they produced useful numbers.

## Initial hypotheses, not confirmed runtime causes

| Surface | Audit/measurement required |
| --- | --- |
| worker-base/publication.c | Measure actual rate-limit wait, pending replacements, successful send duration and idle work. The50Hz/20ms cap is one candidate, not the presumed root cause or a mandatory removal. |
| ntvwm-exe/main.c and presentation.c | Separate20ms hidden-Console acquisition from publisher delay; measure full-buffer reads, glyph/Unicode conversion, allocation/copy and viewport-size sensitivity. Changed rows are already compared at the producer; do not claim every capture sends every cell. |
| Input versus publication locks | Measure state/client lock wait and hold time, synchronous pipe round trips/final acknowledgements, batch size and backlog. Input threads are separate but may contend with capture/publication. Correct lock ownership must remain; removing necessary serialization is not a performance fix. |
| NTVDM DOS path | Measure input arrival through simulated keyboard/IRQ consumption, VGA change/capture/presentation, CCPU useful work versus waits, DOS file/load/environment/PIF cost and cold versus resident worker startup. Do not assume the emulator itself is responsible. |
| NTVDM WOW path | Measure loader/task-start and first usable UI separately; inspect actual guest execution, cooperative task scheduling, host USER/GDI/thunk/callback/file costs and avoidable project adaptation work. Original intentional waits and guest limitations require attribution, not blind deletion. |
| Native launch/Hook and monitoring | Quantify suspended creation, hook installation/width crossing and any finite reporting/authentication overhead in the delivered baseline. Observation must not be confused with execution completion. |

## Measurement contract

1. Freeze source/package/guest/configuration hashes, actual process width,
   hardware/host load, local desktop versus RDP, Console versus Window, geometry,
   font and buffer height. Keep cold and warm/resident starts separate. Run
   complete product scenarios serially because of the global BaseSrv endpoint;
   short-path mappings must use a verified free, research-owned drive, be
   recorded and removed in finally. Preserve other sessions' mappings; the
   initial study used W: because Z: was occupied.
2. Establish repeatable ordinary workloads: CMD and COMMAND single-character
   echo/continuous typing, VER/DIR/MEM, EDIT.COM typing/menu/mouse, modern
   EDIT.EXE control, native/DOS nested handoff and return; DOS CPU-bound and
   file/output-bound authored probes distinguish throughput from presentation.
   Actual Win16 WINMINE launch/UI and bounded interaction exercise WOW; SOL and
   WRITE retain independent known frontiers, never three interchangeable scores.
3. Timestamp true operation boundaries with monotonic time. Input injection
   success is not consumption; visible prompt is not generic startup success.
   Use invocation-specific result/state witnesses, real completions and actual
   UI milestones, without relying on discarded host scrollback history.
4. Report stage durations, CPU versus wall time, input queue/batch/backlog,
   lock contention, capture size/copies, publication/send/ack counts and volume.
   Provide median and suitably sampled tail distribution, sample counts and
   raw evidence. Startup, output throughput and input-to-present latency are
   separate results; do not add independent percentile values into a fake total.
5. Measurements are default-off, bounded/aggregate and outside hot per-opcode
   logging. Quantify instrumentation perturbation with enabled/disabled controls;
   use external sampling where useful without introducing a production observer.
   Failure, timeout and unavailable physical observation remain explicit.

## Proposed bounded phases

- Baseline/source audit: current release call/lock/wait and source-origin ledger,
  reproducible workloads, boundary timings and ranked bottlenecks. Report before
  committing to a producer, transport or emulator change.
- Text responsiveness repair: change only proved project-owned causes. Compare
  publication cap retained versus relaxed/removed under the same workload,
  latest-state behavior and assertions. Preserve FIFO input, final-frame
  barriers, title/geometry/cursor correctness and no frontend filtering. An
  uncapped mode that increases input contention/CPU/backlog is not a success.
- DOS/Win16 startup/execution repair: remove proved added overhead at its owner;
  audit original sources before proposing any mirror intrusion. Keep each actual
  guest path/host API/known frontier distinct and do not invent missing WOW
  functionality merely to make a performance benchmark pass.
- Integration and delivery: rerun matched before/after workloads and all affected
  semantic/negative/lifecycle and retained Console/Window/WOW/mixed-width gates;
  publish a coherent identity-checked package, review/commit/push and await owner
  acceptance under the normal task workflow.

## Source fidelity and acceptance

Apply [source policy](../etc/operations/policy/source-policy.md): unchanged guest
defects are recorded/unmodified; inherited original MVDM/OpenNT host defects
may use an already-existing matching SoftPC correction with minimum diff and
provenance, otherwise retain TODO. Project-added overhead/defects require a
designed repair. Do not replace original timers, VGA semantics, IRQ pacing or
WOW scheduling solely to increase a benchmark score. Any material source diff
or changed product timing contract requires explicit scoped admission.

Acceptance requires a numerical before/after account of each proved repair,
including its cost and remaining bottleneck. Do not promise a fixed latency or
universal smoothness before measurement; insufficient sample size/coverage is
not a passing tail-latency claim. Preserve typing/typeahead ordering, cursor
blink/shape, sustained output, mouse/keyboard fairness, mode/size transitions,
bidirectional handoff/final frame and teardown. No improvement may trade away
correctness, make a still-live target appear completed, or weaken existing
assertions to manufacture a result. Unresolved original/external limitations
are named separately from unimplemented project repairs.
