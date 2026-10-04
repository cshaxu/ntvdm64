# Project Status

## Current Work

**Active: M0 T429 S1** (Ordinary Mode).

Owner directs “先收口t任务吧 准入下一个” on 2026-10-04. T428 is
owner-closed at production P 68e860553 and status P b1b77e129; its
[closure](../history/m0-t428-worker-interface-unification-closure.md)
retains all S1-S6 evidence and disclosed limits. The actual queue head,
NTVDM execution-performance recovery, is admitted as T429. Only S1 is active.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T429 S1; Ordinary Mode. |
| Admission And Approval | Owner accepts T428 closure and admits the next queue-head package on 2026-10-04. Admit bounded baseline/source/performance measurement first; no speculative repair. |
| Candidate Proposal | [NTVDM execution-performance recovery](../proposals/proposal-ntvdm-execution-performance-recovery-001.md). |
| Objective | Freeze source/build/runtime identities and establish reproducible EDIT startup/input/presentation timing and queue/batch evidence; identify actual hot-path costs before repair. |
| Non-goals | No guest/firmware mutation, CCPU algorithm rewrite, CPU30, helper, new component/channel/scheduler/registry, lifecycle policy or wire redesign; no receiver/worker-base display dedup. No S2-S4 repair is claimed by admission. |
| Reference Baseline | main/origin b1b77e129, production 68e860553. Verified/published eight-file set build/M0-T428/S6/r002/runtime/system32; S6/r007 Product manifest, S6/r011 recovery/publication and S6/r012 smoke. MSVC14.43/SDK22621/x86 /MT CCPU40; APP0.0.427/RPC38/I/O25. |
| Files And ABI Surface | Read selected c_main.c and ntvdm-exe SoftPC/input/video adapters, NTCON queue/presentation and existing measurement fixtures. S1 may add bounded test/measurement support with disabled-by-default aggregate diagnostics outside the instruction loop. No cross-EXE wire change. |
| Applicable Rules | Full AGENTS reading set, source policy, original mirror/guest ownership, project-private resource/locking boundaries, build-only outputs, serial global endpoint and Z:-only mapping. |
| Verification | Check eight baseline hashes and build selection; pin guest/workload/input/geometry/context/warm-up/iteration identity. Measure startup and boundary latency distributions with counts/order assertions and an unaffected control. Validate measurement negatives and disabled-mode behavior. Any production-code P additionally requires affected x86 build, Console17/Window17/retained WOW/lifecycle, coherent recovery/publication and deployed smoke. |
| Expected Markers | Evidence distinguishes raw input receipt, frontend delivery, worker read, mouse submission/IRQ consumption, text publication/transfer/present; reports medians/tails/queue counts without treating submitted input as consumed. Per-decode trace hypothesis is source-confirmed, not yet a measured causal conclusion. |
| Asset Needs | Existing validated build/M0-T427/S2/r001 cache and sealed S6 runtime/guest inputs; all new disposable outputs under build/M0-T429/S1. SoftPC is comparison evidence only, never a build/runtime dependency. |
| Reporting Requirements | Report precise inputs, instrumentation cost, source versus measured findings, uncertainty, failed attempts and smallest justified next repair. Physical/RDP tests unavailable or owner-waived remain non-pass observations. |
| Stop Conditions | Need for guest/core semantics, new wire/lifecycle/helper or an unapproved comparison dependency pauses work for renewed admission. Nonreproducible or contrary evidence is recorded, not converted into a speed claim. |
| Exit Criteria | Reproducible baseline has startup/latency distributions and queue/batch counts, control case and instrumentation overhead/disabled-path proof. Source/diff/governance review and reviewed P delivery apply. S2 requires sequential admission after S1; T acceptance remains owner-controlled. |
| Original Owner Request | Close the current worker-interface-unification T and admit the next queued task; improve DOS performance through evidence-backed project hot-path/input/producer changes while preserving OpenNT semantics. |
| Similar-Issue Sweep | Per-instruction observer calls versus coarse diagnostics, frontend/worker queue backlog versus original IRQ timing, surplus producer work versus explicit frame delivery, cold-start/geometry/RDP differences versus genuine implementation cost. |

## Admission State

