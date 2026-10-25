# M0 T455 S2 — DC/drawing dispatch and lifetime evidence

**Status:** closed without a product-source repair. The selected original
route is already complete; this record prevents a speculative replacement
from being introduced merely to create a change.

## Reached route

1. `wow32.c::W32Dispatch` resolves the 16-bit call ID through `aw32WOW` and
   invokes the chosen original thunk.
2. `wowtbl.c` constructs `aw32WOW` by including `wgtbl2.h` in the fixed GDI
   table position; `InitThunkTableOffsets` publishes that GDI offset.
3. `wgtbl2.h` maps the selected ordinals to original `WG32*` handlers with
   their original 16-bit argument sizes: creation, stock/select/current,
   save/restore, transforms, primitives, `DeleteDC` and `DeleteObject`.
4. The formal x86 generator directly compiles and links `wowtbl.c`,
   `walias.c`, `wgdi.c`, `wgdi31.c` and `wreldc.c` into `WOW32.DLL`.
5. Native Windows performs GDI work; the bounded worker alias carrier only
   converts the reached original 14-bit identity at the boundary.

## Ordering audit

| Operation | Reached original ordering | Result |
| --- | --- | --- |
| Create compatible DC | Validate nonzero guest DC through `HDC32`; call native `CreateCompatibleDC`; alias result through `GETHDC16`; optionally set only original `WOWCF_UNIQUEHDCHWND` bit. | Preserved. |
| Select / current object | Native `SelectObject`/`GetCurrentObject`; returned native object enters the same `GETHOBJ16`/`HGDI16` carrier. | Preserved. |
| Save / restore | Native `SaveDC`/`RestoreDC` with original signed 16-bit conversion. | Preserved. |
| Delete DC / object | Native DIB cleanup then native deletion; guest alias retires only after native deletion reports success. | Preserved. |
| Cached `ReleaseDC` | Original cache marks release pending, calls the no-state private-tag facade, calls `GdiFlush`, then later clears cache state and retires the DC alias even when its late native `ReleaseDC` reports failure. | Preserved; this intentionally differs from immediate delete and is original compatibility behavior. |

No autonomous dispatcher, second handle carrier, guessed local-DC tag, or
project rasterizer was found. `ADAPTER-WIN32-056` remains the documented
minimal no-state facade: private local-DC tags are unavailable on the modern
host and original `wreldc.c` immediately follows it with public `GdiFlush`.

## Coherent verification batch

An owned hidden process ran `build/t455-s2/run-s2.cmd` on 2026-10-24:

| Check | Evidence |
| --- | --- |
| Formal selected component build | `cmake --build --preset msvc-x86-release-ccache --target wow32 --parallel 12` reconfigured the established Ninja graph, built its prerequisite `ntvdm.exe`, and reported `ninja: no work to do` for the selected WOW32 link graph. |
| One-carrier contract | The bounded fixture compiled against the current carrier and printed `WOW_GDI_ALIAS errors=0`; it covers null/zero, stable duplicate mapping, stale nonzero invalidity, first-slot reuse and full-table `ERROR_NOT_ENOUGH_MEMORY`. |
| Output identity | Built `WOW32.DLL` SHA-256: `203D17B5F3F1C35C1243688D11A47A2B363A77D87209285F8C80889C2B051D01`. It is build evidence only, not a publication candidate because S2 changed no product source. |

The raw owned stdout/stderr and fixture executable are disposable
`build/t455-s2` evidence. No guest media was changed or rebuilt.

## S3 handoff

S3 owns only the selected `wdib.c` DIB/memory-DC and palette boundary. It must
retain the just-proved one carrier and original DC ordering, not fold DIB
state into a new general GDI manager. Font/text/metafile paths remain W08;
control wrappers remain W06; presentation consumers remain W09.
