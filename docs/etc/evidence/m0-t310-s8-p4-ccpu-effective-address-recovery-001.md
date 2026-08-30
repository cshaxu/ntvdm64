# M0 T310 S8 P4 — CCPU effective-address recovery

## Question

Can the selected CCPU's historical `c_effective_addr(IU16, IU32)` external
form be recovered without importing the excluded kernel-VDM MONITOR path or
turning its numeric result into a native process pointer?

## Inputs

- Selected CCPU declarations in
  `mvdm-host/softpc.new/base/ccpu386/cpu4gen.h`.
- Selected CCPU segment-cache accessors in `c_reg.c`.
- Selected descriptor reader in `c_bsic.c` and descriptor-table boundary in
  `c_main.h`.
- Historical, excluded monitor implementation in
  `mvdm/v86/monitor/i386/sas.c`.
- Selected original callers in `softpc.new/base/video`: `video.c`,
  `ega_vide.c`, and `vga_vide.c`.  These callers consume the result as a
  numeric `sys_addr`/guest-linear value; none is entitled to receive a host
  process alias.

## Recovery

`adapter-mvdm-host-out/softpc/mvdm_softpc_effective_address.c` preserves the
original `IU16`/`IU32` signature. It returns a numeric guest-linear address:

- real or VM86 state uses the original `selector << 4` form;
- a currently cached CCPU segment uses its original cached base;
- another protected selector uses CCPU's original
  `selector_outside_table`/`read_descriptor_linear` route before adding the
  original descriptor base.

The historical monitor's `Sim32GetVDMPointer` result was a raw process alias.
It is deliberately not reproduced: neither the adapter result nor any local
state is a host pointer, no Bochs operation is selected, and no mapping-manager
identity is appropriate because selector, offset and result are all guest
numeric values.

## Verification

The formal T310 Ninja graph generated the explicit
`mvdm-softpc-effective-address.lib` target and compiled it successfully with
MSVC `/MT` on both x64 and x86. The only emitted messages for this unit were
pre-existing global macro redefinition diagnostics from `host_def.h`; no
pointer-width, argument-width or incompatible `c_effective_addr` declaration
diagnostic was emitted.

The same graph's complete forced-link target currently restarts a full
selected-source rebuild after graph regeneration and twice stalled after its
active compiler children exited. Therefore no full-link closure is claimed by
this record. A later clean formal forced-link run must prove that
`c_effective_addr` is absent from both unresolved-symbol logs.

## Cluster disposition

This is an implementation-stage recovery, not an exemption from the parent
cluster review.  `c_effective_addr` remains in the
`SPC-CCPU-SAS-MONITOR-VECTOR` cluster with the two unresolved vector tails:

- `c_sas_touch(addr, length)` is an `IU8 *` immediate CCPU RAM access
  contract.  It must be reviewed against `ccpusas4.c`'s own
  `c_GetPhyAdd`/`Start_of_M_area` lifetime and bounds behavior; it must not
  turn a session mapping identity into a permanent pointer.
- `c_VirtualiseInstruction(eipInRom, size, linearAddrOrPort, dataIn)` belongs
  to the original `biosDoInst -> host_simulate` V86/BIOS virtualisation path.
  No selected original source defines it.  A fabricated zero/no-op result
  would change the exception/BIOS contract, so it remains unavailable until
  the full source-shaped machine transaction is proven.

Before the parent is closed, the original vector initializer, all three
members, all video callers above, `ccpusas4.c`'s physical-memory owner, and
every prior adapter repair must be read together on both host widths.  The
parent cluster therefore remains subject to its required full
owner-contract reread.
