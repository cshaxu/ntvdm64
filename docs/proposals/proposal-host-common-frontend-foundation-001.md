# Proposal — host common frontend foundation

## Status and objective

The owner split the former multitasking frontend and guest VxD candidate into
two ordered candidates. This first candidate establishes the host frontend
foundation; the subsequent [guest driver and NTVDM bridge](proposal-multitask-guest-vdm-frontends-001.md)
uses that foundation. The host foundation is now admitted in this worktree;
[Current](../states/CURRENT.md) is the sole active-packet authority. The guest
driver remains queued. This proposal retains scope, not another active packet.

Today NTCON is the frontend for a task chain. Extend the host model so NTCON
and a DOS VM inside an NTVDM payload can each act as a task-chain frontend.
The NTVDM worker remains the backend for its upstream chain while providing
separately associated frontend endpoints for the DOS VMs in its payload.
Keep payload, worker and VM identity distinct; avoid assuming that a bare VM
handle is globally unique across workers or payload lifetimes.

## Required host scope

1. Extract the shared frontend foundation from NTCON into
   `common/frontend-base`. Define and implement the reusable frontend
   capabilities and lifecycle contracts needed by both hosts; retain
   NTCON-specific presentation, native Console behavior and application
   ownership in NTCON. Connect NTCON to the extracted foundation without
   regressing existing behavior.
2. Keep shared backend I/O mechanisms in `worker-base`, without any dependency
   on `common/frontend-base`. NTVDM directly composes the shared frontend
   capability for its downstream instances. Future NTVWM adoption can use
   that same direct composition. NTCON must not depend on `worker-base`.
3. Connect NTVDM directly to `common/frontend-base` and establish its payload VM
   frontend registry. Key each frontend by the payload's DOS VM identity or
   handle, scoped to the owning worker and payload; account for registration,
   lookup, retirement, handle reuse and generation/lifetime protection.
   Keep VM identity, guest execution and driver-specific integration in
   NTVDM. This phase enables the capability in NTVDM only; NTVWM can adopt it
   later without requiring that future adoption in this candidate.
4. Extend NTSRV's frontend registration, task-chain association and handoff
   so an NTCON frontend or an NTVDM payload's registered DOS VM frontend can
   be the origin of a task chain. Preserve authentication, association,
   independent chains, completion and lifecycle behavior. NTSRV coordinates
   identity and handoff; it does not become the text, screen or input data
   relay for the guest's DOS windows.
5. Extend NTMON to display a payload DOS VM acting as a task-chain frontend,
   identifying its owning NTVDM/payload and VM identity or handle alongside
   its associated chain. Preserve existing NTCON frontend visibility and
   read-only management behavior.

## Connection and task-chain invariants

There are two connection roles, not a limit of two total connections. NTCON
continues to own one frontend I/O connection to the currently active worker
on its task chain. A worker-base-hosting process has at most one upstream I/O
connection in its backend role, to its assigned frontend, and can additionally
host zero or more downstream I/O connections in its frontend role: one for
each of its active payload VM frontend instances. A worker that does not
host such frontends has none. Actual connection lifetime and absence during
handoff remain governed by the existing lifecycle; do not create dummy
connections to satisfy this model.

`common/frontend-base` extracts the reusable frontend connection capability
and per-instance channel/lifecycle state. Both NTCON and NTVDM use it directly.
`worker-base` must not depend on `common/frontend-base`; common must not
depend on `worker-base` or executable-private code. NTCON's native
Console and window presentation and NTVDM's VM identity, guest-specific
adapter and driver connection remain owned by their respective hosts.
Reuse the current worker/backend I/O protocol, client and transport wherever
their semantics apply. NTSRV remains the authority for authenticated frontend
association and task-chain handoff, and each frontend has at most one active
worker connection at a time. Multiple VM frontend instances within the same
worker must nevertheless have independent connections and chains.

When a payload enters a guest multitasking phase, it remains the active task
on its own upstream chain and its worker retains that chain's frontend I/O
ownership while the payload continues executing. Its upstream frontend may be
NTCON or a VM frontend hosted inside another worker. The guest VMs create
independent downstream chains through their own frontend instances; launching
a task on one of those chains does not hand away the containing payload's
upstream frontend. This applies recursively to nested multitasking payloads.
NTSRV coordinates the resulting per-frontend ownership and handoffs without
relaying user I/O. This is a target invariant, not a claim that the current
single-frontend registry already implements it.

The host foundation must have a testable contract independently of the guest
driver, including host-owned fixtures where needed. This candidate does not
implement a Windows guest 386/VxD driver, modify guest Windows or imply that
the guest-side bridge is operational. Do not infer a guest VM identity from
window focus, CR3, process ID or the most recently scheduled VM.

## Admission and evidence

At admission, audit existing NTCON, NTVDM, NTSRV and NTMON frontend and
handoff paths, and decide the exact shared API, VM-key scope, serialization,
threading and lifecycle boundaries using the then-current architecture and
source rules. Preserve the existing frontend and task-chain contracts, one
worker-owned guest state, and native Console input/output semantics.

Verify NTCON regression, two concurrently registered VM frontend identities
and independent task chains, correct association and handoff, monitor display,
completion in both orders, retained upstream ownership during downstream
activity, nesting through a worker-hosted VM frontend, wrong/stale identity
rejection, VM-handle reuse, absence of worker-base dependencies on frontend-base,
and frontend-base instance isolation independently of NTVDM-specific VM state,
retirement and worker/payload failure cleanup. Exercise actual production
components with reproducible fixtures; state separately what remains dependent
on the later real guest driver. Include relevant lifecycle and single-task,
GUI and redirection regressions and coherent publication at delivery.
