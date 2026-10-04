# T428 implementation plan

## Approved boundary

Owner approves implementation on 2026-10-04: simplify, remove duplication and
unify project-added mechanisms. NTVDM keeps original OpenNT execution and
lifetime semantics; NTVWM adapts to those semantics within its own boundary.
No additional relationship registry, process/helper, scheduler or observation
is admitted. NTSRV retains authoritative relationships and management policy.

The [S1 audit](../evidence/m0-t428-s1-worker-control-lifecycle-audit.md) is
concluded as source-only review. Its four choices are now resolved: existing
service records remain the authority; parent restoration leaves run16;
native residency follows the corresponding DOS/WOW session policy; management
shutdown uses the shared worker-base instruction with worker-local cleanup.
The former recommendation to keep GUI-only native ten-second retirement is
superseded by the owner's explicit rejection.

## Sequential bounded stages

| Stage | Scope and required result |
| --- | --- |
| S1 | Source audit and owner decisions; no runtime capability claim. |
| S2 | Remove the added ten-second retirement of admitted, unbound native carriers. Shared GUI residency matches shared WOW. Preserve frontend workerless grace, service empty grace and finite startup failure boundaries. Test with explicit policy-owner time and retained product gates. |
| S3 | Move eligible parent restoration out of run16's Console membership/worker selection. Prove authenticated parent identity in existing service records, exact completion and final I/O acknowledgment; preserve original DOS resume. |
| S4 | Common worker-base shutdown reception and service management contract for DOS/WOW/native, including native GUI-only registration. Preserve original close-handler ordering and actual Console closure proof. |
| S5 | Consolidate existing root-worker association, route cancellation, kind validation, occupancy and management projection; retain source-owned record readers and exclusive/shared policies. Implement native exclusive text/GUI retirement corresponding to original DOS/separate WOW without altering GUI startup-only return. |
| S6 | Ownership/format naming and final duplicate/caller sweep; integrated gates and owner handoff, T remains open until owner acceptance. |

Each code-bearing delivery requires affected x86 /MT CCPU40 builds, focused
positive/negative/lifetime evidence, retained Console17/Window17/WOW gates,
recoverable coherent system32 eight-file publication, reviewed commit and push.
Unrelated side-session planning remains preserved. Only CURRENT admits the
active stage; this table does not create simultaneous active packets.

## Source and sharing rules

Original cmdmisc DOS shared/independent/CloseOnExit and WOWEXEC shared/separate
rules stay in their mirrors. NTVWM implements corresponding local operations;
shared project-owned signaling, resource contracts and validation belong in
worker-base, while NTSRV/run16 retain their own common consumer paths.
Do not extract original task execution, scheduler, blocking or cleanup into
worker-base. GUI target lifetime, carrier residency, direct completion and I/O
lease return are separate boundaries.
