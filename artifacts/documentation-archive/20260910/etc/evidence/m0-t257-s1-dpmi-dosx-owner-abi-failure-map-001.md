# M0 T257 S1 — OpenNT DPMI/DOSX owner, ABI and failure map

## Selected profile

The only admissible profile is the original non-WOW DOSX/DPMI 486 package:
guest `dpmi/486/{dxboot,dxfunc,dxint31,dxintr,dxmain,dxstrt,dxutil}.asm` plus
host `dpmi32/`.  The 25 `53:xx` selectors are entry points into this one
bidirectional lifecycle; none is a standalone provider candidate.

## Full selector map

| Selectors | Original bodies | Package role | Modern disposition before code |
| --- | --- | --- | --- |
| `53:00`, `53:0F` | `dpmiselr.c`, `dpmi32.c:DpmiPassTableAddress`, DOSX `dxutil.asm` | descriptor table publication and updates | Source-derived rehost only: replace pointer/process-LDT representation with bounded guest table access while native Bochs remains descriptor authority. |
| `53:01`, `53:04`, `53:05`, `53:0C`, `53:0D` | `i386/dpmi386.c`, `dpmi32.c`, `dpmimscr.c`, `dxstrt.asm`, `dxfunc.asm` | mode/DOSX/app/session startup and teardown | Whole-lifecycle recovery; old `VdmTib`, feature flags and emulator hooks require a session-owned compatibility record, not a core/mantle API. |
| `53:02`, `53:0A`, `53:14..18` | `dpmiint.c`, `dpmiint.h`, `dpmimscr.c`, `dxintr.asm` | interrupt/fault registration, locked-stack frames and IRET BOP continuations | Source-derived package rehost after table publication; `NtVdmControl`, `VdmInstall*` and direct `VdmTib` are unavailable historical host composition. Native Bochs still delivers architectural faults/IRET. |
| `53:03` | `i386/dpmi386.c`, DOSX fast BOP stubs | fast protected BOP entry | Deferred with the complete protected BOP ingress contract; no independent fast path. |
| `53:06` | `int21map.c`, `buffer.c`, `stack.c`, `dxmain.asm` | PM-to-DOS INT 21 translation | Later internal-DOS subpackage of the same profile; consumes DEM/NTDOS behavior and bounded protected buffer/frame access. |
| `53:07..09`, `53:10`, `53:13`, `53:0B` | `xmem.c`, `dpmimemr.c`, `i386/dpmimem.c`, `dxint31.asm` | XMEM alloc/free/realloc, owner cleanup and memory query | Existing T256 record adapter supplies identity/reallocation only. Original host allocation APIs are replaced by that adapter; source ordering/failure remains DPMI-owned. |
| `53:0E` | `i386/dpmi386.c` | debug-register update | Explicit deferred capability: `ThreadSetDebugContext` is an old host-thread contract and must be coordinated with Debugger/VDD policy, not silently emulated. |
| `53:11` | `dpmi32.c`, `stack.c`, `dxstrt.asm` | PM stack information publication | Coupled to locked stack + IRET family; not independent. |
| `53:12` | `vxd.c`, DOSX VCD call sites | VCD protected-mode service | Explicit unavailable/deferred until VDD/VCD owner package; no adapter substitute. |

## Original module inventory

| Module group | OpenNT files | Source status / key dependency |
| --- | --- | --- |
| Dispatcher/state | `dpmi32.c`, `data.c`, `debug.c`, headers | Must enter together; exposes direct `VdmTib`, `Sim32GetVDMPointer` and DOSX shared data. |
| Descriptor/mode | `dpmiselr.c`, `modesw.c`, `register.c`, `i386/dpmi386.c`, `xlathlp.h` | Needs source-derived pointer/table adaptation. `NtSetLdtEntries`, `ProcessLdtInformation`, `NtVdmControl`, `ThreadSetDebugContext` cannot be linked as the original composition. |
| Frame/interrupt | `dpmiint.c`, `dpmiint.h`, `stack.c`, `dpmimscr.c` | One locked-stack/handler/re-entry unit. Consumes native Bochs fault/IRET but cannot install a generic core callback. |
| Memory | `xmem.c`, `dpmimemr.c`, `i386/dpmimem.c` | T256 supplies bounded RAM reservation/record identity. Original `Nt*VirtualMemory` source is reference only. |
| DOS translation | `buffer.c`, `int21map.c`, `savestat.c` | Depends on protected pointer replacement and guest NTDOS/DEM transactional semantics; includes explicit original unsupported DOS functions. |
| Optional platform | `vxd.c` | VCD/VDD package dependency; remains an explicit non-success terminal. |
| DOSX guest | `dxboot.asm`, `dxfunc.asm`, `dxint31.asm`, `dxintr.asm`, `dxmain.asm`, `dxstrt.asm`, `dxutil.asm` | Must be built/staged and run as a guest component; bx-vdm must not port its selector/INT31 logic. |

## ABI/failure boundary

1. `Sim32GetVDMPointer`, `FlatAddress[]`, `Ldt`, `IntelBase`, `VdmTib` and
   pointer-valued stack/frame records are not portable ABI.  bx-vdm must use
   session-owned records and bounded copied guest accesses.
2. `NtSetLdtEntries`, `NtSetInformationProcess(ProcessLdtInformation)`,
   `NtVdmControl`, `VdmInstall*`, `EnableEmulatorIretHooks`, and
   `ThreadSetDebugContext` are classified historical product dependencies;
   no direct import is admitted.
3. Native Bochs remains owner of CR0 execution, GDT/LDT/IDT validation,
   protected exception delivery and IRET.  The package may drive guest code
   through public existing typed execution boundaries, never copy CPU caches.
4. `53:12` and `53:0E` must retain explicit unavailable/deferred failure until
   their VDD/debug owners are admitted.  No fake success.

## Ordered S plan

1. **S2 — source mirror/import boundary:** establish a quarantined, original
   `dpmi32`/DOSX source manifest with named shim declarations and a compile
   refusal for every historical product import; no ingress.
2. **S3 — session/table startup composition:** recover `53:04/05/0C/0D/0F/00`
   as one source-shaped startup/table unit, only if bounded guest table access
   and DOSX guest staging are proven.
3. **S4 — protected frame/IRET composition:** recover the `02/0A/11/14..18`
   family as one stack/handler lifecycle on native Bochs mechanics.
4. **S5 — XMEM/memory package:** connect existing records to imported source
   ordering for `07..0B/10/13`, after S3/S4 make the profile executable.
5. **S6 — DOS translation and declared capability terminals:** recover
   `53:06`, preserve original unsupported cases, and retain `0E/12` terminal
   dispositions.
6. **S7 — package regression/native profile:** one ordered transition,
   descriptor, interrupt/fault, memory and teardown suite, then one native
   profile observation.

## S1 closure

Every `53:00..18` entrance, its original source owner and its explicit
dependency/failure disposition are now classified.  The successor S2 is an
import-boundary proof, not a trace-driven implementation step.
