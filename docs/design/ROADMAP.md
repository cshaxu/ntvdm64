# Roadmap

## Delivery Principle

Build a narrow, evidence-backed vertical slice before extending either the
machine or the guest. Bochs feature availability is not progress; an admitted
OpenNT caller exercising its original owner through the hard boundary is
progress. Each phase has a stop gate and does not imply compatibility beyond
its stated fixture.

## Phase 0 - T95: Backend Admission

1. Record the identity of the authoritative local Bochs 2.6 input, preserve
   its layout and notices, and replace the inadmissible 3.0 `src/bochs` tree
   with a manifest-backed 2.6 import.
2. Produce a reproducible, headless, no-plugin minimum configuration. Keep
   every optional device and convenience feature disabled.
3. Prove reset, bounded register observation, checked guest-memory access, and
   one controlled CPU stop with positive and negative fixtures.
4. Specify and test the versioned Bochs/OpenNT bridge record. It must reject
   invalid ranges and must pass neither objects nor pointers.

**Gate:** the source identity, minimal backend probe, and adapter negative
tests pass. This phase proves machine mechanics only; it does not claim DOS
execution.

## Phase 1 - Adapter NTVDM Host-Service Plane

1. Establish one source-derived selector/service catalogue and transaction
   model for the declared `NTIO -> NTDOS -> COMMAND /C` profile.
2. Record every needed service's caller, register/guest-memory contract, data
   layout, order, results, and failure cases before enabling runtime dispatch.
3. Call an original host owner where it has a portable closure; otherwise
   rehost the unavailable composition contract inside the plane.
4. Exercise the common plane through typed bridge fixtures and prove that
   Bochs contains no service policy and guest DOS remains guest code.

**Gate:** the first-profile service catalogue, transaction model and positive/
negative fixtures are complete. Do not promote a probe or fixture to the
product target.

## Phase 2 - Contained DOS Profile

Advance the fixed EN-US profile one reached caller at a time:
`NTIO -> NTDOS -> COMMAND /C`. For every new dependency, first classify it as
OpenNT guest/service, host seam, Bochs machine feature, or fixture. Add only
the owner-side code or backend feature justified by that classification.

**Gate:** the declared command/profile fixture runs with an evidence ledger.
This is still not a general DOS or retail NTVDM compatibility claim.

## Phase 3 - Controlled Expansion

Reconsider a Bochs device, DOS subsystem path, WOW16 path, or host seam only
when Phase 2 exposes a specific reached caller. Each is a separate task with
owner evidence, an admission record, and a negative test. WOW16 is a later
guest/service workstream, not a consequence of booting a DOS profile.

## Phase 4 - Downstream Handoff

After an admitted profile exercises the boundary, publish a neutral boundary
and evidence report for `ntvdm64`. It is comparison and design input only; no
source, build, link, or runtime dependency is created.

## Explicitly Deferred

Recovery of the historical V86/CCPU runtime composition, GUI/debugger/plugins,
unrequested PC devices, broad compatibility work, and a generic virtual
machine product are not roadmap work.
