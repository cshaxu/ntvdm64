# M0 T256 S2 — Non-Paged Protected Range Action Design

## Decision

Admit one deliberately narrow selector-blind operation for implementation:
bounded copied read/write through a current protected segment with paging
disabled, restricted to ordinary RAM.  It is not a general virtual-memory
API, and it does not admit DPMI source bodies yet.

## Native composition

Inside mantle, while the existing minimal machine is active:

1. reject inactive, real/V86, paging-enabled, zero-length, oversized,
   invalid-segment and malformed records;
2. call existing `read_virtual_checks` or `write_virtual_checks` against the
   current native segment cache; those checks return boolean and do not enter
   guest fault delivery;
3. form the native linear address with existing `get_laddr32`;
4. use existing `copy_from_ordinary_ram` or `copy_to_ordinary_ram`, whose
   prior full-span validation rejects A20 aliases, handlers, ROM and devices
   before any byte is copied; and
5. return copied bytes or a typed rejection, never a physical/linear address,
   Bochs object or mapping pointer.

With paging disabled, Bochs' normal translation is linear→physical (subject to
A20); the ordinary-RAM validator provides the conservative finite backing
domain.  A disabled A20 alias is rejected instead of silently changing the
address domain.

## Explicit limits

- `CR0.PG=1` is rejected.  The debug page-table walker is not reused.
- MMIO/ROM/handler-backed spans are rejected, not proxied.
- The operation does not alter descriptor tables, page tables, CR0, CPU
  registers, FLAGS, interrupt state or mapping allocation.
- The operation contains no provider, service, selector-byte, DPMI, OpenNT,
  DOS or BOP vocabulary.

## Atomicity

The existing ordinary-RAM copying functions validate the complete range before
their first byte copy.  Therefore accepted writes cannot commit one page and
then fail at a later page.  Segment validation also occurs before physical
copy.  This is sufficient for the bounded non-paged operation; it is not a
claim about paged write atomicity.

## Follow-up disposition

S3 may add the fixed-width mantle record and fixture under `BX-MANTLE-095`.
Guest-linear allocation/mapping identity and paged access remain separate
unadmitted prerequisites for the original DPMI package.
