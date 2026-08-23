# M0 T257 S2 — DOSX/DPMI Source Import Ledger

## Question

Which original non-WOW DOSX/DPMI files may later enter the `bx-vdm` product
source mirror, which historical dependencies require a named compatibility
seam, and which legacy product imports must be refused before a `53:xx`
service can become active?

## Inputs

- T257 S1 [owner/ABI/failure map](../evidence/m0-t257-s1-dpmi-dosx-owner-abi-failure-map-001.md).
- OpenNT host source: `src/opennt/base/mvdm/dpmi32/` and its `i386/` child.
- OpenNT guest source: `src/opennt/base/mvdm/dpmi/486/`.
- Current formal source-membership authority:
  `tools/build/t225-s7-full-module-manifest.json`.
- Existing selector-blind reservation and XMEM-record evidence from T256.

## Import Contract

The future host mirror root is `src/bx-vdm/bop/opennt/dpmi/`; named seams
belong in `src/bx-vdm/bop/shim/`.  S2 deliberately creates neither directory
nor a product route: no DPMI source file is currently in the formal build.
The `dpmi/486/*.asm` files remain guest build/staging inputs.  They are not
translated into `bx-vdm`, and `bx-core`/`bx-mantle` remain DPMI-blind.

Every later copied translation unit starts as an exact OpenNT copy.  A change
inside that copy needs a `DIVERGENCE:` comment naming the unavailable NT4
composition dependency and the smallest preserving shim.  A shim must not
provide raw guest pointers, a host LDT, a copied descriptor cache, a generic
fault callback, or an adapter-owned protected-mode emulator.

## Host Translation-Unit Ledger

| Original file | Later role | S2 disposition | Required named boundary before build admission |
| --- | --- | --- | --- |
| `data.c`, `debug.c`, `dpmidata.h`, `dpmi32p.h`, `precomp.h` | package data, layouts and common declarations | exact-copy candidate, package-coupled | `dpmi_session_shim` must replace `VdmTib`/context macros; no independent activation. |
| `dpmi32.c` | dispatcher, DOSX shared-data and table/stack publication | source-derived composition | `dpmi_guest_memory_shim` for bounded copies; `dpmi_session_shim`; no `IntelBase`, `Ldt` or `FlatAddress[]` identity. |
| `dpmiselr.c`, `xlathlp.h`, `i386/dpmi386.c` | selector/table update and fast BOP entry | source-derived composition | native-descriptor disposition shim plus bounded guest table transfer. `NtSetLdtEntries` and `NtSetInformationProcess(ProcessLdtInformation)` are refused. |
| `modesw.c`, `register.c` | DOSX/app/mode handoff and register state | source-derived composition | copied fixed-width CPU-state request/result only; no direct `VdmTib` context or descriptor-cache access. |
| `dpmiint.c`, `dpmiint.h`, `stack.c`, `dpmimscr.c` | protected interrupt/fault/locked-stack/IRET lifecycle | source-derived composition after S3 | `dpmi_frame_session_shim`; direct `NtVdmControl`, `VdmInstall*`, and emulator-IRET hook imports are refused. Native Bochs remains fault/IRET owner. |
| `xmem.c`, `dpmimemr.c`, `i386/dpmimem.c` | XMEM allocation/free/reallocation/query | source-derived composition after startup/frame admission | existing `dpmi_xmem_record_adapter`; `Vdm*VirtualMemory` and `Nt*VirtualMemory` product imports are refused. |
| `buffer.c`, `int21map.c`, `savestat.c` | protected-mode to DOS INT 21 frame and buffer translation | source-derived composition after protected frame admission | `dpmi_guest_memory_shim` plus existing DEM/NTDOS transactional provider boundary; no unchecked `Sim32GetVDMPointer`. |
| `vxd.c` | VCD/VDD service | explicit unavailable terminal | No replacement in T257.  The VDD/VCD owner package must be admitted before any success path. |

## Guest Source Ledger

