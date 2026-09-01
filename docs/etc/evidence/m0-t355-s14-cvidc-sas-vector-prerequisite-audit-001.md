# M0 T355 S14 — C-VID SAS-vector prerequisite audit

## Question

Why did the sole S14 fixed-container observation terminate before the recovered
`SVC_DEMFASTREAD` route, and is its immediate CCPU/C-VID prerequisite an
original-provider recovery or an invented machine behavior?

## Exact source chain

The copied exception return maps to
`softpc.new/base/cvidc/accessfn.c::sas_overwrite_memory`.  That generated
wrapper unconditionally invokes `Sas.Sas_overwrite_memory(addr, length)`.

The selected CPU40 source already supplies the exact slot contract:

- `softpc.new/base/cvidc/sas4gen.h` declares
  `TYPE_sas_overwrite_memory` as `void (IU32, IU32)`;
- `softpc.new/base/ccpu386/ccpusas4.c::c_sas_overwrite_memory` defines that
  contract and deliberately performs no action because CCPU has no compiled
  code cache to invalidate;
- `softpc.new/base/ccpu386/ntstubs.c::sas_overwrite_memory` is the original
  same-shaped public facade and calls that exact CCPU provider.

The selected table carrier `ccpu386/sascdef.c` instead initializes exactly
this slot to a typed null.  Existing full-table evidence correctly classified
many CPU40/SPC386 slots as unavailable, but this particular slot is not one
of them: it has an original selected-profile provider and a reached generated
caller.  Leaving it null is therefore a composition omission, not an original
unsupported-profile contract.

## Boundary and disposition

This is a one-slot repair inside the already-audited CCPU/C-VID SAS-vector
owner package.  It must bind only `Sas.Sas_overwrite_memory` to the existing
`c_sas_overwrite_memory` provider with its original fixed-width contract.
It must not fill other unavailable vector slots, change guest memory, create a
mapping, add a cache, or change CPU execution.  The provider is a no-op by
original CCPU design.

## Effect on S14

The sole S14 runtime attempt did not reach `50:42` because this earlier
source-owned C-VID slot was null. This is prerequisite evidence only: S14
remains active while its corrected FastRead provider is formally verified.
The one-slot C-VID recovery is a later machine-owner cohort, not part of the
FastRead fallback contract.
