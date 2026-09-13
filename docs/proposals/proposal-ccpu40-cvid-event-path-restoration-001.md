# CCPU40 C-VID and event-path research and restoration

## Status and objective

On 2026-09-12 the owner requested this unadmitted candidate immediately before
[broker reconstruction](proposal-cross-process-broker-closure-001.md) in
[Queue](../states/QUEUE.md). Continue U01 research to establish a genuinely
CCPU40-compatible original C-VID binding and event-timing path, then restore
the proved original composition and retire superseded autonomous policy.
This proposal does not allocate a numeric T, admit implementation now or
declare the candidate path buildable. Reconcile repair ownership with T405
at admission; preserve its open findings and one-active-S rule.

## Read-only research baseline

The 2026-09-12 investigation used bounded source reads and symbol/build-rule
searches in the selected local OpenNT and OpenNT-4.5 SoftPC trees, current
mirrors, overlays and build generator. No preprocessed graph, compilation,
link, guest execution or repair was performed. The
[U01 audit](../etc/evidence/opennt-replacement-audit-20260912.md#c-vid-binding-generator-semantic-reconciliation)
remains a starting point; the observations below refine its missing-provider
hypothesis, not proof of a runtime defect.

Original inputs under `O:\repos.external\OpenNT\base\mvdm\softpc.new`:

| Relative input | SHA-256 |
| --- | --- |
| base/system/quick_ev.c | FE23E0EF2FB79AEBADC5C0404A2369A694AC6D82347BCAE5F2A7BCA74B807111 |
| base/system/qevnt.c | 3E0D97D17BCC4C107B5173BF38868C79AB68D3042D29472A1A7259364DA2947C |
| base/cvidc/cpu4gen.h | C7775BB2A65B0C398B3D326B17BBC183526B7BA19736942F0DB5D77DD6625127 |

Revalidate selected upstream provenance and all additional inputs at admission.
External trees remain read-only; retained NTVDMx64 material is comparative
evidence here, not authority to substitute a new timing provider.

## Established source observations

1. Original `base/cvidc/cpu4gen.h` maps CCPU calls such as cpu_simulate and
   host_q_ev_set_count directly to c_cpu_* providers; non-CCPU branches use
   vector calls or external access wrappers. Original `ccpu386/c_main.c`
   supplies heartbeat set/get and time-to-count support. A full vector table
   is therefore not automatically necessary for every CCPU caller.
2. Current `tools/build/New-T310OriginalSoftpcNinja.ps1` removes CCPU and PROD
   specifically from accessfn.c to expose non-CCPU vector wrappers to external
   consumers. Current GenerateCvidcCpuBinding.mjs supplies provider choices,
   including duplicate object keys whose later null entries override earlier
   names. Determine why each actual consumer needs that route; do not blindly
   delete wrappers or populate every null slot.
3. Original quick_ev.c init_q_ratio explicitly disables quick-event
   recalibration for CCPU. Its initial time/count conversions use time / 10
   and inst * 10, labelled CCPU style. It also contains NTVDM/non-MONITOR ICA
   locking around event operations. This is concrete original CCPU-specific
   behavior, not a proposal to invent a new calibration algorithm.
4. NT-specific qevnt.c instead obtains Instruction Jump Count calibration
   samples and accumulates them against host time. Its source manifest selects
   that file. Current overlay returns one mutable restart value, initially
   100, for both calibration and restart queries. This is not evidence of an
   executed-instruction activity counter or restored original calibration.
5. Original CCPU sources select ntstubs.c, containing old-shaped a3_cpu_* to
   CCPU calls. Current build excludes it because of duplicate FPU/SAS bodies,
   selecting other carriers. Original CCPU/C-VID ccpudefs.inc defines both
   CPU_30_STYLE and CPU_40_STYLE with CCPU. This is historical interface-selection
   evidence, not permission to enable the retired CPU30/MONITOR/V86 product
   path. Identify useful original forwarding bodies independently of macros.
6. Original setup_vga_globals initializes video globals/ring state; this
   investigation did not establish the original generation rule or named
   missing assembly unit claimed by the current binder rationale. Do not
   present that missing-unit explanation as proved. Generated metadata does
   not prove locally chosen providers are original selections.

## Candidate path and open closure questions

Investigate original CCPU direct interfaces and heartbeat with quick_ev.c's
CCPU event policy, retaining original C-VID generated rules, ev_glue and video
state. Keep only demonstrated external wrapper/vector bindings. This is the
preferred source-backed candidate, not a decision to swap quick_ev.c for
qevnt.c immediately or delete the whole binder.

Required research before selecting the restoration:

- Recover actual per-translation-unit macros, include order and selected
  symbols for CCPU, C-VID, host and DPMI consumers; distinguish intended
  generator behavior from the actual retained/formal build graph.
- Compare complete quick_ev.c and qevnt.c APIs: handle representation and
  cancellation, ordering, time/count units, rounding/overflow, initialization,
  reset, callbacks, reentry, locks and threading. Verify original heartbeat
  decrement/dispatch cadence matches the chosen conversion contract.
- Account for remaining timestamp/calibration symbol references even when
  runtime recalibration is disabled. Runtime-unreachable calls may still be
  link dependencies; no fake calibration or recursive self-forwarding stubs.
- Classify every selected public/private vector slot and real caller, including
  null-slot reachability and callback ABI. Resolve Cpu/Sas/Video/Gdp ownership,
  generated layout variants, duplicate definitions and BIOS argument widths.
- Search original build/generator inputs and artifacts before claiming source
  loss. A file missing from the current graph is not a missing original
  algorithm. Existing ntstubs or other bodies may be selected only as audited
  original slices, not bulk-linked to introduce duplicate executor state.

## Proposed sequential stages

1. Research and freeze the original CCPU40 composition and complete outgoing
   boundary; report confidence and unresolved facts before choosing a repair.
2. Restore the proved interface/global-state and C-VID binding composition,
   removing superseded generator/overlay policy with per-slot evidence.
3. Restore the proved original event path as a coherent profile, not a tweak
   to the number 100; retain necessary NT host synchronization through bounded
   original-shaped bindings.
4. Verify the whole profile and reconcile U01, residual diffs and limitations.

Original algorithms remain in mvdm-host; only separately admitted non-MVDM
owners enter opennt-host. Keep CCPU40 as the sole executor. No CPU instruction
semantic edits, CPU30/MONITOR runtime selection, second emulator, broker work
or generic event framework is authorized by this proposal. Follow the source
policy recovery ladder and CPU compatibility profile gate at admission.

## Acceptance

### U07-T and U08-W worker-local ownership

This candidate owns U07-T: participating thread bind/unbind, event-thread
resource lifetime and worker-local session termination, including the recorded
cross-thread nonlocal-return risk. Verify termination on the owning thread,
unarmed/wrong-thread requests, pending callbacks and shutdown races; retain
original event policy with finite mechanics, not a new session framework.
File/pipe buffers remain U07-F in the DOS file proposal; command transport
remains broker-owned. Shared memory leases remain U07-M in DPMI/XMS.

U08-W covers shared worker-local native wait/alert bindings and all their
selected callers. Compare alertable/nonalertable results, timeout, duplicated
handle identity, wake-before-wait, wait/close races and failure propagation
against original contracts. Separate source-owned policy from unavailable
kernel mechanics; no successful empty stub. Base protocol waits and WOW
cooperative scheduling remain with their own proposals. If another candidate
needs this shared contract earlier, explicitly promote the finite prerequisite
under Queue's dependency-first rule rather than implement a parallel wrapper.

For each changed group retain original provenance, current/original difference,
reason, exact restoration and superseded-code disposition. Prove repeatable
generation, unique slot decisions, compatible signatures/layouts, one owner
per shared state, and no reachable null or recursive vector call. Verify event
add/cancel/dispatch, equal/deferred deadlines, callback reentry, timer/CPU
concurrency, initialization/reset, conversion boundaries and cleanup.

Compile/link the actual x86 CCPU40 profile and run focused original-provider
tests before integrated COMMAND/EDIT input/display and selected video-memory
mode tests. Preserve the accepted Console mouse experience. DOSX/WOW progress
may be observed but cannot prove causality or substitute for profile evidence.
No source-only claim of full timing correctness or promise to delete the whole
overlay. If the candidate cannot close, name the precise missing dependency
and earlier recovery rungs before proposing any alternative.

### T407 platform-carrier receiver

T407 S6 transfers CPU40 TR/TSS and table-register lifecycle here. Before
changing that carrier, prove original kernel ownership and why its task
initializer cannot compose in the standalone worker. Test valid TR and I/O
bitmap behavior, failure, reset and worker teardown using only guest values;
a no-hit source scan or successful MEM run is not acceptance. Do not introduce
a kernel scheduler, trap layer, generic guest-pointer manager or task policy.
See [T407 S6 evidence](../etc/evidence/m0-t407-s6-platform-carrier-disposition.md).

Builds/intermediates stay in build/, current approved publication follows
ntvdm32.exe and runtime observations stay in `O:\ntvdm64\logs\`. Run
documentation governance, link and diff checks. Unresolved mandatory behavior
requires explicit owner disposition; it is not an accepted restoration.
