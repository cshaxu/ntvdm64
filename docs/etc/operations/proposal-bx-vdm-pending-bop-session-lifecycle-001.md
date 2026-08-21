# Candidate S Design — bx-vdm Pending BOP Session Lifecycle

## Status And Purpose

This design is admitted as the reopening scope for M0 T236 S2. Its purpose is
to prevent the current synchronous typed-BOP transaction from becoming a
permanent COMMAND/DEM architectural constraint. The previous S2 closure is
P1 source-admission evidence, not this lifecycle's closure.

The target is a bounded `bx-vdm` session lifecycle which can represent a BOP
whose OpenNT-owned host operation completes later.  It is the modern
composition counterpart of the historical CCPU worker lifecycle.  Bochs stays
the SoftPC-equivalent guest-machine engine; it does not acquire COMMAND, BOP,
Win32 child, or host-worker meaning.

## Intended Contract

The typed BOP outcome gains one explicit lifecycle disposition in addition to
ordinary completed and failed results:

| Disposition | Meaning | Guest-machine action |
| --- | --- | --- |
| `COMPLETED` | Provider produced a checked guest/CPU result. | Apply result, then resume or stop as declared. |
| `FAILED` | Provider produced the original OpenNT-defined failure result. | Apply that result, then resume or stop as declared. |
| `PENDING` | Provider has accepted a bounded session operation whose result is not yet available. | Do not advance guest execution; mantle performs a controlled suspend until bx-vdm has a completion. |

`PENDING` is not a success result, a generic callback, or a request for Bochs
to interpret a BOP.  It is a fixed-width session state transition owned by
`bx-vdm`.

```text
Bochs #UD/BOP
  -> bx-vdm ingress and OpenNT-owned dispatcher
       -> COMPLETED / FAILED -> checked result -> mantle resume
       -> PENDING
            -> fixed session continuation
            -> mantle controlled suspend
            -> bx-vdm host worker
            -> completion event
            -> checked typed completion
            -> mantle resume
```

## Ownership

| Owner | Responsibility | Explicit exclusion |
| --- | --- | --- |
| `bx-core` / `bx-mantle` | Bounded stop, retained machine state, controlled resume. | BOP selector, COMMAND, DOS, child process, pipe and host-policy semantics. |
| `bx-vdm` | Pending/completion state machine, continuation validation, worker ownership, outcome delivery. | CPU/device emulation and OpenNT service algorithms. |
| Imported OpenNT COMMAND/DEM source | Original service dispatch, ordering, layout and failure behavior. | CCPU/SAS ownership and private NT4 host-product transport. |
| Host capability seam | Public Win32 child/process/event/wait/pipe/job primitives selected by the OpenNT-owned operation. | Guest-visible raw handles, pointers or unbounded policy. |

## Continuation ABI

A pending continuation is session-owned and fixed-width.  It may contain:

- copied command, environment, `VDMINFO`/`CMDINFO` fragments and other
  bounded OpenNT-owned input records;
- fixed-width CPU/BOP continuation metadata;
- checked guest address/length descriptors, validated again before completion
  writes;
- opaque 32-bit stream/resource IDs from the existing session handle manager;
- generation, state, cancellation and completion/error fields.

It must not contain a raw host `HANDLE`, host pointer, C++ object, CRT-owned
buffer, active typed-BOP call pointer, guest bounce-buffer pointer, or a
borrowed CCPU/SAS address.

The existing session handle manager supplies the host-resource half of this
contract:

```text
guest-facing opaque ID -> session handle manager -> host HANDLE
```

The continuation stores only the opaque ID.  A worker resolves it immediately
before creating/operating on a child and releases it through the manager on
completion, cancellation, or session disposal.

## Source-Recovery Order

This candidate must use the mandatory recovery ladder per reached OpenNT call
site:

1. compile and retain the original OpenNT worker/dispatcher body;
2. replace only its CCPU/SAS/private-broker transport with the fixed
   continuation and public Win32 seam;
3. do not modify Bochs unless a separately registered mechanical boundary is
   indispensable;
4. do not author a replacement COMMAND/DEM algorithm.

In particular, `cmdExec32 -> CreateThread(cmdCreateProcess) ->
GetNextVDMCommand` is a source structure to preserve where the typed lifecycle
can represent its pending period.  The worker must instead receive copied
session data, not a retained historical active-call stack.

## Admission Prerequisites

Do not admit this S until all of the following are evidenced:

1. A versioned fixed-width `PENDING` outcome can cross the existing
   core/mantle/adapter boundary without exposing BOP meaning to Bochs.
2. Mantle can suspend and later resume exactly one session at a declared safe
   machine boundary.
3. `bx-vdm` can cancel and dispose a pending worker without leaking a child,
   Job, pipe endpoint or opaque handle token.
4. Each selected OpenNT worker call site has a source/ABI/failure ledger,
   including `GetNextVDMCommand`, reentrancy, standard streams and completion
   semantics.
5. The first profile is explicitly one session; multiple-session scheduling is
   a separate future owner decision.

## Focused Verification

- completed, failed, pending, cancellation and double-completion negative
  state-machine tests;
- no raw host handle/pointer in guest RAM or fixed continuation records;
- opaque-handle acquire/release and disposal tests;
- controlled Bochs suspend/resume fixture with no BOP vocabulary in
  `bx-core`/`bx-mantle`;
- one imported OpenNT COMMAND child lifecycle fixture proving source ordering
  across pending and completion;
- formal MSVC x64 `/MT` closure, documentation governance and diff review.

## Active Recovery Observation

The recovered P2 path compiles the changed `bx-vdm`, `bx-mantle` and
`bx-core` units under the formal MSVC x64 `/MT /W4 /WX` shape. The focused
COMMAND fixture now crosses `PENDING`, executes the detached imported worker,
and returns through opaque worker/event/job IDs. The former
`ERROR_INVALID_DATA` was an adapter initialization defect: it cleared the
copied double-NUL environment before the worker entered the retained OpenNT
environment conversion. The corrected ordering passes Direct, COMSPEC, pipe,
failure, double-completion and cancellation coverage. A separate selector-
blind real-machine fixture now proves `PENDING` suspension and controlled
re-entry at the faulting instruction. This is still not S2 closure until the
final source/ABI/failure-ledger review and formal-graph execution are
evidenced.

## Explicit Non-goals

This design does not by itself admit Redirector remote/named-pipe protocol,
WOW composition, multiple VDM sessions, arbitrary guest re-entry, an NT4
CSR/BaseSrv implementation, or any Bochs CPU/device modification.
