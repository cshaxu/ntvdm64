# M0 T310 S5 — CCPU x64 memory-boundary audit

## Scope

This is a source-first disposition of the original SoftPC host-memory paths
which are selected by the S5 CCPU candidate but not exercised by the bounded
`D6 FE` executor proof.  It does not enable a replacement allocator and does
not change an original source body.

The selected proof initializes SAS and runs `nt_cprgs.c` plus
`nt_cpu.c::host_start_cpu` / `host_simulate`.  It deliberately does not start
`config.c`, EMM, XMS/DPMI allocation, WOW DIB mapping, BIOS, or device
composition.  Passing that proof therefore cannot validate the paths below.

## Findings and final owner dispositions

- `softpc.new/host/src/config.c::InitNtCpuInfo` is reached only from the
  original configuration path (`config.c:539`).  Its `GDP_PTR + fixed offset`
  reconstruction is a native host-address assumption, not a guest address or
  externally published identity.  It is **not enabled by S5**.  Before the
  configuration path is admitted, `adapter-mvdm-host-out/softpc` must provide
  a source-shaped CPU-state projection or declare the profile unavailable.
  Blindly widening its casts would preserve an unsafe implicit layout
  assumption rather than recover the contract.

- `nt_mem.c::{VdmAllocateVirtualMemory,VdmCommitVirtualMemory,
  VdmDeCommitVirtualMemory,VdmFreeVirtualMemory,VdmReallocateVirtualMemory}`
  retain the original allocation-list algorithm and Intel-linear return ABI.
  They also reconstruct native addresses as `intelMem + ULONG`.  This is safe
  only while the original 32-bit contiguous host reservation exists; it is not
  a valid x64 component boundary.  The original source remains the algorithm
  authority.  A later same-shaped `adapter-mvdm-host-out/softpc` binding must
  resolve the session's **guest-memory mapping-manager instance** into a
  checked backing range, then provide the original `ULONG` guest-linear result
  and original success/failure ordering.  The manager must not publish a raw
  host pointer as that value.

- `nt_mem.c::{VdmAddVirtualMemory,VdmRemoveVirtualMemory}` additionally accept
  an original `ULONG HostAddress` and alter `PhysicalPageREC.translation`
  through `VdmSetPhysRecStructs`.  Reached callers include WOW DIB mapping
  (`wow32/wdib.c`) and the documented DPMI selector use case.  On x64 this
  would truncate an external native pointer before the physical translation
  update.  This is a **mapping-manager + physical-page transaction** boundary:
  adapter code must pin/resolve a host-memory resource in the active session,
  stage the guest-linear translation through the SoftPC mechanical owner, and
  release it at the original remove point.  Neither a pointer-width cast nor a
  generic host-resource ID can silently substitute for the translation entry.

- Direct original callers establish the dependency order: `xms.486/xms.c`,
  `dpmi32/dpmimemr.c`, `dpmi32/dpmiselr.c`, `softpc.new/host/src/nt_emm.c`,
  `wow32/wkmem.c`, and `wow32/wdib.c`.  Thus XMS/DPMI/EMM/WOW are consumers of
  this lower memory binding; none may independently invent an allocator or
  pointer-token table.

## S5 consequence

The bounded CCPU result remains valid, but S5 must not call its x64 SAS setup
a complete SoftPC memory lifecycle.  The next implementation slice is a
source-shaped, session-owned mapping-manager binding for this family, with
its `PhysicalPageREC` behavior separately proven before XMS/DPMI/WOW callers
are enabled.  Until then those callers retain their original unavailable or
unselected profile disposition.

## Negative checks

- No Bochs type, symbol, source input or fallback was selected.
- No `src.old` input was inspected as a source/build/runtime dependency.
- No raw host pointer is introduced into a guest-visible `ULONG` value.
- The existing mapping manager is the only permitted identity/mapping owner;
  this audit creates no second handle or address map.
