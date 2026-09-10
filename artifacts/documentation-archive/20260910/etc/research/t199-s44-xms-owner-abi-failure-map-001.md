# T199 S44: XMS owner, ABI and failure map

## Source inventory

OpenNT `base/mvdm/xms.486/xmsdisp.c` declares the complete twelve-slot XMS
table (`52:00..0B`): A20, move, allocate, free, system page size, total
extended memory, UMB init/request/release, INT15 hook notification, free
extended-memory query, and reallocate.  `xmssvc.h` defines `0C` as the
non-callable sentinel.  The original implementations are in `xmsa20.c`,
`xmsblock.c`, `xmsmisc.c`, and `xmsumb.c`.

## Ownership and contained disposition

| Slot | Original function | Mechanical owner | Current CLI disposition |
| --- | --- | --- | --- |
| 00 | `xmsA20` | bx-mantle A20 capability | adapter register translation; enabled/query handled |
| 01 | `xmsMoveBlock` | bx-mantle RAM copy | deferred until a complete copied move-record ABI exists |
| 02 | `xmsAllocBlock` | bx-mantle extended-memory allocator | handled through opaque mantle handle mapping |
| 03 | `xmsFreeBlock` | bx-mantle extended-memory allocator | handled through opaque mantle handle mapping |
| 04 | `xmsSysPageSize` | bx-mantle memory configuration | deferred: no declared page-size capability record yet |
| 05 | `xmsQueryExtMem` | bx-mantle memory configuration | handled through typed capacity query |
| 06 | `xmsInitUMB` | PC memory/UMB reservation | explicitly deferred; no UMB device/reservation admission |
| 07 | `xmsRequestUMB` | PC memory/UMB reservation | explicitly deferred; no UMB capability |
| 08 | `xmsReleaseUMB` | PC memory/UMB reservation | explicitly deferred; no UMB capability |
| 09 | `xmsNotifyHookI15` | bx-core/bx-mantle interrupt mechanics | deferred; adapter may not register/hook interrupt vectors |
| 0A | `xmsQueryFreeExtMem` | bx-mantle allocator statistics | deferred: total/free/largest query ABI not yet declared |
| 0B | `xmsReallocBlock` | bx-mantle allocator/copy | handled through opaque mantle handle mapping |

The adapter owns only fixed register values and its KiB-base-to-opaque-handle
table.  It must not own guest RAM, A20, UMB discovery/reservation, INT15
handler installation, or Bochs allocation algorithms.  Original XMS code is
normative layout and failure evidence, not a direct modern link input because
it depends on SAS, SoftPC and historical host allocation state.

## Follow-up

The XMS session now makes the six deferred identities explicit typed controlled
stops rather than returning their selected BOP to the CPU as an unhandled
#UD. This is an unavailable disposition, not an implementation of move, UMB,
INT15 hook, page-size or allocator-statistics semantics.

`artifacts/build/t199-s44-xms-family-r2/` is a fresh MSVC x64 `/MT` global
composition witness. It exercises every `52:00..0B` identity through the
native XMS session: A20 and the admitted allocation paths resume, while the
six unadmitted identities stop in the typed boundary. No native guest trace
was run.
