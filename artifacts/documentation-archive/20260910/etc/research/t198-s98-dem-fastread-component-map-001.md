# T198 S98 DEMFASTREAD component map 001

## Question

Why did S97 stop at `50:42` although a source-derived fast-read provider is
compiled into the current composition?

## Original contract

`dossvc.h` identifies `42h` as `SVC_DEMFASTREAD` and `43h` as
`SVC_DEMFASTWRITE`.  `handle.asm` calls `50:42` on the `FastOrSlow` read
branch and takes its slow `50:16` fallback only when the BOP returns CF set.
The visible historical host dispatcher maps `42h` to `demNotYetImplemented`,
which clears CF without reading; this is why the contained CLI profile needs
the explicitly approved source-derived read-side composition rather than a
selector alias to ordinary `DEMREAD`.

The admitted provider retains the copied fast-read contract: real-mode
`C4 C4 50 42`, existing namespace handle, optional `BX:SI` position when ZF
is clear, `CX` count, `DS:DX` destination, bulk checked guest-RAM publication,
AX byte count and CF result.  `FASTWRITE` remains unprovided.

## Current source contradiction

`bx_ntvdm_boot_namespace_plane_v1_dispatch` routes `42h` only after DEM plane
classification says `NAMESPACE`.  But the current
`bx_ntvdm_dem_plane_v1_component` lists `42h` under `RAW_MEDIA`, so the plane
returns a pass-through result before reaching the provider.  The manifest does
compile the provider; this is a composition-classification regression, not a
link omission or a Bochs/mantle defect.

This contradicts the retained S59 witness, which correctly states that `42h`
must be assigned to the namespace component.  The actual current source is
authoritative for S98.

## S97 correspondence

The one S97 run reports `next=50:42`, after an earlier observed fast-read
event at `0032:6351` with `EAX=4005h`, `ECX=20h`, `EDX=000Ah` and
`DS:SI=9fba:0000`.  It stops before `54:02`.  This records reachability but
does not prove the invocation is safe to accept until the provider's existing
handle/position state and failure path are tested through the repaired
classification.

## Bounded next repair

S99 may restore only service `42h` to `NAMESPACE`, retain `43h` as explicitly
unprovided, and add focused classification/composition tests for both.  It
must not alter the fast-read ABI, treat raw-media services as namespace
services, add host filesystem access, change Bochs/mantle, or run a guest.
