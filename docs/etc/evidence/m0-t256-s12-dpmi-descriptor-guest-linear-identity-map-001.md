# M0 T256 S12 — DPMI descriptor and guest-linear identity map

## Decision

The remaining descriptor prerequisite is **not** an OpenNT DPMI leaf and it
does not select a Bochs patch yet.  The imported OpenNT x86 body installs
descriptors into the NT4 *host process* LDT, whereas the adopted Bochs CPU
already owns a real guest GDT/LDT/LDTR implementation.  They cannot be joined
by copying `FlatAddress[]`, `Ldt`, `IntelBase`, or a host pointer across the
boundary.

The next admission must decide a selector-blind, guest-resident descriptor
table lifecycle (including the GDT descriptor which names an LDT), or record
why original guest setup can establish it without one.  No `53:xx` ingress is
enabled by this map.

## Source map

| Source | Historical contract | What it proves | Current disposition |
| --- | --- | --- | --- |
| `dpmi32/dpmi32.c:34,44..49` | `FlatAddress[LDT_SIZE]` and a dispatch whose first entry is `DpmiSetDescriptorEntry`. | Selector base lookup is a package-global host-address shortcut, not a guest CPU table. | Do not import as a host-address table. |
| `dpmi32/dpmi32.c:DpmiPassTableAddress` | Stores `Ldt = Sim32GetVDMPointer(getAX() << 16, ...)` and `IntelBase = (ULONG) Sim32GetVDMPointer(0, 1, FALSE)`. | Both identities are dereferenceable NT4-process pointers. | Source-derived bx-vdm representation is required; neither value may cross the modern ABI. |
| `dpmi32/dpmiselr.c:DpmiSetDescriptorEntry` | Reads descriptor bytes from guest `ES:BX`; derives base/limit; updates `FlatAddress[]`; then calls `DpmiSetX86Descriptor` on x86. | A real descriptor-byte source and CPU descriptor lifecycle must agree before any DPMI service can use selectors. | Preserve ordering only after guest-table lifecycle exists; do not make mantle interpret selector/DPMI policy. |
| `dpmi32/i386/dpmi386.c:DpmiSetX86Descriptor` | Calls `NtSetLdtEntries`; for longer ranges calls `NtSetInformationProcess(ProcessLdtInformation)`. | OpenNT's x86 CPU-visible selector state was an NT4 process-LDT product dependency, not a reusable public modern composition seam. | Cannot be linked into bx-vdm unchanged.  Its validation/order are source reference for a later source-derived provider. |
| `dpmi32/i386/dpmi386.c:Sim32pGetVDMPointer` | Uses `FlatAddress[selector >> 3] + offset` and returns `PUCHAR`. | Original DPMI source expects selector translation to produce a native pointer. | Replace each use with bounded copied access or a bx-vdm record identity; never expose Bochs RAM/host pointers. |
| `bx-core/cpu/protect_ctrl.cc:LLDT_Ew` | Native CPU validates a GDT-resident LDT descriptor and fills LDTR cache. | Bochs already owns architectural LDT load/validation; it must not learn DPMI/BOP vocabulary. | Retain native owner. |
| `bx-core/cpu/segment_ctrl_pro.cc:fetch_raw_descriptor` | Native selector lookup reads descriptor bytes through guest `gdtr` or `ldtr` cache base/limit. | The required table is guest-resident RAM plus native CPU state, not an adapter table. | Retain native owner. |
| `bx-core/cpu/init.cc:922..934` | Reset initializes an LDTR cache at base zero with byte limit `0xffff`. | Reset state is not evidence that a DPMI guest GDT/LDT lifecycle has been established. | No implicit DPMI admission. |

## Ownership and minimum sequence

1. `bx-vdm` retains the original DPMI package's selector policy, register ABI,
   source ordering and failure disposition.  It may hold non-pointer records;
   it must not retain a duplicate CPU descriptor cache.
2. `bx-mantle` may only offer a future typed, selector-blind lifecycle if it
   can be expressed without DPMI, DOS, BOP or selector policy.  Ordinary RAM
   reservation (`BX-MANTLE-097`) is capacity only; it does not create a GDT,
   LDT or LDTR identity.
3. `bx-core` remains the exclusive owner of descriptor parsing, GDT/LDT limit
   checks, LLDT, segment loads, faults and cache state.  No current source fact
   selects a core intrusion.
4. Before a source mirror imports `dpmiselr.c`, the next package must prove
   one of two routes: (a) original guest setup writes a valid GDT/LDT and uses
   native instructions to load it, or (b) a narrowly typed mechanical startup
   lifecycle can establish those tables without encoding DPMI semantics.
5. Only after that decision may the existing XMEM ordinary-RAM record adapter
   be connected to a complete `53:00..18` owner-package recovery.  Paging,
   IDT/IRET/fault delivery and protected guest control transfer remain separate
   prerequisites.

## Explicitly rejected shortcuts

- Treating `FlatAddress[]` as a guest-linear map or mirroring it into mantle.
- Casting guest addresses, `Ldt`, `IntelBase`, or `XMEM_BLOCK *` to host
  pointers on either x86 or x64.
- Calling `NtSetLdtEntries` or `NtSetInformationProcess` from the modern CLI.
- Adding a DPMI-aware `set_ldtr`, descriptor-cache copy, arbitrary CR0 setter,
  or `53:00` entry while table/interrupt/IRET lifecycle remains unproven.

## S12 closure

S12 closes the descriptor/guest-linear identity question as an evidence map.
It proves that the reservation and XMEM records completed by S8--S11 are
necessary but insufficient.  The sole next bounded question is native
guest-resident descriptor-table lifecycle admission; this document makes no
DPMI runtime or provider claim.
