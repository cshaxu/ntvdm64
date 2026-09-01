# Proposal — COMMAND/DEM/SoftPC guest scalar-alias lifecycle recovery

## Candidate status

This is an **unnumbered queue candidate**, not an active T/S packet.  It does
not authorize source or behavior changes while M0 T346/S4 remains active.

## Objective

Recover the complete original `cmdSetInfo` registration contract without
retaining durable host pointers into guest memory.  The result must preserve
the OpenNT COMMAND/DEM/SoftPC source order and observable results while using
the existing session mapping manager as the only guest-address authority.

## Original owner package

The one source-defined cohort consists of:

- NTDOS `msinit.asm`: registration of `SCSINFO`, `SCS_Is_Dos_Binary`, and
  `SCS_FDACCESS` through `SVC_CMDSETINFO`.
- COMMAND `cmdmisc.c`: registration plus original writes from `cmdGetNextCmd`.
- COMMAND `cmdpif.c`: original PIF binary-type reset.
- NTDOS `msproc.asm` and `macro.asm`: guest consumption/reset of binary and
  CDS synchronisation fields.
- NTDOS `misc.asm`, DEM `demmisc.c`, SoftPC `nt_fdisk.c` and `nt_rflop.c`:
  disk-access counter reads, writes and reset.

It intentionally excludes a BOP service rewrite, DOS drive policy,
configuration expansion, guest-media modification, FDC/BIOS behavior, a new
mapping manager, raw host-pointer transport and all Bochs routes.

## Required recovery ladder

1. Retain the original translation units and their registration/use ordering.
2. Extend only `adapter-mvdm-host-out/softpc` with a same-shaped,
   session-owned scalar-location/short-lease façade backed by the existing
   `mvdm_guest_location` and mapping manager.
3. Make the smallest mirror-site substitutions at original pointer retention
   and dereference sites.  Each has a local `DIVERGENCE` annotation and a
   matching component README registration.
4. Do not introduce newly authored COMMAND, DEM, disk, BOP or guest policy.

An adapter-owned durable **location** is allowed; a durable host pointer or a
second guest mapper is not.  Each read/write acquires a bounded lease at its
original access point, commits if necessary, and releases before returning.

## Proposed S sequence

### S1 — Original registration/use/lifetime ledger

Record the three positions, their widths, exact registrations, every
original host and guest read/write, thread/session lifetime, teardown result,
and all error paths.  Decide the smallest unified adapter API.  No code.

### S2 — Adapter scalar-location contract

Implement and locally test the session-bound adapter API for byte and word
read/write/reset/increment/decrement.  It must reject unmapped, cross-session,
stale-epoch and out-of-range locations.  It may not expose a guest pointer to
callers.

### S3 — Original source binding migration

Migrate every selected COMMAND/DEM/SoftPC source access to S2's API in source
order.  Remove the two durable raw `GetVDMAddr` aliases only after all listed
uses have one route.  No parallel v1/provider path remains.

### S4 — Cohort verification and transfer

Run local scalar/teardown/negative tests and the selected x86/CPU40 formal
product build.  Then use one unchanged fixed runtime observation only to
classify the next owner; success is not inferred from a BOP marker.  If the
run still does not reach graphics, transfer that exact next owner without
expanding this cohort.

## Completion criteria

- All three original registration locations and every listed consumer have
  one source-shaped adapter route.
- No durable guest pointer remains in the selected COMMAND/DEM/SoftPC paths.
- Mapping manager ownership, access direction, epoch and teardown behavior
  have focused positive and negative proof.
- Original source files remain mirrors or minimal registered divergences.
- Selected x86/CPU40 formal build and local lifecycle tests pass.
- One fixed observation is recorded without treating it as a leaf-repair
  selector.
