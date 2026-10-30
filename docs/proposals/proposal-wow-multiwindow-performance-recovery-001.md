# Proposal — WOW multi-window performance recovery

## Status and owner request

On 2026-10-29 the owner directed that further SOL Options and related WOW
multi-window performance optimization stop in T473 and be queued as a new T.
The owner subsequently moved it directly after Win3.1 interactive performance.
This unnumbered proposal is not an active task. Admit it only after the
preceding candidates under the normal governance rules.

## Problem and retained evidence

Win16 programs are responsive with only their main window, but opening or
closing additional windows can take seconds. The owner observed this in SOL
Options, Notepad Find, Program Manager New Group and Winmine. Current isolated
SOL probes still show approximately two seconds or more per dialog transition.
The existing investigation is
[S11 dialog performance](../../artifacts/mvdm-audit/s11-dialog-performance/investigation.md);
the exact package identities, timings, negative controls and rejected
experiments are linked there and in the S11 foundation evidence.

Recorded observations implicate repeated project-owned window graph
acquisition and publication but do not identify a complete end-to-end cost
or justify bypassing callbacks, identity checks or synchronization. The
current reference-scan A/B comparison found no clear end-to-end improvement;
its reduced scan complexity is separately documented
[here](../../artifacts/mvdm-audit/s11-foundation-evidence/atomic-task-retire/execution-check/current-record-reference/review.md).
Do not conflate this investigation with the queued full Win3.1 desktop input
and graphics performance task: workloads and ownership chains differ, though
they may share evidence if measurement establishes a common cause.

## Scope and acceptance

1. Rebind the reproduced workload, installed/isolated package identities and
   current source to admission-time evidence. Measure application dialog
   open and close latency, CPU time and the actual hot paths with bounded
   observer overhead and comparable baseline/candidate conditions.
2. Trace the dominant cost through original WOW behavior, project adapters,
   native Windows calls and execution ownership. Keep original scheduling,
   callbacks, message and object identities, nested reentry, retirement,
   rollback and the approved external-window snapshot boundary intact.
3. Implement only justified, maintainable improvements. Respect the owner's
   rule: an optimization should reduce code, elapsed time, memory, or improve
   algorithmic complexity without a large net code increase; do not claim a
   user-visible speedup from a microbenchmark or unpaired measurements.
4. Validate normal, failure, nested, concurrent and destruction paths, and
   compare the same multi-window workloads across SOL and other affected WOW
   applications. Preserve immutable guest inputs, source policy and mirror
   exception registration. Run the applicable full product gates before
   publication.

T473 retains its original execution, identity and lifecycle correctness
obligations and its full verification requirements. In particular, the
separately observed `publish-native-return` error 1400 is not declared fixed,
waived, or reclassified as a performance issue by this handoff. Existing
independently justified and verified worktree changes remain subject to normal
T473 review; neither this proposal nor the handoff authorizes a rollback.
