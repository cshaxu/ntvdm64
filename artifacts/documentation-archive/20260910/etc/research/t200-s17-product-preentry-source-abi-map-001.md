# T200 S17 — product preentry source/ABI map

## Result

The product path has a source-derived NTIO publication shape, but does not yet
have a source-bound mantle initial-state provider.  Therefore S17 admits no
machine publication implementation.  The next task must represent and bind
the required initial-state evidence as a versioned profile input; it must not
hard-code historical snapshot bytes in bx-mantle or bx-vdm.

## Historical handoff

OpenNT `base/mvdm/softpc.new/host/src/nt_msscs.c:200-221` loads the selected
NTIO file at `NTIO_LOAD_SEGMENT:NTIO_LOAD_OFFSET`, restores the virtual
interrupt word at `FIXED_NTVDMSTATE_LINEAR`, then calls only `setCS` and
`setIP`.  The retained constants resolve to the existing plan ABI geometry:

| Operation | Typed product representation | Owner |
| --- | --- | --- |
| Capture four overlapping bytes | `preserved_state_address=0x714`, `bytes=4` | later mantle mechanical publisher |
| Publish validated NTIO payload | guest write at `0x700`, payload offset zero | bx-vdm direct composition |
| Restore captured bytes | existing startup-plan preservation rule | later mantle mechanical publisher |
| Transfer control | real-mode `CS:IP=0070:0000` | existing startup-plan entry record |
| All other CPU state | retain native machine reset state | bx-mantle/core |

This agrees with `bx_ntvdm_startup_plan_v1`; S16 now supplies the owned
payload.  It does not authorize a complete CPU state rewrite.

## First guest dependencies

The first NTIO prefix is not independent of machine state.  OpenNT
`dos/v86/doskrnl/bios/msinit.asm` reads and saves IVT 10h, 15h, 19h and 1Bh,
tests BDA `0040:0096`, and uses the virtual-interrupt word at `0x714` before
the first `50:11`.  The r18 evidence records one specific CPU5 stock-ROM
profile's raw ranges, including `0x714..0x717 = 00 00 00 c0`; its exact
meaning and admission boundary are retained in
`adapter-r18-first-prefix-input-admission.md`.

Those raw bytes are not defaults.  They are evidence for one firmware profile
and only through the first original prefix; they do not authorize BIOS/device
or interrupt implementation.

## Current architecture comparison

`bx_ntvdm_minimal_machine_c::initialize` supplies SIM, RAM, default port
space, CPU reset, A20 and extended-memory lifecycle.  It does not initialize
ROM firmware, execute POST, or bind a selected firmware snapshot.  Conversely,
the direct source-built profile consumed by S16 has no machine-startup
initial-state declaration.  It cannot prove the required IVT/BDA/virtual
interrupt inputs for an actual minimal-machine NTIO run.

The old `adapter_runtime` has an environment-installed profile-plan path and
the old full-product `main.cc` consumer.  Both are rejected for the new CLI
engine because they conflate environment state with product composition and
would revive the full product shell.

## Required next ABI

S18 must design—not apply—a copied, versioned initial-state declaration owned
by bx-vdm/profile composition.  It must contain only profile-identified,
bounded ordinary-RAM range data and evidence identity, with explicit absent
state.  The future mantle API may accept a validated opaque declaration and
perform generic preflight/copy only.  It must not name IVT, BDA, NTIO, BOP,
DOS, OpenNT, firmware or service concepts.

Before any CPU run, the declaration must be bound either to a compatible
minimal-machine observation or rejected.  Fixture preentry actions and the
old finite-run request are not product inputs.

## Decision

The next implementation remains blocked by an intentionally explicit input
contract, not by missing CPU or BOP behavior.  S17 closes the source/ABI map;
S18 owns the profile declaration and admission design.
