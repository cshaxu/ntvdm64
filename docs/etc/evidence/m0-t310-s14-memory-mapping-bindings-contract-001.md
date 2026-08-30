# M0 T310 S14 — memory-mapping-bindings owner review

## Question

Can the original `VdmAddVirtualMemory`/`VdmRemoveVirtualMemory` lifecycle and
the required DOS physical-page alias seam work on x86 and x64 without
truncating a host pointer into OpenNT's fixed-width `ULONG HostAddress` ABI?

## Original contract and selected boundary

`softpc.new/host/src/nt_mem.c` retains the original allocation sequence:
validate an initialized Intel memory system, reserve an Intel span, invalidate
SAS code state, install the physical-page record binding, then remove it in
the reverse order.  The original interface carries a 32-bit host linear
address; that was safe only while an NTVDM process pointer was also 32 bits.

The selected replacement preserves the original spelling, parameter order,
Intel linear address and NTSTATUS outcome.  Before `nt_mem.c` executes its
original allocation algorithm, `mvdm_softpc_physical_mapping_prepare` resolves
the session-owned mapping-manager identity and restores only the necessary
native alignment.  The selected `VdmSetPhysRecStructs` carrier installs or
retires that association.  `ccpusas4.c` resolves it only at its immediate
original physical-access point.

`VdmMapDosMemory`/`VdmUnmapDosMemory` retain the historical page-number
surface for an Intel-to-Intel alias.  They are numeric guest physical pages,
not host identity values, and use the same session lifecycle record.  The
adapter returns original-style invalid/not-found NTSTATUS values rather than
silently creating an alias.

## Disposition

This is the one admitted adapter-backed mapping boundary.  The mapping manager
contains native `x86` or `x64` host addresses behind a session-local stable
32-bit identifier; OpenNT's original machine code continues to manipulate
only fixed-width Intel numerics.  `nt_mem.c` and `ccpusas4.c` retain their
original algorithms and call order with the registered, narrow divergence.
No Bochs, MONITOR, kernel VDM or raw host pointer is selected.

## x86/x64 evidence

Fresh formal MSVC `/MT` selected CCPU graphs ran on both architectures.  The
fixture published a page-aligned host allocation into the mapping manager,
called original `VdmAddVirtualMemory`, observed the same byte through the
source-shaped `c_GetPhyAdd` path, installed a `VdmMapDosMemory` page alias,
wrote through the alias and observed the host page change, then unlinked the
alias.  A second unlink returned `STATUS_NOT_FOUND`; removal released the
mapping-manager identity before session teardown.  Each run then completed the
bounded original CCPU/FPU interval through `returned-recursive` with exit
code zero.
