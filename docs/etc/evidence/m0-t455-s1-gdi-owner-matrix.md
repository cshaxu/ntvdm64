# M0 T455 S1 — GDI/DC/drawing owner matrix

**Status:** closed S1 source audit. This is ownership evidence, not a claim that the W07 drawing family is accepted at runtime.

## Method and fixed boundary

The audit reads the selected x86 build graph, the original MVDM providers and each current host binding. `build/.ninja/msvc-x86-release/wow32/build.ninja` selects `walias.c`, `wgdi31.c`, `wgdi.c`, `wreldc.c` and `wdib.c`; they are not an unused archive or a replacement drawing implementation.

| Concern | Authority | Rule |
| --- | --- | --- |
| Guest GDI identity | Original WOW caller and its 14-bit handle slot | The low two compatibility bits are not an independent identity. |
| Native GDI object/DC validity and drawing | Modern Windows | Real `HGDIOBJ`/`HDC` ownership and validation remain native. |
| 16/32 identity bridge | One worker-local alias carrier | It maps a 14-bit slot to a native handle; it does not type-tag, generate, schedule or draw. |
| DC cache cleanup | Original `wreldc.c` ordering plus public `GdiFlush` | The unavailable private local-DC tag is not reconstructed. |
| DIB private lifetime | Original `wdib.c` | Deferred to S3, rather than duplicated by a new bitmap layer. |

## Provider and state matrix

| Family / actual entry | Original owner and selected build input | Current owner and normal / failure / retirement rule | Audit disposition and next owner |
| --- | --- | --- | --- |
| Generic GDI handle conversion: `GDI32`, `GDI16`, `WOWHandle32`, `WOWHandle16` | `mvdm/wow32/walias.c,h`; original reads `ENTRYWOW.FullUnique` through `GdiQueryTable` and composes the 14-bit index plus uniqueness. | `ntvdm-exe/wow/wow_gdi_alias.c`, reached through `walias.h`. `NULL` maps to zero; the same live native handle keeps its slot; an unallocated nonzero slot maps to `INVALID_HANDLE_VALUE`; retire clears only that slot; allocation failure is zero plus `ERROR_NOT_ENOUGH_MEMORY`. | **Retain one carrier.** `GdiQueryTable` returns `NULL` for a real bitmap on the supported modern host, proved by T422 E8. Reintroducing a guessed private table or a second manager is prohibited. S2 exercises caller ordering. |
| DC and object creation: `WG32CreateCompatibleDC`, `WG32CreateDC`, `WG32CreateIC`, object/stock creators | `mvdm/wow32/wgdi.c`, directly compiled. It invokes the native service and returns `GETH*16` aliases. | Original provider plus the one bridge; modern Windows owns the created object. A failed native result remains failure and does not create a live slot. | **Keep original provider; S2 verify.** No autonomous creation layer was found. |
| Selection/current object: `WG32SelectObject`, `WG32GetCurrentObject` | `wgdi.c`, directly compiled. `SelectObject` returns the prior object through `GETHOBJ16`; `GetCurrentObject` converts its native result through `HGDI16`. | Native DC owns selection; the carrier only gives the returned/selected native identity a guest slot. | **Keep original provider; S2 verify** selection, prior-object identity and stale input behavior. |
| Save/restore, mapping and primitive drawing | `wgdi.c` and `wgdi31.c`, directly compiled. | Original argument conversion/call order invokes native GDI; no current project rasterizer was found. | **Keep original provider; S2 verify** DC state and selected primitive paths. Font/text-specific portions hand off to W08. |
| DC destruction: `WG32DeleteDC` | `wgdi.c`, directly compiled. | Native `DeleteDC` is attempted after DIB handling; `FREEHOBJ16` is called only when the native deletion succeeds. Failed deletion retains the guest alias. | **Keep exact ordering; S2 verify.** A second lifetime owner would be a defect. |
| Object destruction: `WG32DeleteObject` | `wgdi.c`, directly compiled. | Native `DeleteObject` follows DIB-section handling; alias retirement follows only successful native deletion. | **Keep exact ordering; S2 verify.** DIB exception belongs to S3. |
| Released/cached DC cleanup | `mvdm/wow32/wreldc.c`, directly compiled. It marks a cache entry release-pending, calls `GdiCleanCacheDC(HDC32(...))`, then `GdiFlush`. | `wow32-dll/source/wow_public_user_facade.c::GdiCleanCacheDC` is the registered `ADAPTER-WIN32-056` no-state success binding: the original only validates a private local-DC tag, unavailable on public modern HDCs; the next original call is the public flush. | **Retain exception; S2 verify ordering.** It must not become a parallel cache or infer a private tag from sampled host state. |
| DIB section and memory DC | `mvdm/wow32/wdib.c`, directly compiled. | Original DIB state remains the selected owner; current bridge supplies handle conversion only. | **S3.** Do not add a bitmap manager in S1/S2. |
| Palette/bitmap presentation consumers | Original GDI/DIB providers and later consumer packages. | No new owner selected in W07 S1. | **S3/W09 handoff.** Palette/bitmap/cursor/menu presentation must not be silently absorbed into DC recovery. |
| Font, glyph, metafile paths | `wgdi31.c` and `wgmeta`/font sources, with separate existing divergence records. | Not a W07 foundation claim. | **W08 handoff.** |
| Control drawing wrappers | Existing WOW control packages. | Consumers of a valid native HDC, not a rival GDI identity provider. | **W06 handoff.** |

## Duplicate-owner and lifecycle findings

1. The lexical census finds one current implementation of `mvdm_wow_gdi_handle_{from_native,to_native,retire}`. No second live GDI alias table, drawing engine or guessed NT4 GDI table exists in the selected product tree.
2. The original release direction is material: a failed `DeleteDC` or `DeleteObject` must not retire its guest alias. Successful release makes the slot reusable; stale nonzero guest values deliberately become invalid, rather than a meaningful null/screen DC.
3. T422's immutable contract fixture already proves zero, stable duplicate mapping, stale-slot invalidation, slot reuse and the 16,383 live-slot capacity. That is carrier evidence only. S2 must add/associate real original caller evidence for select/release/DC state before it may claim the drawing binding is complete.
4. `ADAPTER-WIN32-056` is not an alternative GDI cache. It preserves the original reached result where the private tag cannot be represented and lets the selected original `GdiFlush` perform the public synchronization.

## S2--S4 bounded handoff

| Stage | Fixed implementation/acceptance boundary |
| --- | --- |
| S2 — DC and selected drawing | Audit every selected `wgdi.c`/`wreldc.c` entry that reaches the carrier; correct only a proved adapter ordering/ABI/lifetime gap; verify create/select/current/save/restore/delete and cache release as one batch. |
| S3 — DIB and palette | Reconnect selected `wdib.c` lifetime and DIB/palette ownership; retain original bitmap exceptions and hand presentation consumers to their existing package. |
| S4 — graphics acceptance | Validate SOL/WRITE usable main-window initial drawing and retain WINMINE; record environment-dependent layout boundaries rather than changing guest behavior. |

No guest image is modified or rebuilt by this audit. The only unavailable dependency is the modern-host-inaccessible NT4 `GdiQueryTable` entry table; the one bounded bridge remains the registered, tested exception.
