# M0 T344 — In-process multi-session reentrancy audit

## Purpose

Determine the source-defined constraints on more than one MVDM session in one
modern process. This is an audit before feature enablement: original MVDM
global, TLS, BaseVDM, SoftPC, monitor and worker state must each receive one
owner/disposition.

## S plan

### S1 — Original state / caller / ownership inventory

Inventory selected original static globals, TLS/TEB state, worker state,
BaseVDM records, session hooks, mapping-manager instances and SoftPC globals.
For each, record original owner, direct callers, scope (process/thread/VDM),
current product owner and whether it is a blocker, source-shaped binding or
explicitly one-session-only state.

P1 establishes the cross-package carrier catalogue. P2 must read the actual
CCPU worker, SAS/vector, monitor identity and WOW task definitions—not merely
their declarations—so that a per-thread buffer is not mistaken for a
per-session machine and a process-global carrier is not silently multiplied.

### S2 — Selected reentrancy cohort

Only after S1 selects a coherent cohort, recover its source-shaped lifecycle
and negative behavior. Do not turn a global into a per-session value merely to
make a fixture pass.

### S3 — Formal closure

Verify selected lifecycle and rejection behavior on x86/x64 formal CPU40
graphs. The closure may validly declare the product single-session if source
evidence proves no smaller safe cohort.

## Non-goals

No CSRSS recreation, process broker expansion, multi-user control, guest/BOP
changes, CPU30, Bochs route, alternate SoftPC executor or speculative
multi-session scheduler.
