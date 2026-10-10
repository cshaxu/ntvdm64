# Proposal — Win3.1 interactive performance root-cause recovery

## Status and owner request

This is a queue-tail, unnumbered candidate T.  It preserves the result of the
paused `M0 T440` investigation rather than treating partial optimizations as a
solution.  Its purpose is to identify the actual reason that an installed
Windows 3.1 Standard desktop remains visibly sluggish while the read-only
SoftPC comparison is substantially smoother.  It does not admit a numeric T,
change the active packet, or authorize a product change.

The owner directs that correct, independently useful optimizations remain in
the baseline.  Only an experiment that changes guest time semantics without a
verified product benefit is a rollback candidate.  In particular, the global
CCPU pacing experiment is not a demonstrated responsiveness repair; the
variable bounded worker record transport and source-side publication work are
not to be reverted merely because they were not the final bottleneck.

## Objective

Produce one reproducible, active Win3.1 interaction trace that separates:

1. host raw/window input acceptance and NTCON coalescing;
2. worker pipe delivery and NTVDM input consumption;
3. NTVDM queue residence, original mouse IRQ/guest callback work, and CCPU
   execution;
4. VGA invalidation, capture, publication, transport, NTCON decode and
   presentation; and
5. actual visible interaction latency where a trustworthy local or RDP witness
   is available.

The candidate then repairs only the measured, project-owned bottleneck.  It
must distinguish a project adapter fault from an inherited OpenNT/MVDM behavior
and from a deliberately different SoftPC execution architecture.

## Frozen findings carried from M0 T440

| Finding | Evidence and consequence |
| --- | --- |
| Static Win3.1 desktop is not saturating the frame pipe or presenter. | A 5.016 s worker trace recorded 7 DIB copies, 9 publisher signals and 4 graphics invalidations. Ordinary exchange calls were about 80–200 us. This excludes static 50 Hz publication/1 MiB records as the proven cause; it does not measure movement. |
| Current native compilation is optimized. | The selected graph uses MSVC `/O2`, the supported release-speed option. Ninja incremental operation is normal. Build configuration is not a substitute for a runtime cause. |
| The worker is executing guest/JIT code while the retained desktop is slow. | IP samples predominantly landed in CCPU translated-code allocations. The current idle observation saw `IdleDisabledFromPIF=0`, but `ienabled=0`, `IdleNoActivity=0`, `NowWaiting=0`; no original idle wait was reached. |
| Global CCPU pacing is not an accepted fix. | It reduced CPU consumption but the owner observed no meaningful desktop responsiveness improvement. It must not be made a default timing policy solely for this symptom. |
| There is a possible input-pressure mechanism, not a proven root cause. | Project bridge queue policy retains up to 17 relative samples before merging; original mouse EOI work schedules roughly 10 ms. In a true unmerged burst this could replay old movement. NTCON already coalesces raw input, and no active Win3.1 trace has shown a high water mark or IRQ debt that proves this mechanism is responsible. |
| The comparison product differs upstream of rendering. | SoftPC's in-process mouse route reaches its virtual hardware directly; this product uses an authenticated frontend/worker path plus NTVDM bridge. Do not transplant SoftPC lifecycle or scheduler code simply to match a benchmark. |

The retained evidence records are:

- `docs/etc/evidence/m0-t440-s1-publication-timing-baseline.md`
- `docs/etc/evidence/m0-t440-s2-graphics-chain-audit.md`
- `docs/etc/evidence/m0-t440-s3-source-side-graphics-publication.md`
- `docs/etc/evidence/m0-t440-s4-window-performance-attribution.md`
- `docs/etc/evidence/m0-t440-s5-variable-worker-io.md`
- `docs/etc/evidence/m0-t440-s6-post-s5-responsiveness-attribution.md`
- `docs/etc/evidence/m0-t440-s7-ccpu-hlt-event-wait.md`
- `docs/etc/evidence/m0-t440-s8-win31-xms-attribution.md`

