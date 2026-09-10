# M0 T256 S1 — Protected Virtual Access and Mapping Call Map

## Scope

This map answers whether retained Bochs primitives can be assembled into a
copied, fault-contained protected virtual-range action without importing
provider semantics.

## Native path map

| Layer | Existing path | Property | Result for a host-facing copied action |
| --- | --- | --- | --- |
| Segment rights and limits | `BX_CPU_C::read_virtual_checks` / `write_virtual_checks` in `access.cc` | Returns boolean, but only checks the current native segment cache and intended byte range. | Useful building block only; it does not translate all pages or copy bytes. |
| Address formation | `get_laddr32` | Computes base plus offset. | Insufficient alone: no access or page validation. |
| Normal virtual read/write | `read_virtual_*_32` / `write_virtual_*_32` → `access_{read,write}_linear` | Correct architecture path, but invalid segment, alignment and page failures invoke `exception()`/`page_fault()`, whose control contract may longjmp from CPU execution. | Not callable as a synchronous provider/mantle API. |
| Debug translation | `dbg_xlate_linear2phy` in `paging.cc` | Returns boolean instead of delivering a CPU fault.  It walks mappings, but is debug-oriented and does not express requested read/write privilege, accessed/dirty behavior, alignment, atomic multi-page semantics, or normal exception state. | Explicitly rejected as a product access primitive. |
| Physical copied access | `BX_MEM_C::copy_{from,to}_ordinary_ram` and backing equivalents | Bounded copied bytes with no host mapping export. | Suitable only after a new no-fault virtual preflight yields a complete physical span list. |

## Minimum mechanically valid shape

The future operation must be a bounded fixed-width record, not an adapter
callback or pointer:

1. copied request: current segment index, offset, byte count, read/write
   direction and copied write bytes;
2. core-owned preflight: validate segment type/limit and every affected page
   with normal access permissions, but return a typed failure instead of
   entering guest fault delivery;
3. only after all spans validate, perform physical copied reads/writes;
4. copied result: success/failure and copied read bytes; no linear/physical
   address, host page, descriptor cache or Bochs object crosses outward.

The required transaction is all-or-nothing for writes.  A partially completed
multi-page write followed by a page fault would not be source-compatible with
the declared provider call contract.

## Original-source consumers

The retained DPMI sources use this missing ability broadly, rather than at one
service leaf:

- `i386/dpmi386.c` obtains protected stack/register blocks through
  `Sim32GetVDMPointer` before switching context;
- `dpmiint.c` constructs and consumes protected interrupt/IRET frames;
- `dpmimem.c` returns information records through protected ES:DI; and
- `int21map.c` maps parameter and user buffers across DPMI↔DOS transitions.

Therefore a single `53:xx` handler cannot own a temporary substitute.

## Mapping-identity consequence

The original `xmem.c` representation publishes host virtual addresses and
`XMEM_BLOCK *` values through register pairs.  The new mechanical access
operation cannot solve that representation.  A later bx-vdm compatibility
shim must retain original allocator/order/failure behavior while assigning
guest-linear identities backed by an independently declared machine mapping
resource.  It cannot use host pointers or the existing opaque host-handle
manager, whose purpose is external host resource lifetime rather than guest
memory mapping.

## S1 decision

No existing public native primitive is safe to expose directly.  S2 may only
design a no-fault preflight/copy transaction after separately proving it can
share normal Bochs access semantics without duplicating page-table or
descriptor policy.  The design must remain selector-blind and contain no
DPMI/OpenNT/DOS/BOP vocabulary.
