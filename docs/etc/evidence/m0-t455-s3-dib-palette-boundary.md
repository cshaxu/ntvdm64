# M0 T455 S3 — DIB, memory-DC and palette boundary

**Status:** closed without a product-source repair.  The selected x86 route
already uses the original owners, has one DIB-information list, and does not
need a second bitmap, palette, or GDI identity manager.

## Reached original route

| Surface | Original route and owner | State/lifetime result |
| --- | --- | --- |
| `DIB.DRV` memory DC | `WG32CreateDC` in `wgdi.c` recognizes the original DIB driver spelling and calls `W32HandleDibDrv` in `wdib.c`.  It obtains selector size/flags through `RET_GETDIBSIZE`/`RET_GETDIBFLAGS`, creates the compatible DC, file mapping and DIB section, selects the bitmap, then commits selector replacement through `RET_SETDIBSEL`. | `DIBINFO` is the only owner of its DC, map/view, bitmap, selector and nested lock count.  A failed selector callback or native create never calls `W32AddDibInfo`, so it cannot become a live guest DC. |
| DIB release / realloc | Original kernel `WK32FindAndReleaseDib` converts the DIB handle to its selector and uses the same `DIBINFO` list.  `WG32DeleteDC` uses `W32CheckAndFreeDibInfo`. | The final release first calls original `RET_FREEDIBSEL`, then unmaps/deletes/closes and removes the list node.  A nested release decrements the original lock count but leaves the resources live. |
| `CreateDIBSection` | Original `WG32CreateDIBSection` creates the native bitmap, obtains the guest selector through `RET_SETDIBSEL`, and records the result in `DIBSECTIONINFO`.  Its table entries are `CREATEDIBSECTION`, `GETDIBCOLORTABLE`, and `SETDIBCOLORTABLE` in `wgtbl2.h`. | `WG32DeleteObject` reaches the corresponding DIB-section release check before native deletion.  On x86 the original code deliberately sets `pvIntelBits = pvBits`; the non-i386 `VdmAddVirtualMemory` / descriptor-mapping branch is not selected. |
| Palette | `wgpal.c` directly owns palette creation, entry copy, animation, resize, nearest-index, selection and realization thunks; native GDI owns the `HPALETTE`. | No project palette cache or presentation-side palette authority was found.  Original table entries deliberately marked `UNIMPLEMENTEDAPI` remain original table policy, not a missing project replacement. |
| Callback transport | `wcall16.c` identifies the four DIB callback IDs; `wow_user_native_extension.c` classifies them as native extension callbacks rather than application procedures or launch requests. | The callback identity stays within the existing original callback transport.  No new message path, scheduler, guest change, or handle carrier is introduced. |

The selected `wdib.c` and `wgpal.c` have no semantic delta since canonical
OpenNT mirror commit `622b71cda` (`git diff --ignore-space-at-eol --numstat`
reports no lines).  Their temporary-file/mapping cleanup, including its
historical failure paths, is original source.  No observed modern-host defect
justifies replacing it in this S.

## Verification batch

The formal cached x86 build graph was invoked for `wow32`; it rebuilt its
required `ntvdm.exe` prerequisite as necessary and reported the selected
worker and `WOW32.DLL` graphs up to date.  The tested `WOW32.DLL` identity is
`203D17B5F3F1C35C1243688D11A47A2B363A77D87209285F8C80889C2B051D01`.

`original-external-memory-test.exe` was rebuilt from the same formal worker
graph and exited zero.  Its real native `CreateDIBSection` case selected the
bitmap into a compatible DC, mapped the returned bits through
`VdmAddVirtualMemory`, proved `SetPixelV`/`GdiFlush` visible through guest
physical memory, proved a guest byte write visible through `GetPixel`, then
removed the mapping before deselection/deletion.  It printed:

```text
real DIB GDI/guest bidirectional sharing PASS
original add/remove unaligned sharing and normal-backing PASS
```

The retained negative boundary observation also ran: modern `GdiQueryTable`
returned `NULL` for a real bitmap (`directContract=false`, probe exit `1`).
That is the already-approved reason for S1's single bounded alias carrier;
it is not a DIB fallback and it did not cause any source change here.

This fixture proves host DIB storage sharing, selection and cleanup.  It does
**not** claim that immutable Win16 application DIB thunk traffic has been
accepted.  Real guest raster content, palette effects and application-visible
graphics belong to S4's integrated acceptance boundary.

## S4 handoff

S4 may use only the retained original owners and the S1 alias carrier.  It
must exercise real WINMINE/SOL/WRITE display paths and distinguish a guest or
environment limitation from an unproven host binding defect.  It may not
replace `wdib.c`, invent a rasterizer, or convert the native storage fixture
into claimed guest acceptance.
