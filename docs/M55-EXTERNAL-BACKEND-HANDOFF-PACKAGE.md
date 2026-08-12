# M55 External Backend Handoff Package

Status: research handoff, 2026-08-08. This is not an implementation request
and changes no external repository.

## Preconditions Before External Adapter Work

1. A governed machine backend must have a real product/core consumer for each
   requested capability. A single external adapter is not the required proof.
2. The consumer must specify behavior without file names, instruction
   selectors, operating-system records, paths, handles, or product session
   policy.
3. Core tests must pass independently of any external adapter source tree.

## Neutral Capability Candidates

### Configured Decode Observation

At configuration time, accept a bounded byte pattern and freeze registration
with machine topology. At decode/fetch time, provide copied bytes and copied
execution context. Allow only `unhandled`, `handled-resume`, `stop`, or
`fault`; core validates byte consumption and atomically applies only permitted
general-register/status updates. The facility must not change execution mode,
control/segment state, mappings, devices, interrupts, or host objects.

### Paused Ordinary-Memory Transaction

At one paused/stopped execution boundary, accept copied physical spans and
copied bytes. Validate every span through the ordinary-memory route before any
write; reject overflow, non-ordinary routes, conflicting overlap, stale
boundary, and injected publication failure. Commit all normalized spans or
none. Invalidate the transaction on resume, reset, stop, fault, destruction,
or context change. No RAM pointer is exported.

## Required Evidence Order

1. Independent consumer and neutral contract.
2. Core unit tests: registration freeze/conflict, illegal outcome rejection,
   no mode change, invalid span rejection, overlap rules, injected atomic
   failure, and stale-boundary invalidation.
3. A narrow fixed-width external C ABI with opaque machine/transaction handles
   and copied values only. No C++ object, exception, CRT ownership, path,
   native handle, or guest pointer crosses it.
4. Owned adapter fixtures exercise the ABI using sentinels and redacted traces.
5. Only then may an external historical-image prefix trace begin; it stops at
   the first original dependency without inventing a result.

## Waiting Rules For This Workspace

While these prerequisites are absent, `ntdos64` must not create an execution
loop, decoder, RAM mapping escape hatch, serial-write approximation, synthetic
service dispatcher, or normal-path link to frozen instrumentation. It may
maintain BYOB admission fixtures and source/provenance records only.
