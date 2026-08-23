# M0 T256 S13 — Native descriptor-table lifecycle admission

## Decision

No new bx-core or bx-mantle lifecycle ABI is admitted.

The retained DOSX guest does not establish its descriptor table solely through
architectural `LGDT`/`LLDT` instructions.  Its `selGDT` is a historical
selector alias to the NT4 process LDT.  Guest code reads and writes descriptor
bytes through that alias, then uses `53:00` to ask the NT4 host to install the
same bytes in the host process LDT.  Replacing that product contract with a
mantle descriptor-table initializer would put DPMI selector policy into the
machine layer and create a second descriptor implementation.

Bochs already supplies the architectural half: native LGDT, LLDT, GDT/LDT
lookup, limit validation and faults.  The remaining work is a source-derived
DPMI/DOSX composition problem, to be solved only with the complete protected
context, interrupt/IRET and bounded guest-memory requirements.  This S makes
no provider or ingress claim.

## Guest and host source chain

| Source | Fact | Consequence |
| --- | --- | --- |
| `dpmi/486/dxstrt.asm` | Builds DOSX state, then invokes `FBOP BOP_DPMI,InitDosx`. | The DPMI package starts as a coordinated guest/host lifecycle, not a leaf service. |
| `dpmi/486/dxutil.asm` | `AllocateSelectorBlock`, `FreeSelector`, `FindSelector` and `NSetSegmentDscr` load `es,selGDT`, inspect/write descriptor slots, and invoke `SetDescriptorTableEntries` (`53:00`) after mutations. | `selGDT` is an operational memory alias whose lifetime and table policy belong to DPMI/DOSX. |
| `dpmi/486/dxmain.asm` | Uses `selGDT` for descriptor-base inspection during transfer services and calls `ParaToLDTSelector`. | A static, empty or reset-base LDT does not meet the guest's intended contract. |
| `dpmi32/dpmi32.c:DpmiPassTableAddress` (`53:0F`) | Captures a `Sim32GetVDMPointer` pointer to the table named by the 16-bit `selGDT`. | Its native pointer is historical composition state, not a value that a current engine can return. |
| `dpmi32/dpmiselr.c:DpmiSetDescriptorEntry` + `i386/dpmi386.c:DpmiSetX86Descriptor` | Reads guest descriptor bytes then modifies the NT4 process LDT with `NtSetLdtEntries` / `NtSetInformationProcess`. | Original ordering can guide a future source-derived provider; the NT4 mechanism itself is unavailable and must not be relinked. |
| `bx-core/cpu/protect_ctrl.cc:LGDT_Ms,LLDT_Ew` | Native CPU consumes guest-resident pseudo-descriptor/GDT entries and fills GDTR/LDTR state after architectural validation. | The adopted CPU is the only owner of CPU descriptor state; no patch is selected. |

## Rejected candidate seams

| Candidate | Rejection |
| --- | --- |
| Mantle API that creates/loads an LDT from DPMI descriptors | It necessarily decides `selGDT`, selector allocation, descriptor mutation and DPMI failure behavior. Those are not selector-blind machine mechanics. |
| Copy `FlatAddress[]` / `Ldt` into a bx-vdm cache | It duplicates the CPU table, preserves host-pointer assumptions and will diverge from native Bochs validation. |
| Use the reset GDTR/LDTR bases as an implicit DPMI table | Reset values are generic CPU state; they do not publish a valid DOSX table or preserve guest startup/teardown semantics. |
| Call NT4 LDT system services | `NtSetLdtEntries` and `ProcessLdtInformation` describe the old host-process model and are not a portable public host capability for the CLI. |

## Exact transfer boundary

The later DPMI owner package must recover the table as a **single source
package**, with these ordered subcontracts:

1. DOSX table publication and bounded byte access, replacing the old
   `Sim32GetVDMPointer` representation without exposing a backing pointer.
2. Source-shaped descriptor update/selector allocation with native Bochs
   architectural validation as the authority.
3. Protected context transition, IDT/fault/IRET path, and cleanup/re-entry;
   the `dpmimscr.c` source explicitly warns that applications may change GDT
   and LDT unpredictably outside the DPMI server.
4. Only then XMEM record allocation/reallocation can become reachable through
   a complete `53:07/08/09` package path.

This is a dependency result, not a transfer of responsibility to bx-core or
bx-mantle.  Any later external-code change still requires its own registered
exception before implementation.

## S13 closure

S13 closes the lifecycle admission question: the correct result is **defer to
the whole DPMI/DOSX owner package, with native Bochs descriptor semantics
retained**, not a new mechanical API.  The next remaining substrate question
is protected interrupt/fault/IRET continuity and is separately source-mapped
before any `53:xx` route may open.
