# DPMI extended-memory owner and remaining provider selection

`dpmi32/dpmimemr.c` selects the original shared XMS suballocator directly
under CPU40 for allocation, free and resize (DIV-224). The original NT4 path
first calls the VDM allocator, falling back to the same SA functions only on
`STATUS_NOT_IMPLEMENTED`. The CPU40 XMS initialization reserves the enclosing
extended-memory region before this entry; the generic VDM free list and the
SA free list therefore answer different questions. The original SA allocator
is reused; the changed part is provider selection, not a new allocator.

The independently computed retention metric is 117 of 121 original
uncommented nonblank source lines (96.69%); it is not a measure of restored
behavior. The cached worker map attributes all four allocate/free/resize/query
entrypoints to `original-mvdm-dpmi32:dpmimemr.obj`. Source and map identity do
not establish current-package execution.

T420 S36's recorded guest witnesses include failed growth with preserved data,
successful forced relocation and explicit free, plus four children exiting
with allocations and the parent checking capacity restoration. Those witnesses
used the historical worker hash recorded in that evidence, not this S10
package. They are stronger than a successful allocation alone but do not
establish every current call, simultaneous XMS/DPMI use or broker teardown.
The BOOL failure correction in `xmem.c` is separately registered DIV-272;
it must not be counted as an implementation inside this file.

The current `DpmiGetMemoryInfo` still invokes `VdmQueryFreeVirtualMemory`
before unconditionally overwriting its output with `SAQueryFree` for CPU40.
The selected `nt_mem.c` implementation asserts initialization and walks its
free-list headers before returning; this is extra work with a distinct owner,
not evidence that the result is used. No failure or runtime regression is
established by this static observation. Audit initialization and diagnostic
side effects, compare original fallback ordering, and test current real guest
query, failure and XMS/DPMI interleaving before deciding whether this call can
be removed or its provider boundary can be restored. Preserve the approved
original behavior unless evidence establishes a safe narrower change. No
product or guest source changed in this audit.

## Current original suballocator fixture

The existing `suballoc_lifecycle_fixture.c` was rebuilt from current
`src/mvdm/suballoc/suballoc.c` with the generated x86 Ninja fixture graph at
`build/.ninja/msvc-x86-suballoc-audit`; source SHA-256
`B06496E16D551F2BBAC37D896B61857C6569EDA8C98FB3D26DBF574E594148E0`,
fixture executable SHA-256
`D2AFCE5A5C53B7A462CE538D82A691098B043DAD1C07455CD0B6DDCA28FC4A91`.
The normal mode passed its growth/shrink, overlap, relocation, exhaustion,
coalescing and first-commit-failure checks. The explicitly injected failure
audit returned zero after observing its stated original behavior: a partial
commit can leave a page committed while the allocation bitmap remains free;
a failed decommit can leave the corresponding bitmap range marked free.
These latter markers are limitation observations, not passing allocation
semantics. The fixture uses supplied mock memory callbacks and does not prove
the real CPU40 callbacks can fail in those ways or that any guest encountered
the condition. `suballoc/suballoc.c` is accounted for separately in shard 2.
No product or guest image was changed.
