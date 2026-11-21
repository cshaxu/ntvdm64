# Proposal - Redundant queues and state-machine cleanup

## Status and owner objective

Owner request dated 2026-11-20: audit and repair unnecessary queues, state
machines and handoff layers, especially same-thread logical decoupling that
should use inward implementation ownership or an inner-defined callback.
This is a queued T candidate, not an admission or numeric T allocation.
[Queue](../states/QUEUE.md) owns its position; the active task is unchanged.

## Scope and evidence

Inventory production host components in the six architecture layers, including
shared libraries, compat, mirror integration boundaries, common, frontend-base,
worker-base and executable/DLL composition. Follow actual producer/consumer
chains and indirect callback targets. Search tools identify candidates; names
such as queue, dispatch or state alone do not prove redundancy.

Process component/file paths in stable directory order and keep one tracked
record per complete mechanism. Record:

- Source locations, callers, actual producer and consumer threads/processes,
  execution timing, and the reason work is deferred.
- State/resource authority, ownership transfer, ordering, cancellation,
  completion, error propagation, reentry, lock/thread affinity and shutdown.
- Existing queue, scheduler, state machine or provider that could already
  perform the responsibility, with evidence of duplication or necessity.
- Disposition: retain with evidence, move inward, replace with synchronous
  callback/direct call, merge with the existing owner, or delete unused code.
  Identify exact affected callers and tests; uncertain findings remain open.

Prioritize eliminating artificial asynchronous handoffs introduced solely to
avoid a source-layer dependency, forwarding queues that duplicate an existing
transport, multiple competing lifecycle authorities, redundant pending/running/
completed flags, and speculative retry/fallback state without a supported
behavioral requirement. Do not equate all state or all queues with defects.

## Implementation contract

Follow [Architecture Rules](../rules/ARCHITECTURE.md): same-thread logical
decoupling uses correctly placed code or a typed callback owned by the inner
component and registered by the outer composition. No new queue, event pump,
polling loop or worker thread may disguise that dependency.

A queue requires a real cross-thread/process handoff and a demonstrated need
for buffering/order/lifetime. Crossing a thread boundary alone does not prove
an extra queue is necessary; first reuse the existing scheduler/transport.
Do not introduce a queue merely because separate files or layers communicate.

The inner body must own its real preparation, validation and mechanism state;
a callback must not forward that whole body outward. Record callback registration,
borrowed-context lifetime, calling thread, error return and unregister/join.
Keep product policy and process/guest lifecycle authority with their actual owner.

Use one authoritative state machine per responsibility. Derived state must have
an explicit synchronization/invalidation rule and cannot independently decide
completion. Linearize or merge transitions only when the required behavior,
concurrency and ownership remain intact. Do not replace a necessary asynchronous
handoff with a blocking callback that introduces deadlock, reentry or CPU stalls.

Preserve error/completion ordering, exactly-once ownership transfer, cancellation,
shutdown and existing product/guest contracts. Avoid new fallback paths and
unbounded recovery. Do not modify or rebuild original guest binaries. Mirror
changes retain #error/#line and follow the existing non-error diff approvals;
prefer reducing both diff points and lines over relocating equivalent complexity.

## Proposed stages and verification

1. Establish the finite mechanism inventory and full-chain evidence, grouped
   into same-thread indirection, redundant cross-thread handoff and duplicate
   state ownership. Report audited, repaired, retained and unresolved separately.
2. Repair proven same-thread cases by inward relocation or typed callbacks.
   Complete each ownership chain, including callers, cleanup and tests.
3. Consolidate redundant handoffs and state machines with one authority and
   retained supported concurrency; remove superseded providers and wrappers.
4. Recheck production build/link providers and remaining callbacks. Record
   queue/state/transition counts and code/mirror diff changes before and after.

For each changed mechanism, verify the real calling thread, synchronous versus
asynchronous result contract, ordering/reentry, failure rollback, cancellation
and shutdown as applicable. Use existing focused tests and isolated integration
runners; do not create a duplicate test framework or merely mirror the code.
Complete cached full builds, coherent dual publication, affected targeted tests,
documentation gates, commit and push under the delivery rules at admission.
Every retained mechanism needs a concrete responsibility and evidence; every
removed mechanism needs final-code verification, not just a shorter implementation.
