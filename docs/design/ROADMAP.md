# Roadmap

## Delivery principle

Recovery proceeds by complete original package and dependency closure, not by
the next trace hit. A phase selects source, proves its external interface
family, composes the smallest original-owner closure and then uses trace only
as integrated verification.

## Phase 0 — MVDM package rebootstrap

1. Create the thirteen production roots and canonical OpenNT/OpenNT-4.5
   package union.
2. Restore the Bochs mirror/mechanical boundary and complete load-only DOS/WOW
   guest mirrors.
3. Establish multi-instance-safe `session`, the three typed mapping-manager
   instances and the shared x86/x64 32-bit compatibility object space.
4. Establish the formal VDM-monitor interface family and cross-process
   `broker` contract.
5. Generate independent MSVC x86 and x64 Ninja graphs and prove package, guest
   and `src.old` isolation.

**Gate:** all rebootstrap proposal exit criteria pass. This phase establishes
source and interface closure, not general guest execution.

## Phase 1 — Low-dependency original owner packages

Select complete original MVDM packages whose machine, monitor, Win32 and
broker prerequisites are already closed. Compose their original translation
units through same-shaped adapters, run family-local positive/negative tests
and delete no historical failure semantics.

**Gate:** each selected package is code/ABI complete in both host graphs and
its unavailable branches are explicit.

## Phase 2 — Protected mode, Redirector, WOW and VDD dependencies

Recover DPMI, Redirector, WOW16 and VDD/debugger packages in dependency order.
Enable Bochs features only for reached original callers. Extend the broker only
for source-proven multi-process command/WOW coordination; do not recreate
CSRSS internals.

**Gate:** each complete owner package passes its declared local matrix before
one integrated guest trace is used to observe behavior change.

## Phase 3 — COMMAND and cross-family lifecycle

Complete original COMMAND child/return, DOS EXEC parent restoration, brokered
`GetNextVDMCommand`, console/event and WOW lifecycle after their owners are
available. Preserve one active imported MVDM session per process unless a
separate reentrancy audit admits in-process multiple contexts.

## Phase 4 — Integrated verification and packaging

Reconcile all BOP/dependency trackers, run original and source-built guest
image matrices, classify remaining defects by owner, and package x86/x64 host
products with immutable guest-image manifests. Trace-guided leaf repair begins
only after planned package code/ABI closure.

## Explicitly deferred

Reconstruction of NT4 kernel VDM or CSRSS, speculative in-process multiple
imported MVDM contexts, unrequested Bochs devices and compatibility claims
outside an admitted profile remain deferred.
