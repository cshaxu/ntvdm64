# M0 T256 S5 — Guest-RAM Geometry Impact Map

## Finding

The current runnable composition has a hard 1 MiB machine geometry.  This is
not merely a fixture default:

- `bx_ntvdm_machine_stage_v1_begin` constructs the minimal machine with
  `0x100000` guest and host bytes (`bx-mantle/bx_ntvdm_machine_stage_v1.cc`);
- `bx_ntvdm_run_finite_bare_bytes` does the same
  (`bx-mantle/bx_ntvdm_finite_run.cc`); and
- the engine obtains its stage request from startup composition, so there is
  no session/CLI memory-geometry contract on the product route.

Many current bx-vdm validation shims also intentionally reject addresses at
or above `0x100000`.  They are correct for the current conventional-memory
profile, but they cannot quietly become DPMI extended-memory access rules.

## Consequence

There is no safe, unclaimed ordinary-RAM domain from which an `xmem.c`
allocation can be made.  A first-fit allocator in mantle would either overlap
the active guest image/DOS memory or fabricate backing not present in the
machine.  Neither is admissible.

## Required sequencing

Before a generic range reservation record is implemented:

1. define a session-owned machine-memory geometry at admission, including
   configured total guest RAM and any explicitly reserved ordinary-RAM
   interval;
2. make the stage and production engine consume that copied geometry, while
   retaining the existing 1 MiB default for current DOS/WOW16 profiles;
3. audit each current `0x100000` check as **conventional-only**,
   **whole-guest bound**, or **fixed boot-layout bound**; do not bulk-replace
   constants; and only then
4. admit a selector-blind range reservation record over the explicitly
   reserved interval.

This is a generic machine-capacity and lifecycle prerequisite.  It belongs to
the bx-mantle/bx-vdm composition boundary, not to an individual DPMI BOP and
not to bx-core.  It does not justify an adopted-Bochs patch.

## S5 decision

S5 does not add a reservation allocator.  It transfers that implementation
only after the geometry contract and the bounded `0x100000` audit are
admitted.  The source-shaped DPMI `53:07..10/13` memory lifecycle remains
deferred behind this common prerequisite.
