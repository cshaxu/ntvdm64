# M0 T291 S4 — `host_simulate` postcondition ledger

`host_simulate` is an original SoftPC spelling, not a universal completion
semantic. This ledger groups every direct selected-source call site by the
postcondition its original owner expects. Definitions, declarations and prose
mentions are intentionally excluded.

| ID | Direct source call sites | Original owner and required postcondition | Current disposition / successor |
| --- | --- | --- | --- |
| HS-01 | `adapter-mvdm-host-out/wow/mvdm_wow_callback_transaction.c:79`; source counterpart `wow32/wcall16.c:780` | WOW non-fast callback: after guest execution, restore saved IP, recover the callback stack and copy AX:DX from `CBVDMFRAME`. | Bounded real-mode mechanical foundation only. S4 P4 records its task limit; later WOW callback-stream recovery owns loaded guest execution and fast WOW. |
| HS-02 | `mvdm-host/wow32/wkman.c:879,1184,1413` | WOW manager resume: execution returns into WOW manager with a valid monitor/guest stack and task context; code follows with task, callback or monitor state work. | Not enabled. S5 must first recover command broker/task binding and explicit monitor stop outcomes. |
| HS-03 | `mvdm-host/dpmi32/buffer.c:326`; macro call in `dpmi32/xlathlp.h:23` | DPMI buffer/translation continuation expects monitor-mediated guest completion and its original register/stack effects. | Not enabled; owned by the DPMI package and its protected/real frame contracts, not by the generic SoftPC facade. |
| HS-04 | `mvdm-host/dos/dem/demdasd.c:513,532` | DEM DASD continuation expects BIOS-like disk operation completion and carry/register error outcomes. | Not enabled; owned jointly by DEM DASD source and firmware/device capability. |
| HS-05 | `mvdm-host/v86/scaffold/i386/softpc.c:232`; `v86/scaffold/i386/stubs.c:38` | Historical V86 product start/monitor scaffold expects complete old VDM monitor control. | `not-host-runtime`: it would recreate the rejected old product shell. |
| HS-06 | `mvdm-host/softpc.new/host/src/nt_mouse.c:463,660,1760,1805` | Host mouse paths expect device event / interrupt delivery after simulation. | `not-host-runtime` until a source-selected input/device package is admitted; no adapter invents mouse completion. |
| HS-07 | `mvdm-host/softpc.new/base/video/video.c:2019,2146`; `base/system/rom.c:427` | Video/ROM paths expect firmware/device execution and native machine state transitions. | Owned by firmware/device composition; not a host-service or WOW completion. |
| HS-08 | `mvdm-host/softpc.new/base/disks/floppy.c:2510,2566,2722,2966,2988`; `floppy_i.c:407` | FDC paths expect device wait/IRQ/DMA progression and return to their source state machine. | Owned by firmware/device/FDC composition; unavailable until that package is source-admitted. |
| HS-09 | `mvdm-host/softpc.new/base/ccpu386/ccpusas4.c:1425`; `base/inc/sas.h:283` | CCPU/SAS execution path expects the original executor itself and its storage model. | `not-host-runtime`: Bochs is the sole executor; adapter-softpc preserves only boundary spelling. |

## Result

The only currently composed call is HS-01, and it has a documented bounded
callback postcondition. All remaining sites have one owner-specific successor;
none may be enabled merely because `host_simulate()` has one same-spelled
adapter entry.
