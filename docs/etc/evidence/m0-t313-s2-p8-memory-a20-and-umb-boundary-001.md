# M0 T313 S2 P8 — Memory, A20, and UMB boundary witness

## Original owners retained

The selected x86 CCPU40 formal graph directly composes these original MVDM
owners:

- `softpc.new/base/ccpu386/ccpusas4.c` for SAS/RAM and A20 state;
- `softpc.new/base/dos/emm_mngr.c` for EMS handle/page-frame allocation;
- `xms.486/{xms,xmsa20,xmsblock,xmsdisp,xmsmisc,xmsumb}.c` for XMS and UMB
  control flow;
- `softpc.new/host/src/{nt_emm,nt_umb,x86_emm}.c` for the selected host UMB
  contract.

No project-owned EMS, XMS, UMB allocator, RAM implementation, or second A20
state exists in this route.

## Bounded current witness

`New-T313CcpuLifecycleNinja.ps1` now includes the existing
`adapter-mvdm-host-out/softpc/mvdm_a20.c` same-shaped adapter.  Its only work
is to retain the historical `sas_enable_20_bit_wrapping`,
`sas_disable_20_bit_wrapping`, and query spellings while delegating to the
selected original CCPU/SAS owner.

The refreshed current x86 run at `build/M0-T313/S2/ccpu-lifecycle/r2` passed:

1. original SAS initialization and RAM/vector checks;
2. XMS-shaped A20 enable, query, disable, and query against original SAS
   state;
3. a positive external physical-page publication and original
   `VdmAddVirtualMemory`/`VdmMapDosMemory` DOS alias;
4. negative zero-span/invalid-address rejections and post-remove resolution
   rejection; and
5. existing bounded CCPU/FPU execution and typed outer return.

The physical-page identity is published and released through the session's
single mapping manager.  It proves a host-object-to-guest-page boundary
without exposing a native pointer in the historical MVDM ABI.

## UMB disposition

`xmsumb.c` retains the original UMB list, merge and request/release logic.
Its `ReserveUMB` PVOID-shaped address is explicitly decoded only as the
historical numeric physical address via registered `MVDM-HOST-DIV-078`; it is
not treated as a native pointer.  `nt_umb.c` remains the selected original
owner of the UMB list/reservation source body.

This P intentionally does not claim a full EMS/XMS allocation service or a
guest XMS BOP.  Such a claim would require the original `ReserveUMB` and
XMS/EMS caller sequence to be entered by its owner package, not a fixture
replacement.  The source bodies are selected in the formal graph and their
mapping boundary is now evidenced; later service-package integration must
exercise the original allocation/request path.

## Cleanup review

The reached A20 path consists solely of the original CCPU owner and its small
same-shaped adapter.  No removable non-original implementation was found in
`mvdm-host` or its reached overlay path.