Numbers in those records are historical inputs, not a claim that the current
runtime has a measured end-to-end Win3.1 latency distribution.

## Required diagnostic repair before any product hypothesis

The existing test-only `worker_performance` wrappers can record queue high
water, enqueue/merge/consume lifetime, post-IRQ debt, graphics invalidation,
publisher signal, DIB capture and transfer.  The last attempted isolated run
did not start: `measure-worker-boundaries.ps1` used `Get-CimInstance` as a
preflight and failed under the current client permission boundary.  That is a
test-tool defect, not a product observation.

The first S must make the probe runnable without CIM, using a bounded ordinary
Win32 process-existence check or a recorded permitted process query.  It must
not use CUA, inject desktop input, replace the published product package, or
turn instrumentation into a production feature.  The trace must be written
under `build/`, have a pinned worker/frontend/guest/configuration identity,
and prove its instrumentation is enabled only for the measured process.

The workload must use either:

- an existing owned automated continuous-input probe that reaches the same
  NTCON → worker → original mouse path, with a separate proof that it does;
  or
- one bounded owner physical Win3.1 movement witness, with the worker and
  frontend diagnostic binaries in an isolated package.

An input-post success, a synthetic Console event, or a static desktop snapshot
is not sufficient evidence for physical-motion latency.

## Candidate S breakdown

### S1 — Repeatable active-interaction measurement closure

Repair the failed measurement preflight and build only the smallest test-only
worker/frontend wrappers needed for a single active Win3.1 Standard trace.
Record wall/CPU samples, queue high-water, queue time, IRQ debt, video source
rate, send/ack, decode/present and trace overflow.  Run enabled/disabled
controls to bound observer cost.  Do not alter production code.

### S2 — Root-cause attribution and source-fidelity ledger

Rank measured stages by wall-time/backlog rather than summing overlapping
percentiles.  For the dominant stage, classify in this order: directly
composable original OpenNT/MVDM owner; smallest source-shaped facade; already
documented SoftPC correction; or a narrowly registered project implementation.
Explicitly reject unrelated frame, timer, scheduler, guest/PIF or transport
changes.  If no stage dominates, close the candidate as an evidence gap rather
than manufacturing an optimization.

### S3 — Minimal measured repair, only if S2 proves one

Implement the selected repair at its real owner, preserving original IRQ,
VGA, CCPU and lifecycle semantics.  Retain already-correct S5/S7/S8
improvements.  A global timing/pacing policy, a new helper/thread, protocol
change, guest-media modification or SoftPC executor import needs separate
owner approval and a new admission.

### S4 — Matched verification and delivery

Use the identical pinned workload and instrumentation to show a numerical
before/after change at the repaired stage and no new input loss, ordering,
frame, cursor, final-drain, DOS/Win16 handoff or lifecycle regression.  Build,
publish only a coherent verified package if production code changed, then
record remaining physical/RDP limits explicitly.

## Non-goals and guardrails

- Do not weaken product tests, shorten guest waits, or make host scrollback a
  correctness proxy.
- Do not remove the 50 Hz publication policy, change 1 MiB bounded records,
  or deduplicate NTCON rendering merely because a different stage is slow.
- Do not modify immutable guest media, PIF behavior, Windows 3.1 binaries or
  the owner's desktop.
- Do not add a poller, helper, second executor or independent scheduler.
- Do not claim that CCPU instruction throughput, static rendering cost or CPU
  occupancy alone explains user-visible smoothness.
- Preserve the source-first recovery audit and the existing NTSRV lifecycle /
  worker-neutral NTCON architecture.

## Completion conditions

The task closes only with either (a) an evidence-backed root cause and matched
repair, or (b) a documented inability to form a representative active witness
after the diagnostic path itself is proved, with no speculative production
change.  A valid repair requires repeated, pinned before/after measurements,
focused semantic tests, relevant Console/Window/Win16 lifecycle checks and
owner-facing Win3.1 confirmation.  It must name any remaining external or
inherited limitation separately from project-owned debt.
