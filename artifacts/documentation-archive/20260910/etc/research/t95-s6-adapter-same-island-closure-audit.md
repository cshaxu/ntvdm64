# T95 S6 Adapter Same-Island Closure Audit

## Question

Can the current adapter runtime be linked directly into the r4 MSVC/x86 native
machine as the first composition step?

## Inputs

- `src/bx-ntvdm-adapter/bx_ntvdm_adapter_runtime.c/.h`.
- `src/bx-ntvdm-adapter/bx_ntvdm_host_session.c/.h`.
- Historical UCRT r14 object-only record
  `adapter-r14-profile-observation-object-closure.md`.
- r4 default-off execution-plan seam in `refs/bochs/main.cc`.

## Observations

The adapter directory presently has 38 C translation units. The current
runtime directly includes the host-session, BIOS-memory, DEM debug/DTA/drive/
hard-error/DPB, CMD-info, host-drive policy, machine-profile and startup
snapshot paths, in addition to BYOB image/profile code. Its public runtime
header likewise exposes generic exception/read/gather/multi-write/observation
and host-transaction types.

The historic r14 UCRT object evidence names only a smaller predecessor set and
is neither MSVC/x86 nor same-version link evidence. It cannot establish a r4
companion executable closure. The r4 default target is correctly independent:
the execution-plan include/call is behind
`BX_NTVDM_ENABLE_EXECUTION_PLAN`, and no adapter object is in its link vector.

## Decision

The current `bx_ntvdm_adapter_runtime.c` is **too broad to link directly** as
the first native-composition slice. Doing so would pull an unreached host
service plane and all of its future semantic surface merely to obtain an NTIO
startup plan. That would not be a mechanical closure; it would violate the
declared minimum adapter boundary.

No MSVC compile or link is admitted from this audit. There is no conclusion
about compiler compatibility or executable availability.

## Required Extraction Boundary

Before a companion target, define one adapter-owned, project-authored
`startup-session` slice with exactly these responsibilities:

1. Accept the already CLI-validated profile/root identity and three immutable
   BYOB component identities.
2. Validate/copy the payload identity into adapter-owned lifetime storage.
3. Produce the existing pointer-free `startup_plan_v1` for NTIO only.
4. Expose no selector dispatcher, DEM/BOP/CMD service, guest read/gather,
   host namespace, drive enumeration, search state or generic transaction
   consumer.

It may reuse existing ABI/value records, but may not include the broad runtime
header as a shortcut. The broad runtime remains the future host-service plane
owner after its catalogue is reached and independently admitted.

## Follow-Up

The next task is a source-level extraction design: map the exact current
functions/data types needed for that startup slice, identify reusable ABI
files, and define negative compile/link tests. It may not move code, compile,
link, run, alter Bochs, or claim guest startup.
