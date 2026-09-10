# M0 T199 S23 C2 Extended-Memory Capability Admission

## Source Finding

OpenNT `xmsblock.c` implements XMS allocation through historical
`SAAllocate`, `SAFree`, and `SAReallocate` over `ExtMemSA`; it returns KiB
bases and moves historical linear addresses. It is not independently linkable
and does not describe a direct Bochs physical-RAM allocator.

Bochs owns the usable primitive: `BX_MEM_C::get_memory_len`, checked
ordinary-RAM range predicates/copies, and lazy one-MiB backing-block creation.
`get_vector` and `allocate_block` cannot cross a component boundary because
they expose a raw host pointer and perform allocation without an XMS lifetime
contract.

## Admission Decision

C2 will be a source-derived mantle capability, not a direct link of
`xmsblock.c`: fixed-width allocation/free/reallocation/query/move requests;
opaque numeric handles; physical offset/length results; no host pointer; and
all moves use the existing checked ordinary-RAM copy owner. The first aperture
starts at 1 MiB and ends at `get_memory_len`; it rejects a machine with no
extended aperture. XMS translation remains deferred until C2, C3, C5 and the
whole XMS provider package are complete.

## Required Exception And Tests

A new mantle exception must name the allocator table and lifecycle gate before
implementation. Tests must prove non-overlap, handle invalidation, resize
atomicity, copy bounds, no allocation below 1 MiB, and rejection leaves state
unchanged. No BOP, OpenNT source, selector, or adapter pointer is admitted.
