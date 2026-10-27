# XMS block request source boundary

`xms.486/xmsblock.c` remains partial because of its separately registered
guest-memory move adaptation. The unchanged original allocation/free/realloc
functions call the shared `ExtMemSA` suballocator. A failed `SAAllocate` or
`SAFree` returns AX=0 to the guest; failed reallocation returns CX=0. The
separate [suballocator audit](../shard-2/suballoc-xms-callback-failure-review.md)
shows injected failure bookkeeping observations and selected callback owners.

Two original-source branch invariants need direct validation. In
`xmsAllocBlock`, a zero DX bypasses `SAAllocate` yet the function reads local
`BaseAddress` for its return. In `xmsReallocBlock`, equal old/new sizes bypass
`SAReallocate` yet the function reads local `NewAddress`. These are
source-level uninitialized-read candidates, not established guest defects.
The original guest HIMEM source `dos/v86/dev/himem/himem4.asm` checks equal
reallocation lengths and returns before its XMS_REALLOCBLOCK service, but its
shown allocation path calls XMS_ALLOCBLOCK without an adjacent zero-size
check. Whether earlier caller validation, the actual immutable installed
HIMEM binary or another reachable service prevents or permits either input
has not been established. Source-level absence of a check in one function
is not proof of an end-to-end invocation.

Trace actual selected `xmsdisp.c` dispatch, immutable HIMEM provenance and
all guest callers. Test zero size and unchanged-size request on an independent
guest probe plus controlled host entry, with exact return and allocation
state, comparing original contract before selecting any host correction.
The `ALLOC` zero path and the decommit-failure bookkeeping are separate
questions. No original guest, product code or runtime image was changed.
