# M0 T407 S6 platform-carrier disposition

## Decision

S6 completes T407's U04 disposition without creating a replacement kernel
VDM. It separates the directly recoverable MVDM contract from carriers that
depend on NT4 kernel task/TEB state. The latter transfer to existing component
owners with explicit acceptance, rather than being called original merely
because their fields resemble x86 structures.

## Carrier ledger

| Carrier | Original owner and reachable contract | Current state | T407 disposition |
| --- | --- | --- | --- |
| `VDM_DPMIINFO` / PM stack | `dpmi32/dpmiint.c::DpmiPassPmStackInfo`; `dpmi/486/dxboot.asm` calls BOP 53:11 then installs `CX:DX` as `SEL_VDMTIB`. The original structure is `VdmTib.PmStackInfo`. | CPU40 cannot publish the kernel `VdmTib` host address to guest code. The bounded guest projection preserves layout, clears before each BOP return, copies flags/stack selector/IRET fields, and returns a 16:16 guest address. | Retain as the smallest source-shaped MVDM seam. It has no host pointer, generic lease or alternate guest layout. Bootstrap runtime reachability remains unproven by the S5 MEM-only regression and carries to final WOW16 acceptance. |
| DOSX GDT/LDT/IDT | `dxboot.asm` establishes adjacent IDT/GDT and `DpmiSetX86Descriptor` publishes the process LDT; CCPU consumes GDTR/LDTR/IDTR. | S5 provides separate GDT/LDT images and source IDT binding. The first-IDT latch is process-local: the product has one worker/session per process, so no cross-session reuse exists. | Delivered in S5. Do not add speculative reset machinery absent a multi-session worker lifecycle. Table/interrupt lifecycle belongs to CCPU40 profile acceptance if that product shape changes. |
| CPU40 TR/TSS | NT kernel VDM creates the native task and installs its task register; `fastpm.asm`/monitor context assume that already exists. `KiInitializeGdtEntry` is only a descriptor field encoder, not the lifecycle owner. | `modesw.c` contains a finite two-TSS CCPU carrier because CCPU requires a valid TR to execute the original transition. No selected composable user-mode initializer exists; importing kernel startup needs scheduler, trap and process state outside the product boundary. | Retain only as a non-original CCPU mechanical carrier. Transfer task-register/port-I/O/reset proof to the CCPU40 candidate; no kernel-service imitation is admitted here. |
| FastWOW TEB/TD prefix | `wow32/i386/fastwow.asm` reads FS TEB `WOW32Reserved` and three TD fields (`vpStack`, `vpCBStack`, `FastWowEsp`). | Current prefix projection avoids leaking a modern host TEB into guest execution. The original FastWOW object is not part of the selected T407 formal process link, so inspection cannot claim a runtime hit or callback lifecycle. | Retain solely as an unaccepted finite guest projection. Transfer first-entry/callback/return/reset/teardown proof and any reduction to the WOW/debugger candidate. |

## Four-rung result

1. Original MVDM code/layout remain for BOP 53:11 and descriptor publication;
   original FastWOW assembly names the TD field order.
2. PM-stack projection and CCPU table bindings are finite compatibility seams
   because original kernel addresses cannot cross into guest state.
3. Kernel VDM task construction and native TEB carrier are unavailable outside
   kernel/NT4 USER environment; importing them as `opennt-host` helpers would
   import their owning runtime.
4. S6 adds no replacement lifecycle.

## Verification and limits

- S5 fresh x86 product/descriptor fixture passed; corrected `MEM.EXE` exited
  0. This proves no immediate DOS regression after table separation.
- It does **not** prove BOP 53:11, FastWOW callbacks, a TSS task switch, or
  full WRITE. No trace hit is asserted for those paths.
- Source/caller/build scans find `wow32/i386/fastwow.asm` absent from the
  selected T407 formal process link; its dynamic provider is a later owner
  boundary. One-worker-per-process likewise gives no evidence for IDT reset
  across worker reuse.

## Receiver and T407 closure condition

The [CCPU40 candidate](../../proposals/proposal-ccpu40-cvid-event-path-restoration-001.md)
owns TR/TSS and table-register lifecycle acceptance. The
[WOW/debugger candidate](../../proposals/proposal-wow-debugger-original-owner-restoration-001.md)
owns FastWOW TEB/TD lifecycle acceptance. The final WOW16 package validates
the integrated BOP 53:11/WRITE frontier. These are explicit unaccepted
receivers, not successful T407 functionality claims.
