# M0 T256 S10 — XMEM Record Adapter Closure

## Outcome

The bx-vdm-only, source-derived XMEM record adapter is code and fixture
closed. It preserves the OpenNT `xmem.c` allocation-list ownership model while
replacing only the nonportable pointer identity and NT virtual-memory backend.

## Retained and diverged behavior

- A successful allocation obtains a 64 KiB-aligned ordinary-RAM address,
  records `owner`, size and private allocation identity, then returns copied
  address and nonzero session record ID.
- Unknown/stale IDs are rejected without changing record state.
- `release_owner` walks every matching record, as `DpmiFreeAppXmem` does.
- `reset` releases every record before clearing session state.
- The original `XMEM_BLOCK *` returned through `SI:DI` is replaced by a
  session-local record ID. The private mantle allocation ID stays inside
  bx-vdm; no host pointer or host allocation is introduced.

## Formal evidence

Fresh graph: `build/M0-T256-S10/formal-r1`.

`t256-s10-xmem-record-adapter-fixture` passes inactive mantle failure,
allocation of two 64 KiB blocks at `00100000` / `00110000`, distinct IDs,
stale-ID rejection, owner sweep, all-record reset and machine cleanup.

## Explicit limit

This is not an active DPMI service. Reallocation/copy, descriptor/LDT
installation, `Sim32GetVDMPointer`, paging and `53:07/08/09` ingress remain
deferred to the complete DPMI context/mapping package.
