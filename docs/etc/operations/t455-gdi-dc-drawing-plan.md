# T455 WOW32 GDI identity, DC and drawing plan

## Authority

T455 admits Queue W07. [Current](../../states/CURRENT.md) is the sole active
packet authority. The read-only `ntvdm64-wow32-audit` supplies source evidence;
it never supplies a runtime dependency. The task recovers selected original
GDI/DC behavior through the smallest source-shaped binding and removes
project-owned substitutes when an available original owner can replace them.

## Fixed ownership target

The selected original GDI identity provider owns the 14-bit current-slot rule
and its normal/failure/reuse semantics. Native Windows owns actual HGDIOBJ/HDC
resources. The project adaptation layer may only translate identity, data and
lifetime at that boundary; it may not mint a parallel handle generation/type
system or retain independent drawing truth. W03 remains the allocation/alias
owner; W05 is the sole resource-load foundation.

## Stages

| Stage | Scope | Exit |
| --- | --- | --- |
| S1 | Identity/source boundary | Finite original provider/caller/state matrix and retain/delete/replace disposition for every current carrier/adapter. |
| S2 | DC/drawing service batch | Reconnect selected original stock/select/delete/release, transforms and primitive paths as one coherent batch. |
| S3 | Raster batch | Reconnect selected bitmap/DIB/palette paths and WINMINE monochrome/DC boundary. |
| S4 | Graphics acceptance | Exercise real guest content, negative/lifetime cases and full product regression; publish coherent package. |

## Boundaries

S1 does not convert GDI server code into a host runtime, infer private modern
USER/GDI layouts, add a generic PE/resource fallback, or change guest media.
A discovered dialog, menu, font/text/metafile or input path is handed to its
existing W06/W08/W09/W10+ owner unless it is a finite direct prerequisite of a
selected W07 original provider.
