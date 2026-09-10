# T205 S2 P1 — XMS page geometry and allocator statistics

## Question

Can the existing selector-blind extended-memory owner provide the two missing
machine facts needed by original XMS `52:04` and `52:0A`, without exposing
host RAM, a raw mapping, or a new adapter allocator?

## Change and observations

`bx_ntvdm_extended_memory_v1` now has `QUERY_FREE`. It walks its existing
fixed 32-slot, 1-MiB-based allocation table and returns bounded total and
largest contiguous free KiB. It performs no allocation and has no XMS/BOP
vocabulary. The fixed result record carries `free_kib` and
`largest_free_kib` in addition to its pre-existing capacity/address fields.

The XMS package translates `52:0A` to AX total free KiB and DX largest free
KiB, matching `xmsblock.c::xmsQueryFreeExtMem`. `52:04` calls ordinary
user-mode `GetSystemInfo` and returns its page-size value in AX, matching
`xmsmisc.c::xmsSysPageSize`; it does not use host total memory or alter host
state. This API is available in a normal user-mode CLI process.

The strict MSVC x64 `/MT /W4 /WX` XMS package regression was rebuilt and run
from current sources. It verifies page size is nonzero, `52:0A` copies the
stubbed total/largest values into AX/DX, and `52:01,06..09` retain typed
deferred outcomes.

The broad minimal-machine recipe compiled the changed mantle translation unit
but its final link failed on pre-existing omitted CPU/decode objects. This is
not claimed as native verification. The failure is retained so S2 does not
mistake a successful adapter-only test for a Bochs-machine closure.

## Remaining work

`52:01` needs a copied `SS:BP` move-record gather and a checked conventional
to/from extended backing bridge. `52:02/03/0B` still need their full source
error matrix. `52:06..09` remain outside this P and preserve their deferred
dispositions.
