# T199 S45 — DPMI owner, ABI and failure map

## Question

What is the complete OpenNT DPMI BOP surface, which machine subsystem owns
each operation, and what must the current non-invasive CLI profile do before a
complete protected-mode composition is admitted?

## Inputs and procedure

The authoritative identity list is `src/opennt/base/mvdm/inc/dpmi.h`:
`BOP_DPMI` is selector `53h`, `MAX_DPMI_BOP_FUNC` is 25, and the file names all
services `00h..18h`.  Call sites and original provider context were read from
`base/mvdm/dpmi/486/dxboot.asm`, `dxfunc.asm`, `dxint31.asm`, `dxintr.asm`,
`dxmain.asm`, `dxmmgr.asm`, `dxstrt.asm`, and `dxutil.asm`.

The current CLI has no admitted DPMI protected-mode server, LDT manager,
fault/interrupt reflector, VDD bridge, or DPMI extended-memory provider.
Consequently this packet does not reimplement a partial DPMI provider in
`bx-vdm`.  It preserves selector/service identification there, assigns the
mechanical owner below, and returns a typed controlled stop for every selected
DPMI BOP.

## Complete identity and disposition

| Slot | Original name | Primary original context | Mechanical owner | CLI disposition |
| --- | --- | --- | --- | --- |
| 00 | `SetDescriptorTableEntries` | `dxutil`, `dxint31` | bx-core CPU/LDT | explicit unavailable |
| 01 | `DPMISwitchToProtectedMode` | `dxutil` | bx-core CPU mode state | explicit unavailable |
| 02 | `SetProtectedmodeInterrupt` | `dxboot`, `dxfunc`, `dxstrt`, `dxutil` | bx-core/bx-mantle interrupt delivery | explicit unavailable |
| 03 | `GetFastBopAddress` | `dxboot` | DPMI composition | explicit unavailable |
| 04 | `InitDosx` | `dxstrt` | DPMI composition/session | explicit unavailable |
| 05 | `InitApp` | `dxfunc` | DPMI composition/session | explicit unavailable |
| 06 | `XlatInt21Call` | `dxmain` | DPMI DOS bridge | explicit unavailable |
| 07 | `AllocXmem` | `dxint31`, `dxmmgr` | bx-mantle extended memory | explicit unavailable |
| 08 | `FreeXmem` | `dxint31`, `dxmmgr` | bx-mantle extended memory | explicit unavailable |
| 09 | `ReallocXmem` | `dxint31`, `dxmmgr` | bx-mantle extended memory | explicit unavailable |
| 0A | `SetFaultHandler` | `dxboot`, `dxstrt`, `dxutil` | bx-core/bx-mantle exception delivery | explicit unavailable |
| 0B | `GetMemoryInformation` | `dxint31` | bx-mantle memory configuration | explicit unavailable |
| 0C | `DpmiInUse` | `dxfunc` | DPMI session | explicit unavailable |
| 0D | `DpmiNoLongerInUse` | `dxstrt` | DPMI session | explicit unavailable |
| 0E | `SetDebugRegisters` | `dxint31` | bx-core CPU debug state | explicit unavailable |
| 0F | `PassTableAddress` | `dxboot`, `dxstrt` | DPMI composition | explicit unavailable |
| 10 | `TerminateApp` | `dxstrt` | DPMI session | explicit unavailable |
| 11 | `InitializePmStackInfo` | `dxboot`, `dxstrt` | DPMI session | explicit unavailable |
| 12 | `VcdPmSvcCall32` | `dxfunc` | VDD/host composition | explicit unavailable |
| 13 | `FreeAllXmem` | `dxstrt` | bx-mantle extended memory | explicit unavailable |
| 14 | `IntHandlerIret` | `dxintr` | bx-core/bx-mantle interrupt delivery | explicit unavailable |
| 15 | `IntHandlerIretd` | `dxintr` | bx-core/bx-mantle interrupt delivery | explicit unavailable |
| 16 | `FaultHandlerIret` | `dxintr` | bx-core/bx-mantle exception delivery | explicit unavailable |
| 17 | `FaultHandlerIretd` | `dxintr` | bx-core/bx-mantle exception delivery | explicit unavailable |
| 18 | `DpmiUnhandledException` | `dxintr` | bx-core/bx-mantle exception delivery | explicit unavailable |

## Boundary and result ABI

`src/bx-vdm/bx_ntvdm_xms_dpmi_plane_v1.c` supplies only the original identity,
owner classification, and profile disposition.  The DPMI session validates the
four-byte `C4 C4 53 service` BOP window and emits `BX_NTVDM_CPU_RESULT_V2_STOP`
for every unavailable identity.  The generic bridge then publishes the typed
controlled stop.  No DPMI selector, service name, LDT, interrupt, fault, RAM,
or VDD behaviour is introduced into Bochs.

This is an explicit unavailable result, not an implementation of DPMI and not
a historical error-code claim.  A future DPMI package must admit a complete
provider and its owner contracts as one package; it must not turn individual
slots into trace-driven adapter patches.

## Verification and confidence

The source-built global package fixture will exercise all 25 identities through
the generic ingress and bound native composition.  It must observe typed stop
for each member, proving no selected DPMI BOP falls through as raw `#UD`.
Confidence is high for identity and ownership boundaries because they derive
directly from `dpmi.h` and the listed original call sites; capability is
intentionally unavailable in this profile.
