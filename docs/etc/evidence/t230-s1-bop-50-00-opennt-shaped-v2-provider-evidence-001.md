# M0 T230 S1 P2 — BOP 50:00 OpenNT-shaped v2 Provider Evidence

## Scope

This is a bounded source-recovery practice for DEM `50:00` only.  It does not
close the DEM owner package, the `50:00` native integration path, or T230.

## Original source and role

| Field | Record |
| --- | --- |
| Primary tree | This repository's pinned OpenNT tree |
| Source path | `src/opennt/base/mvdm/dos/dem/demhndl.c` |
| Revision examined | `69754fbf98ba08991be207da908153cf2c5d5aa3` |
| Original owner | DEM `demChgFilePtr`, dispatched by `demdisp.c` as BOP `50:00` |
| Original fragment | `demhndl.c:342-389` |
| Historical role | DOS file-pointer change: `AX:BP` handle, signed `CX:DX` displacement, `BL` origin; success returns `DX:AX`, failure goes through `demClientError` |
| Notices | No additional notice occurs in the referenced function range; the new file records its source path and function range in its leading comment. |

## Recovery-ladder disposition

Direct compilation of the whole historical `demhndl.c` was rejected for this
endpoint: it is a multi-service translation unit bound to the old CCPU/SAS
register macros, a historical raw guest-visible `HANDLE` representation, and
its complete historical hard-error composition.  Linking that whole object
would import unrelated DEM services and unavailable historical composition.

The selected rung is therefore **smallest adapter/shim source-derived rehost**:
`src/bx-vdm/bop/demhndl.c` retains the original `demChgFilePtr` function's
local variable layout, compile-time Win32/DOS origin assertion, signed
`CX:DX` construction, `SetFilePointer` call ordering, `-1` failure branch and
`DX:AX` success layout.  It is not newly invented seek behavior.

Alternatives not selected:

- compiling the complete original translation unit — non-composable historical
  dependencies and unrelated service expansion;
- retaining the prior `SetFilePointerEx`-only leaf as the formal provider —
  it did not preserve the original body/order as a visible source artifact;
- Bochs change — not applicable; this is host DEM behavior.

## Explicit divergences

Every divergence is adjacent to the relevant code in `src/bx-vdm/bop/demhndl.c`:

1. `GETHANDLE(AX,BP)` becomes a bounded session opaque-token lookup; x64 host
   `HANDLE` values are never guest-visible.
2. CCPU `setCF/setAX/setDX` becomes a typed copied CPU delta; bx-vdm never
   mutates Bochs CPU state directly.
3. Historical `demClientError(hFile, -1)` is represented by the currently
   admitted register failure boundary pending rehosting of the original INT 24
   hard-error owner.

Readonly remains routed to its existing readonly namespace backend.  The v2
provider is selected only for Direct Win32-handle tokens; no Overlay or
Virtual route was enabled by this change.

## Validation

A fresh formal MSVC x64 `/MT`, CPU5/P-MMX Ninja graph was generated below
`build/M0-T230-S1/50-00-opennt-v2-r1` from
`tools/build/t225-s7-full-module-manifest.json`.  The focused formal target
built all 160 required edges and linked:

`bin/t225-s19-dem-package-lifecycle-fixture.exe`

The fixture passed both invocations:

```text
direct=0 readonly=0
```

The extended fixture covers `FILE_BEGIN`, `FILE_CURRENT` and `FILE_END`
(including a negative end-relative displacement) through the common `50:00`
route.  It confirms only local Direct/Readonly service semantics.  A full
73-service matrix, retired-Virtual removal and the bounded native observation
remain required by T230.
