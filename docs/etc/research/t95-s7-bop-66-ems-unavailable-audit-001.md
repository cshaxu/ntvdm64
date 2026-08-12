# T95 S7 BOP 66 EMS-Unavailable Audit 001

## Reached contract

Trace `012` reaches `C4 C4 66` after the source-shaped `5F` completion.  The
guest call is not an arbitrary EMS request: `dos/v86/doskrnl/bios/spcemm.asm`
lines 134--162 define `InitSpcEmm`.  It clears `BX`, passes `DS:DX` only for
the later `68` callback, invokes BOP `66`, and treats either `BX=FFFFh` or
`BX=0` as failure.  Failure returns `AX=FFFFh` to `sysinit1.asm`, which then
branches to `NoEmmServices` rather than installing the EMM stub driver.

The same file states that the stub exists solely so applications can identify
available LIM services and "should only be loaded if emm memory is available".

## Historical owner and profile decision

`softpc.new/base/bios/emm_fncs.c` shows that historical `emm_init` obtains
LIM sizing from the PIF configuration, allocates expanded-memory backing, and
returns its installed page count in `BX`.  A nonzero result also enables its
own DMA/LIM setup.  These are not required to express the OpenNT guest's
unavailable branch.

The first CLI profile has no PIF capability model and does not admit EMS.
Therefore its faithful, minimum outcome is the source-defined unavailable
result: `BX=0`, three-byte BOP resume, and no other CPU/FLAGS/memory/device
effect.  This does not emulate EMS and does not claim that `67` (INT 67h
service) or `68` (alter-page-map callback) are connected.

## Ownership and limits

The adapter owns this one guest-visible completion because it replaces the
removed SoftPC host-side configuration/allocation decision.  Bochs retains no
EMS-specific branch or device.  The guest retains its original branch and its
decision not to install `EMMXXXX0`.

The endpoint must reject protected/v8086 mode, any selector other than `66`,
bad instruction bytes, and RIP overflow.  It may not inspect `DS:DX`, read
guest RAM, install an IVT entry, provide a page frame, allocate host memory,
or alter flags.  Selector `06` remains listener-only diagnostic evidence.