S1 initial measurement is recorded in
[performance baseline](../etc/evidence/m0-t429-s1-performance-baseline.md):
optional buffered test-observer timing, 42 real serial COMMAND/EDIT/native-control
samples and two missing-marker negatives pass; the actual disabled diagnostic
provider has a separately measured isolated cost. The ordinary fixture's
observed Console is 120x30, not an assumed 80 columns. Production and published
eight-file hashes remain unchanged; no speedup is claimed. Test-only measured
worker links now exercise the actual 200-input guest path: three measured runs
plus a disabled control pass, with 205 records accepted, pressure coalescing,
queue high-water17 and no rejection/overflow. Queue consumption, read batches,
video transaction and final handoff/close acknowledgment are measured; bounded
sample-overflow and disabled/last-error unit checks pass. Actual EDIT now passes
200 continuous movements, menu exit and MEM with separate producer/transfer/
frontend timing in three measured runs plus disabled control. Queue conservation,
frontend completion and instrumentation-cost units pass; timings locate a much
larger synchronous transfer cost than text assembly, without yet isolating its
cause. Physical RawInput, matched SoftPC runtime comparison and full-product
perturbation bounds remain unproved. S1's final disposition remains open.

The proposal supplies the bounded S1-S4 sequence: baseline measurement;
release hot-loop diagnostic removal; only proven input/producer repair;
integration and owner handoff. Only this S1 packet authorizes current work.
Existing 30ms native presentation polling is not a new repair target.

## Current Technical Baseline

O:/winnt/system32 contains run16.exe, ntsrv.exe, ntcon.exe, ntvdm.exe,
ntvwm.exe, ntmon.exe, WOW32.DLL and VDMREDIR.DLL. All eight match the final
T428 S6 tested package. Recovery is build/M0-T428/S6/r011/recovery.
[S6 evidence](../etc/evidence/m0-t428-s6-ownership-naming.md) includes the
whole-T requirement audit, 29 service cases, 37 shutdown assertions, 34
sequential leases, Console17/Window17/WOW, six GUI cases, loss/isolation
and published smoke/hash proof.

NTSRV remains the sole relationship/lifecycle authority; original DOS/WOW
execution, records, blocking/resume and cleanup stay in their mirrors.
Default shared GUI carriers reside/reuse; no exclusive GUI option was added.
run16 parent restoration no longer discovers the worker using Console members.
NTCON owns presentation; common and worker-base retain their bounded mechanisms.

Each EXE derives the product Windows root from its own system32 parent.
Original media/configuration retain declared relative paths; SYSTEM.INI is
at root and NTVDM.REG beside the worker. User applications use ordinary
CWD/PATH or explicit paths. The owner-supplied Windows 3.1 applications remain
at O:/winnt. Guest/configuration and running processes are unchanged by this
closure/admission. Physical RDP/focus, broader WOW usability and host scrollback
retain their disclosed limits; admission does not upgrade these to passes.

Other-session planning is preserved. Queue's admitted performance candidate
is removed while later relative order is retained; its existing proposal is
now the T429 brief's source. Unrelated worker-proposal chronology remains separate.

## Recent M0 Closures

| Task | Outcome and retained evidence |
| --- | --- |
| T428 | Owner-directed closure, worker interface/association/lifecycle/naming unification. Production 68e860553; status b1b77e129. [Closure](../history/m0-t428-worker-interface-unification-closure.md). |
| T427 | Owner-accepted root/search isolation and system32 host co-location; S5 62a71fd90. [Closure](../history/m0-t427-system-root-search-isolation-closure.md). |
| T426 | Owner-accepted monitor tree, S4 700b3d862. [Closure](../history/m0-t426-console-root-monitor-tree-closure.md). |
| T425 | Owner-directed closure after S11 typeahead proof. [Closure](../history/m0-t425-worker-neutral-frontend-closure.md). |
| T424 | Owner-closed at audited ff50ff588, production S12 46abdb554. [Closure](../history/m0-t424-component-control-closure.md). |
| T423 | Owner-accepted S40 f64559086. [Closure](../history/m0-t423-console-window-runtime-closure.md). |

## Recent Governance

Owner closes T428 and admits T429 on 2026-10-04. Closed chronology resides in
history and indexed S evidence; CURRENT has only the T429 S1 packet. This
documentation-only delivery runs governance, relative-link and diff checks;
it neither rebuilds nor replaces the accepted runtime.
