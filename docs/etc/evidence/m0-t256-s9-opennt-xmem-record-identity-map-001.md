# M0 T256 S9 — OpenNT XMEM Record-Identity Map

## Scope

This is a source/ABI map only. It does not admit a `53:xx` route, an imported
DPMI body, a host pointer, or a Bochs change.

## Original facts

`src/opennt/base/mvdm/dpmi32/xmem.c` keeps a process-local circular list:

```c
typedef struct _Xmem {
    PVOID Address;
    ULONG Length;
    struct _Xmem *Prev, *Next;
    WORD Owner;
} XMEM_BLOCK;
```

`DpmiAllocateXmem` reads requested bytes from `BX:CX`, calls
`DpmiAllocateVirtualMemory`, allocates the list record with `malloc`, inserts
it, then returns `Address` through `BX:CX` and the *record pointer* through
`SI:DI`. `DpmiFreeXmem` and `DpmiReallocateXmem` reconstruct that pointer from
`SI:DI` and dereference it. `DpmiFreeAppXmem` and `DpmiFreeAllXmem` walk the
same list.

On x86, `i386/dpmimem.c` implements the allocator through
`NtAllocateVirtualMemory` / `NtFreeVirtualMemory`; on a historical NTVDM the
returned virtual address was simultaneously an address usable by the host
body and by the shared VDM address model. That premise does not exist in the
current Bochs composition, on either x86 or x64.

## Required source-shaped replacement

| Original field / operation | Original observable role | bx-vdm replacement | Disposition |
| --- | --- | --- | --- |
| `XmemHead`, `Prev`, `Next` | Circular lifetime/cleanup list | Session-owned fixed record table or linked records wholly inside bx-vdm. Its ordering, owner sweep and destruction order mirror `INSERT_BLOCK`/`DELETE_BLOCK`. | Adaptable. |
| `Address : PVOID` | Returned allocation base; input to reallocation/free | `uint32_t ordinary_ram_address` returned through original `BX:CX`; it comes only from `BX-MANTLE-097`. | Adaptable; never a host pointer. |
| `Length : ULONG` | Allocated length and resize input | `uint32_t byte_count`, preserving 32-bit request/result truncation rules only where original source does. | Adaptable. |
| `Owner : WORD` | `DX` application/PSP owner for `DpmiFreeAppXmem` | Unchanged `uint16_t owner`; populated and compared in the same source order. | Adaptable. |
| `XMEM_BLOCK *` split to `SI:DI` | Private allocation identity used by `Free`/`Reallocate` | Session-generated **32-bit opaque record ID**, split/combined only at the bx-vdm ABI boundary. It maps to the mantle opaque allocation ID internally. | Required divergence, with local comments. |
| `malloc/free(XMEM_BLOCK)` | Host heap metadata | Fixed bx-vdm session records; no pointer crosses provider ABI. | Required divergence. |
| `DpmiAllocateVirtualMemory` | Host NT virtual allocation and returned PVOID | Selector-blind mantle allocate record over an explicitly admitted ordinary-RAM interval. | Required replacement. |
| `DpmiFreeVirtualMemory` | Host release of PVOID | Mantle release by its opaque ID, only after bx-vdm resolves the session record. | Required replacement. |
| `DpmiReallocateVirtualMemory` | Allocate/copy/free native virtual blocks | Later bx-vdm orchestration: reserve new, checked copied transfer, release old, then preserve original result/failure ordering. | Deferred pending copied transfer policy and full DPMI context. |

## ABI boundary rules

1. Mantle's ID is never returned in `SI:DI`, stored in an OpenNT-shaped record
   visible outside bx-vdm, or interpreted as an address.
2. The bx-vdm record ID is never a native address. It is monotonically
   allocated, session-local, nonzero, and rejected if stale or unknown.
3. `BX:CX` carries only a 32-bit ordinary-RAM address. It cannot be used as a
   direct host dereference; all later data movement uses bounded copied
   mantle actions.
4. The existing OpenNT list semantics are retained where externally
   observable: successful allocation inserts before returning registers;
   failed allocation sets carry and leaves no record; free/reallocate reject
   an unknown record before any mantle operation; owner/all cleanup follow
   original traversal order.

## Remaining blockers and ordering

The record adaptation itself can be admitted next as bx-vdm-only source-derived
code. It does **not** make an active DPMI provider complete:

1. stage admission must select a nonzero explicit interval;
2. bx-vdm record ↔ mantle opaque-ID adapter and local lifecycle regression;
3. copied move/reallocation semantics;
4. protected descriptor/LDT ownership and guest-linear mapping identity; then
5. only the complete `53:07/08/09` owner slice may be considered for ingress.

Paging, `Sim32GetVDMPointer`, `FlatAddress[]`, `IntelBase`, native LDT pointer
access, and the remaining DPMI lifecycle remain separately deferred. This map
does not authorize recreating the historical CCPU/SAS shared address space.