| Original guest module(s) | Role | Disposition |
| --- | --- | --- |
| `dxboot.asm`, `dxstrt.asm`, `dxmain.asm`, `dxfunc.asm`, `dxutil.asm` | DOSX bootstrap, BOP installation, selectors and shared data | original guest source build/stage input; S3 validates its layout before host startup/table composition. |
| `dxint31.asm`, `dxintr.asm` | INT 31 services, protected interrupt/fault continuations | original guest source build/stage input; S4 validates the guest stack/frame ABI before host composition. |

## Refused Historical Product Imports

The following names are evidence of the old NT4 product shell, not an import
surface for this CLI composition.  They must not appear in an admitted
unchanged DPMI mirror translation unit without its listed, reviewed
source-derived seam:

| Historical dependency | Original locations | Disposition |
| --- | --- | --- |
| `Sim32GetVDMPointer`, `IntelBase`, pointer-valued guest frame records | `dpmi32.c`, `buffer.c`, `int21map.c`, `stack.c`, `dpmiint.c`, `i386/dpmi386.c` | replace only by bounded copy/read/write calls in `dpmi_guest_memory_shim`; never expose a host pointer. |
| `VdmTib`, `VdmContext`, `PmStackInfo` | common headers, `dpmi32.c`, `dpmiint.c`, `dpmimscr.c` | session-owned fixed-width state record in `dpmi_session_shim`; no thread-local NT4 structure recreation. |
| `Ldt`, `FlatAddress[]`, `NtSetLdtEntries`, `NtSetInformationProcess(ProcessLdtInformation)` | `dpmiselr.c`, `register.c`, `dpmiint.c`, `i386/dpmi386.c` | native Bochs remains descriptor authority.  No x64 process-LDT compatibility façade. |
| `NtVdmControl`, `VdmInstallHardwareIntHandler`, `VdmInstallSoftwareIntHandler`, `VdmInstallFaultHandler`, `EnableEmulatorIretHooks`, `DisableEmulatorIretHooks` | `dpmiint.c`, `dpmimscr.c`, `dpmiselr.c` | refuse direct import.  S4 may recover only the OpenNT policy/order through a declared DPMI frame session contract. |
| `NtAllocateVirtualMemory`, `NtFreeVirtualMemory`, `VdmAllocateVirtualMemory`, `VdmFreeVirtualMemory`, `VdmReallocateVirtualMemory`, `VdmQueryFreeVirtualMemory` | `dpmimemr.c`, `i386/dpmimem.c` | map only through the existing opaque XMEM record adapter and ordinary guest-RAM reservation lifecycle. |
| `ThreadSetDebugContext` | `i386/dpmi386.c` | explicit `53:0E` deferred failure until Debugger/VDD ownership is admitted. |

## Procedure and Observations

1. Enumerated every host `dpmi32` translation unit and the seven guest DOSX
   modules in the selected profile.
2. Searched the host source for its known NT4 composition imports.  The
   results locate direct-pointer usage in the dispatcher, DOS translation and
   interrupt code; process-LDT calls in `i386/dpmi386.c`; old virtual-memory
   calls in the memory units; and old VDM hook calls in the frame unit.
3. Checked `tools/build/t225-s7-full-module-manifest.json`: no source below
   `src/bx-vdm/bop/opennt/dpmi/` is a current product member, and that mirror
   directory does not exist.  Thus the listed imports cannot become linked by
   accident during S2.

## Interpretation

No DPMI unit is honestly “compile unchanged in isolation”: the common OpenNT
headers encode the historical NT4 VDM representation.  That does not justify
a rewrite.  It fixes the later recovery sequence: exact-copy the source as a
package, introduce only the named boundaries above, and preserve original
control order and explicit failure terminals.  Descriptor, CPU exception and
IRET mechanics stay native Bochs mechanics; DPMI policy stays in `bx-vdm`.

## S2 Exit Result

This ledger gives S3 a fixed source selection and prevents ad-hoc NT4 import
choices.  It admits no `53:xx` ingress, runtime claim or external-code
intrusion.  S3 may start only with a source identity copy plus the declared
startup/table seams.
