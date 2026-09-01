# M0 T355 S19 — XMS allocation/commit lifecycle audit

## Objective

Identify the selected CPU40/x86 owner of the first extended-memory access at
`0x00110000` without altering CCPU fallback semantics or running another
container observation.

## Corrected selected graph

The S18 direct-RAM observation established that XMS first reaches
`0x00110000`.  The formal CPU40/x86 graph selects original `xms.c`,
`xmsa20.c`, `xmsblock.c`, `xmsdisp.c`, `xmsmisc.c`, and `xmsumb.c`, but it does
not select original `xmsmemr.c`.  With `MVDM_XMS_SESSION_BACKEND` defined,
`xms.c::XMSInit` passes the callback names `xmsCommitBlock`,
`xmsDecommitBlock`, and `xmsMoveMemory` to `SAInitialize`; their resolved
definitions are in `adapter-mvdm-host-out/softpc/mvdm_xms_memory.c`.

This corrects the former S18 attribution to `xmsmemr.c::xmsCommitBlock` and
`stubs.c::sas_manage_xms`.  Those retained original source forms are not on
the selected formal callback path.

## Original lifecycle and failed ordering

Original `softpc.new/host/src/nt_msscs.c::scs_init` calls `XMSInit` after DEM
initialization.  Original `xms.486/xmsblock.c` ultimately calls
`suballoc.c::SAAllocate`; that allocator invokes its commit callback before it
publishes the allocation.  In the original non-`i386` branch of
`xms.c::XMSInit`, `VdmAllocateVirtualMemory` reserves the Intel backing and
`SAInitialize` receives `VdmCommitVirtualMemory` and
`VdmDeCommitVirtualMemory` as the allocation callbacks.

The selected session callback violates that order:

1. `xmsCommitBlock` requests a write lease;
2. `guest_memory_lease_acquire` first invokes the lease read callback;
3. `mvdm_softpc_guest_memory_read` invokes `c_sas_loads`;
4. the first XMS read targets uncommitted backing at `0x00110000` and faults
   before the requested range can be cleared.

This is an XMS backing-lifecycle selection error, not a CCPU direct-RAM,
C-VID, EMS, mapping-manager, or DEM `50:42` behavior defect.

## Selected recovery

The currently admitted product is CPU40/x86 only.  Its source-faithful
recovery is to remove `MVDM_XMS_SESSION_BACKEND` from the formal graph so that
the existing original non-`i386` `XMSInit` branch selects:

```
VdmAllocateVirtualMemory(..., FALSE)
  -> SAInitialize(VdmCommitVirtualMemory, VdmDeCommitVirtualMemory,
                  xmsMoveMemory)
```

`nt_mem.c` therefore reserves and commits the original `intelMem` backing
before a guest-memory transfer can read it.  The existing same-shaped
`xmsMoveMemory` adapter remains the one mapping-manager-bound transfer seam;
it is not replaced by a raw host pointer.

## Disposition

S19 closes the source/caller/build-selection audit.  S20 is the bounded
build-selection recovery: regenerate the formal CPU40/x86 Ninja graph, prove
the macro is absent and the original allocation/commit branch is selected,
then compile and link.  It performs no runtime observation.
