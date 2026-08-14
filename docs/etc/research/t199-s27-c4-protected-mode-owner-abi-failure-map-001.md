# M0 T199 S27 C4 Protected-Mode Owner, ABI, and Failure Map

## Question

Can the minimum machine expose a bounded native mode/descriptor capability without recreating OpenNT DPMI policy or importing historical NT process composition?

## Inputs

| Input | SHA-256 | Finding |
| --- | --- | --- |
| `dpmiselr.c` | `9BDD58D63029DE9438633A62934AF2B2B868206CB7F06CC868F537E030FBD234` | DPMI reads descriptors through `Sim32GetVDMPointer`, maintains `FlatAddress`, and applies compatibility policy. |
| `i386/dpmi386.c` | `E47BD6F7A1F48BA94022C9D5ED78714C269F751737F8F69711FC44C06523F8F0` | i386 uses `NtSetLdtEntries`/`NtSetInformationProcess`, VDM mappings and fixed NTVDM state bits. |
| `modesw.c` | `AA7157E2A39FEAD41E42E87F87E332E9464BF69A668671DE976C69637CE06A67` | Mode switch needs DOSX stack/segment preparation plus historical workarounds. |
| `dpmidata.h` | `F9772482513601C2E316E097A92485864557AF77010E77285D725D903A0FD7B2` | State includes LDT records, application flags and locked protected-mode stack lifetime. |
| `src/bx-core/cpu/cpu.h` | `E4409B4C5562CBEFBF0B85994DC8B5E817AE5F39AA9BE21DE48902F7DD4BCDDE` | Bochs owns CR0, segment caches, GDTR/IDTR/LDTR/TR and native transitions. |
| `src/bx-core/cpu/crregs.cc` | `3B913B1D3ED85C09718E613EEEAD984749BEACB91DDA85EFFA247C8E963D3A6D` | CR0 mutation is instruction/CPU semantics, not a host-service request surface. |

## Dependency Map

| DPMI requirement | Historical coupling | Native fact | Disposition |
| --- | --- | --- | --- |
| `53:00,01,03,04,05,0F,11` descriptors/selectors/mode | Guest descriptor bytes, `FlatAddress`, LDT policy, process LDT APIs and application state | Bochs has architecture state but no external lifecycle ABI | Not admissible. |
| Real/protected transitions | DOSX frame and stack, `VdmTib`, feature bits and fixed NTVDM state | Guest can execute architectural transitions; direct host switch bypasses guest semantics | Not admissible. |
| Fault/IRET/debug | DPMI frames, IRET hooks, stacks, IDT/IVT and process context | Native faults are core-owned; C5 is deferred | Not admissible. |
| DPMI extended memory | Session lifetime and flat mapping | C2 only provides opaque native aperture blocks | Deferred. |
| `53:12` VCD | VDD/serial product integration | No contained equivalent | Explicitly unavailable. |

## Admission Decision

**C4 is deferred, not implemented.** A selector-blind record can observe CPU mode or validate a fixed mechanical range, but it cannot provide arbitrary DPMI descriptor/mode mutation: selector policy, LDT lifetime, protected stacks, fault frames and transition ordering are DPMI component semantics. Exposing Bochs internals to `bx-vdm` reverses dependency; duplicating `dpmiselr.c` in mantle or adapter creates a second DPMI implementation. No CR0 setter, LDT wrapper, descriptor table, host pointer, NT API, adapter selector knowledge, or bx-core patch is admitted.

## Follow-up

Keep the complete DPMI package deferred except `53:12` unavailable. Consolidate the complete machine-capability decisions into package-level dispositions; do not promote individual DPMI BOPs from trace.
