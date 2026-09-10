# M0 T310 S8 P4 EMM Storage Contract Closure

## Original Package and Problem

`softpc.new/base/dos/emm_mngr.c` is the original Expanded Memory Manager
algorithm. Its per-EMS-handle `handle[]` table is explicitly documented as a
host-dependent storage ID and is accessed only through original
`host_allocate_storage`, `host_reallocate_storage`, `host_free_storage`,
`USEBLOCK`, and `FORGETBLOCK` operations.

The NT4 declaration used `long` for both that private pointer-like ID and the
unrelated `backfill` byte count. This worked only because native pointers and
`long` were both 32 bits. On x64 it truncates every EMM handle backing pointer.

## Recovery

- The `handle[]` declaration is `IHP`, the original SoftPC native host-pointer
  carrier used throughout the allocation/access calls.
- `backfill` is split out as `ULONG`, because it is a fixed-width byte count
  copied from `LIM_CONFIG_DATA`, never a pointer.
- Existing `<stdlib.h>` coverage supplies native `malloc/free` declarations;
  no allocator, mapping manager, adapter, or EMM algorithm was added.

This is `MVDM-HOST-DIV-061`, now matching both its README registration and the
selected source implementation. Private EMM storage never crosses a guest or
component ABI, so using the session mapping manager here would be incorrect.

## Verification

Formal Ninja MSVC `/MT /W4` compilation of the complete original
`emm_mngr.c` translation unit succeeded on x64 and x86. Both logs contain zero
instances of the former `C4013`, `C4047`, `C4311`, or `C4312` EMM
pointer/allocation diagnostics. Remaining non-width diagnostics stay visible.
