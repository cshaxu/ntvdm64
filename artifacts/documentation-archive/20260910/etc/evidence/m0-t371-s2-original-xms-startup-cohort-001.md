# M0 T371 S2 — Original XMS startup cohort

## Question

Can the first complete XMS startup cohort retain original `XMSInit` and
`suballoc` behavior while selecting the correct original VDM-memory path or
the already bounded mapping-manager fallback?

## Inputs

- Original `mvdm-host/xms.486/xms.c`, `xms.h`, and
  `mvdm-host/suballoc/{suballoc.c,suballcp.h}`.
- Original `mvdm-host/inc/suballoc.h` callback declaration.
- Existing adapter `mvdm_xms_memory.c` and session guest-memory lease.
- The selected CPU40/x86 formal product graph.

## Procedure

1. Compared the original `PSAMEMORYMOVEROUTINE` typedef, `SAInitialize`
   storage/call path, and both original direct XMS move definitions.
2. Corrected only the imported `xms.h` declaration to the source-proved
   `(Destination, Source, Count)` callback form.  The change is registered as
   `MVDM-HOST-DIV-198`; it changes no XMS algorithm, callback name or result.
3. Added a focused source-shaped fixture that compiles original `xms.c` and
   original `suballoc.c`, supplies only an empty successful UMB reservation,
   and binds guest backing through the existing session mapping manager.
4. Ran both original `XMSInit` allocator selections:
   - successful `VdmAllocateVirtualMemory` at the original XMS base, followed
     by original `VdmCommitVirtualMemory`/`VdmDeCommitVirtualMemory` callbacks;
   - original `STATUS_NOT_IMPLEMENTED`, followed by
     `xmsCommitBlock`/`xmsDecommitBlock`/`xmsMoveMemory` callbacks.
5. In each case, allocated, wrote, freed and reallocated an original
   `suballoc` block to prove its initialization and commit/decommit direction.
6. Regenerated and linked the complete CPU40/x86 `original-softpc-process.exe`
   formal product.

## Observations

- `suballoc.h::PSAMEMORYMOVEROUTINE` is authoritative and explicitly declares
  `(ULONG Destination, ULONG Source, ULONG Size)`.  The selected original
  `xmsmemr.c` and `i386/xmsmem86.c` definitions use the same order.
- The old `xms.h` declaration was the sole reversed carrier.  Passing it as a
  callback hid the mismatch rather than establishing a valid contract.
- `xms-init-fixture.exe` built with Ninja and exited zero.  It proves that
  `XMSInit` initializes `ExtMemSA`, that `SAAllocate` invokes the selected
  commit callback, and that `SAFree` invokes the selected decommit callback.
- In the successful VDM allocation branch the fixture recorded the original
  VDM callbacks.  In the `STATUS_NOT_IMPLEMENTED` branch it recorded none of
  those callbacks; the existing session-mapped callbacks performed the same
  original suballocator role.
- The disposable formal graph rebuilt all required CPU40/x86 sources and
  linked `original-softpc-process.exe` successfully.  Its historical compiler
  warnings remain visible; none was suppressed or reclassified by this S.

## Interpretation

The original XMS startup cohort is source- and link-closed for its two
allocator selections.  It uses the mapping manager only where the original
allocator reports unavailable and the original callback contract requires a
guest-memory backing; numeric XMS addresses remain numeric values.

This is not evidence that a guest reached `52:xx`, that an A20/UMB/INT 15
service ran, or that a DOS extender started.  Those are later original
consumer cohorts.

## Follow-up

S3 owns the original DPMI/DOSX cohort.  It must retain the original
`MS_bop_3 -> DpmiDispatch` table and distinguish host DPMI source proof from
immutable guest DOSX execution.
