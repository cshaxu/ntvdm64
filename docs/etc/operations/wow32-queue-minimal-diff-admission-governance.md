# WOW32 queue minimum-diff admission governance

## Role and authority

This operational record implements the owner-requested queue audit. It is not
an alternate queue, task admission, runtime closure record, or replacement for
any candidate proposal. [Queue](../../states/QUEUE.md) remains the sole ordered
candidate authority; [Current](../../states/CURRENT.md) remains the sole active
packet authority. A candidate may become active only through the normal
admission process.

The source-contract baseline is the separately managed OpenNT-only
WOW32/NTVDM audit, whose management entry is `management/README.md` and whose
generated status is `management/BASELINE-STATUS.md`. That ledger distinguishes
entries, callsites, shared structures, protocols, mechanisms and build
variants. Its static audit closure does not certify a linked image, this
product implementation, or a runtime result. Candidate evidence must make
those claims separately.

## Common admission rule

Before an admitted WOW32 candidate changes production code, its first bounded
review must record the affected original OpenNT owner, source path and build
condition; the exact Guest/WOW32/modern-host ABI boundary; the selected
modern API or explicitly unavailable capability; ownership of pointers,
handles, buffers, callbacks and teardown; and a normal, failure and retirement
proof entrypoint. A name match, existing export, report count, static ledger
row, successful link, or previous application startup is not enough.

The implementation chooses the smallest valid form in this order:

1. reuse the selected original body without a behavioral mirror diff;
2. use a small registered binding at the original owner;
3. place a material new mechanism in its named specialist adapter/ABI owner;
4. retain an explicit unavailable result when no source-shaped modern binding
   exists.

No candidate may create a second USER queue, scheduler, task table, window or
class authority, message ownership registry, Guest-memory mapping service, or
callback-lifetime manager. Native USER owns public HWND and public state;
page-domain projection is the sole original-layout view; original WOW32 owns
its selected conversion, dispatch, cleanup and failure algorithms. A raw
native pointer, HANDLE, FARPROC or OS-thread context may not cross a fixed
Guest/WOW ABI or survive an asynchronous boundary.

Every new mirror difference has one new, stable `DIVERGENCE(...)` identity,
an adjacent reason, and one register row. An identity may not be reused for
unrelated behavior. Material project code belongs in a named adapter, not in
an original mirror body. A candidate must not claim a whole family complete
from a lexical count or from a happy-path application run.

## Queue lanes and admission gates

The lanes below are implementation gates, not a reordering of Queue. Green
candidates may proceed after their candidate-local owner review. Yellow
candidates may proceed only after the named boundary review closes for the
changed path. Red candidates may inventory and design, but may not add a new
queue, callback, task, object-lifetime or teardown mechanism until they reuse
an already-proved W01/W02 boundary or a newly admitted bounded repair. The two
acceptance candidates consume evidence only; they do not absorb implementation.

| Queue candidate | Lane | Required gate |
| --- | --- | --- |
| W03 modules, memory and aliases | Yellow | Use the existing allocation/alias owner; no new mapping, heap or task-retirement owner. |
| W04 files, environment and OEM | Green | Trace any actual W03 dependency; preserve original OEM conversion and rollback. |
| W05 resource loading | Yellow | Reuse W03 aliases and record load/lock/free ownership before a consumer change. |
| W07 GDI/DC/drawing | Yellow | Reuse one GDI identity carrier; do not invent generation/type policy. |
| W08 fonts/text/metafiles | Yellow | Separate host font discovery from Guest product-root lookup and reuse W04/W07 owners. |
| W06 dialogs/input/hooks/timers | Red | Prove callback admission, task retirement and native input ownership; no second router or scheduler. |
| W09 menus/resource conversion | Yellow | Reuse T447 MENU-01/object graph; do not add another graph, mutation or retirement owner. |
| W18 common dialogs/OLE | Red | Reuse W01/W02/W06 callback, resource and cancellation boundaries before implementation. |
| W10 clipboard | Red | Prove delayed-render, owner-replacement and task-loss rundown through the existing lifecycle owner. |
| W12 shell/layered Registry | Green | Reuse the one NTVDM.REG owner; never write system Registry or create a parallel provider. |
| window-drawing residue | Yellow | Trace the actual painter/client boundary before any visual repair; no masking. |
| everyday three-application usability | Acceptance | Consume completed owners only; return every defect to its named owner. |
| W19 hard errors | Red | Reuse original response and hung-task/worker tail; do not add another terminator. |
| W11 DDE | Red | Prove message/reply/payload ownership and peer-death cleanup on the existing W01 path. |
| W16 sound/multimedia | Red | Prove completion/cancel callback admission and device/task rundown before async work. |
| W13 Winsock | Red | Prove async notification, unhook and peer-loss cleanup without a new callback carrier. |
| W15 printing/spool | Yellow | Reuse W07/W08 job/DC lifetime and report unavailable endpoints as non-passes. |
| W14 COMM | Red | Prove event/cancel/close/task-loss lifetime before pending-operation support. |
| W17 ToolHelp/debugger | Red | Reuse W01/W02 task identity and notification retirement; no second task terminator. |
| W20 integrated acceptance | Acceptance | Reconcile owner evidence only; no temporary provider, test-only semantic substitute or hidden repair. |

## Required handoff and closure

A candidate that discovers work outside its defined family records the original
owner, exact edge, confidence and why the current path cannot own it, then
hands it to the earliest existing queue candidate in this table. It does not
create a new numbered subpart, silently enlarge its scope, or defer required
cleanup to W20.

At each candidate closure, review every touched original mirror and adapter
against this record and the external ledger; reconcile new or changed
DIVERGENCE identities; state the applicable OpenNT build condition; and report
separately static source evidence, build/image evidence, runtime evidence and
remaining limits. This is the minimum-diff audit required before advancing to
the next candidate.
