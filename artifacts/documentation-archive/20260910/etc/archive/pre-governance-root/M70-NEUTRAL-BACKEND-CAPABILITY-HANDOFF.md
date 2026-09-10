# M70 Neutral Backend Capability Handoff

Status: external core-owner handoff, 2026-08-08.

## Purpose

This handoff requests evaluation of two generic machine facilities derived
from independent architecture work and confirmed by the fixed-profile research
matrix. It is deliberately free of DOS, NTDOS, BOP, SVC, path, handle, BIOS,
and product-shell vocabulary.

It does not authorize a change to an external repository. A core owner must
still identify an independent first-party consumer for each facility.

## Candidate A: Configured Decode Observation

### Contract

At machine configuration time, a caller registers a bounded byte pattern at a
defined decode point. Registration conflicts and maximum length are rejected;
the set freezes with machine topology. At runtime, a matching instruction
offers copied execution context and bytes to the observer.

Only these outcomes are allowed:

```text
unhandled
handled-resume
stop
fault
```

Core, not the observer, validates byte consumption and atomically applies only
approved copied general-register/status updates. The observer cannot alter CPU
mode, segments, control state, mappings, devices, IRQ state, host objects, or
execution ownership.

### Required Core Tests

- zero/over-limit pattern rejection and deterministic maximum;
- duplicate and overlapping registration rules;
- configuration-only registration and freeze after initialization;
- matching physical-fetch semantics, not a linear-address shortcut;
- copied context only, no backing-state pointer escape;
- unhandled behavior preserves ordinary CPU behavior;
- invalid patch/outcome rejection with no partial CPU update;
- atomic allowed patch application; and
- stop/fault behavior at defined instruction boundaries.

### Independent Consumer Gate

The first consumer must use configured valid-instruction observation without
operating-system selectors, file formats, private structures, or product
policy. It must need the same constrained outcome model.

## Candidate B: Paused Ordinary-RAM Transaction

### Contract

At a stopped/paused boundary, a caller submits copied physical spans and byte
payloads. Core normalizes spans and validates all of them through its existing
memory routing. Every span must be ordinary writable RAM; overflow,
provider/ROM/device routes, overlap conflict, stale boundary, and validation
failure reject the transaction before publication.

On commit, every normalized span becomes visible or none do. The transaction
invalidates on resume, reset, stop, fault, destruction, or execution-context
change. Neither a RAM pointer nor a device callback crosses the API.

### Required Core Tests

- zero-length and address-plus-length overflow rules;
- RAM versus ROM/provider/device rejection through the actual memory route;
- non-overlap/overlap normalization rules;
- preflight failure leaves all bytes unchanged;
- injected write/publication failure rolls back all spans;
- successful multi-span visibility is all-or-none;
- stale boundary invalidation across every lifecycle transition; and
- reset does not mutate immutable mapped content.

### Independent Consumer Gate

The first consumer must have a generic reason to install coordinated ordinary
RAM state at a paused boundary, without image/file, boot, DOS, or product
policy semantics.

## External ABI Requirements

Only after both contracts and tests exist, expose a narrow versioned `extern
"C"` ABI:

- opaque machine, registration, and transaction handles;
- fixed-width copied structs and explicit calling conventions;
- checked span descriptors and copied byte buffers;
- structured error/result codes; and
- no C++ objects, exceptions, CRT allocation ownership, native paths/handles,
  guest pointers, callbacks that expose backing storage, or product types.

The external ABI must be version-negotiated and must not require this research
source tree to compile, link, test, or run.

## Admission Evidence Back To This Workspace

Before `ntdos64` uses the ABI, the core owner supplies:

1. source location and revision for both contracts;
2. independent-consumer rationale;
3. passing focused test names/results covering the lists above;
4. ABI version and ownership/error rules; and
5. a minimal owned fixture proving copied-value use without external profile
   sources.

Only then may this workspace build a redacted historical-image prefix fixture.
That fixture must stop at its first unimplemented historical dependency and
must not add an execution loop, synthetic dispatcher, raw guest-memory access,
or a normal-path link to trace instrumentation.

## Basis

This handoff is derived from
`docs/M55-EXTERNAL-BACKEND-HANDOFF-PACKAGE.md`,
`docs/M58-GOVERNED-BACKEND-READINESS-REAUDIT.md`, and
`docs/M69-NT4-ENUS-BOOTSTRAP-CAPABILITY-MATRIX.md`.
