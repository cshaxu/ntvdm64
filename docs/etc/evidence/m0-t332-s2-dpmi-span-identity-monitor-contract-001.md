# M0 T332 S2 — DPMI span, identity and monitor contract

## Result

The selected original DPMI32 provider has two different pointer-shaped
contracts.  They must not be conflated.

- `Sim32GetVDMPointer` is the original CPU40 in-machine memory interface.
  `mvdm-host/softpc.new/host/src/sim32.c` preserves
  `Sim32pGetVDMPointer → convert_addr → GetVideoMemory/NtGetPtrToLinAddrByte`.
  Its result is used only by host provider code executing in the same SoftPC
  machine invocation.  Its pointer lifetime is deliberately source-shaped:
  callers such as `DpmiInitDosx`, DPMI stack/IRET code and `int21map.c` keep
  pointers while traversing original tables and frames.
- XMEM and PM-stack-information publication are genuine host-boundary numeric
  carriers.  `mvdm_host_identity_*` delegates to the session's existing
  `host_resource_mappings` instance, so it is the one generic mapping-manager
  implementation and not a parallel DPMI registry.  It is the correct
  replacement for publishing an x86 native pointer in a 32-bit register pair.

Therefore S2 keeps the original Sim32 in-machine lifetime intact.  Replacing
each expression with a lease/bounce buffer would change the original source
algorithm, invalidate nested pointer arithmetic and create broad mirror
differences.  The existing `session_guest_memory_lease` remains the bounded
adapter contract for cross-component copied access (for example XMS and
`mvdm_guest_location`); it is not a substitute for SoftPC's in-machine
`Sim32pGetVDMPointer` API.

## Monitor result

`VDM_TIB` is provided as an original-layout, session-bound TLS projection by
the monitor adapter.  `dpmiint.c`'s
`NtVdmControl(VdmSetInt21Handler, ...)` reaches the historical kernel-VDM
control contract.  The current same-shaped adapter intentionally implements
only `VdmQueryDir`; this request returns `STATUS_NOT_IMPLEMENTED`.  That is a
source-defined unavailable boundary, not an opportunity to return a fake
registration success.

## S3 admission

S3 may now recover the selected original initialization, descriptor and XMEM
cohorts.  It may use the original Sim32 source binding, the one session
host-resource mapping-manager wrapper for host identities, and the explicit
monitor failure boundary.  It may not add a DPMI-local mapper, an alternate
dispatcher, raw native identity in guest registers, a kernel-VDM substitute or
a protected-mode success shortcut.
