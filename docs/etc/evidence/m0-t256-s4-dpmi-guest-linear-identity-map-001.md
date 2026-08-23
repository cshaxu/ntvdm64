# M0 T256 S4 — DPMI Guest-Linear Identity Map

## Question

Can the retained OpenNT `dpmi32` source use the closed T256 S3 copied
protected-range action as its memory identity, or can it otherwise be linked
without changing its pointer model?

No.  The source has one historical identity which is valid only because the
NT4 VDM and the source process share a 32-bit flat address space.  Bochs guest
linear memory, host virtual memory, and host implementation records are three
different domains.

## Original identity and consumers

| Original source | Historical representation | Required observable effect | Current disposition |
| --- | --- | --- | --- |
| `dpmi32/i386/dpmimem.c:38..168` | `NtAllocateVirtualMemory(NtCurrentProcess(), ...)` returns a host virtual address through `PVOID *Address`. | The address is presumed simultaneously usable by native OpenNT code and the DOS extender. | Not reusable: this allocates process memory, not Bochs guest RAM. |
| `dpmi32/xmem.c:55..229` | `ULONG BlockAddress` is returned in `BX:CX`; `PXMEM_BLOCK` itself is returned as a split `SI:DI` handle and later dereferenced. | `53:07/08/09` allocate/free/reallocate an extender-visible base and retain per-PSP ownership. | Needs a source-derived v2 mirror: host pointers cannot become guest values on x64, and even x86 must not expose host pointers into a Bochs guest. |
| `dpmi32/dpmi32.c:151..294` | `Sim32GetVDMPointer` returns direct pointers for shared DOSX structures; `Ldt` and `IntelBase` are host pointers cast to `ULONG`. | `53:04/05/0F` retain DOSX state and descriptor-table identity. | The S3 copied action can transfer bounded bytes but cannot furnish persistent direct pointers or an LDT identity. |
| `dpmi32/i386/dpmi386.c:273..337` | `Sim32pGetVDMPointer` combines `FlatAddress[selector]` and offset, returning a native pointer. | All imported DPMI bodies rely on dereferenceable selector-derived pointers. | Must not be emulated by exposing a Bochs backing pointer. |
| `dpmi32/dpmiselr.c:90..160` | Guest-supplied LDT entries update `FlatAddress[]`; i386 then calls `DpmiSetX86Descriptor`. | Selector base, native CPU descriptor and any DPMI allocation must agree. | Requires native-Bochs descriptor/LDT lifecycle plus a guest-linear allocation record; not supplied by S3. |

The retained CCPU compatibility declaration (`opennt_ccpu_sim32_x86_abi_compat.h`)
is evidence of the same historical pointer contract, not a portable seam: it
maps the request to `c_GetLinAdd` and therefore also assumes direct CCPU/SAS
storage.

## What S3 supplies, and what it does not

S3 (`BX-MANTLE-095`) proves bounded copied reads/writes through a current
native protected segment only while paging is disabled.  It neither returns a
mapping pointer nor allocates a guest range.  It cannot stand in for
`Sim32GetVDMPointer`, `FlatAddress[]`, `IntelBase`, an `XMEM_BLOCK *`, or an
LDT entry.

## Minimum valid recovery shape

The next implementation must remain split by ownership:

1. **`bx-mantle`, selector-blind machine primitive:** a versioned lifecycle
   record that reserves/releases an interval from already configured ordinary
   guest RAM and returns only a 32-bit guest-linear base plus a non-pointer
   opaque allocation ID.  It may not allocate host process memory, expose a
   backing pointer, mention DPMI/selector/DOS, change descriptors/page tables,
   or interpret a BOP.
2. **`bx-vdm`, OpenNT DPMI mirror/shim:** preserve the source `xmem.c`
   allocation, ownership, failure and register-result ordering, but replace
   `XMEM_BLOCK *` and host address casts with a session-owned allocation record
   keyed by the generic opaque ID.  Every original direct guest dereference
   must use explicit bounded copied access; each divergence is locally
   commented and audited.
3. **Later DPMI selector/lifecycle package:** establish native Bochs LDT
   semantics and the mapping from a guest descriptor base to the retained
   allocation.  Paging is a separate disposition; S3 deliberately rejects it.

This is not a request to reimplement a V86/CCPU host address space.  It keeps
machine range ownership in mantle and DPMI/selector policy in bx-vdm.

## S4 decision

No `53:xx` source body is admitted in S4.  A proposed generic ordinary-RAM
range reservation ABI is the next prerequisite; it must be registered before
implementation.  The following failures remain source-shaped until their
owner packages exist: paged mapping, LDT descriptor installation, direct
pointer-returning `Sim32GetVDMPointer`, and the historical host-pointer
`SI:DI` handle.
