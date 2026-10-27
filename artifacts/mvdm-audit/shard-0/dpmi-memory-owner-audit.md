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
