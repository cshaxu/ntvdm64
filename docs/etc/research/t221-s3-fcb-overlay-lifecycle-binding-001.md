# T221 S3 FCB Overlay lifecycle binding

## Question

Can the already-admitted Overlay FCB lifecycle package (`50:2C..31`) bind to
one selector-blind private file provider while retaining the original OpenNT
`demfcb.c` register, share-mode, checked-DTA, and host-isolation contracts?

## Inputs

- `src/opennt/base/mvdm/dos/dem/demfcb.c`: `demCreateFCB`, `demOpenFCB`,
  `demCloseFCB`, `demFCBIO`, `demDate16`, and `demGetFileInfo`.
- Existing typed file-session, Overlay file, resolver, namespace and handle
  backends in `src/bx-vdm`.
- `tests/bx-vdm/bx_ntvdm_dem_whole_provider_v1_test.c`.

## Procedure

1. Bound `50:2C/2D/2E/2F/31` through the FCB Overlay backend; `50:2F` keeps
   its existing checked DTA gather/write route.
2. Translated the original FCB sharing mode into the private two-bit Overlay
   share mask, without passing the Win32 `FILE_SHARE_DELETE` bit across that
   boundary.
3. Corrected the complete FCB result ABI to Bochs register encoding:
   `2C/2D` return `AX:BP`, `BX`, `CX`, `DX:SI`; `2F` returns `AX:BX` and
   `CX`; `30` returns `AX` and `DX`; `31` returns `AX`, `CX`, `DX`, and
   `BX:DI`.
4. Ran the source-built full provider fixture under MSVC x64 `/MT`.

## Observations

`build/M0-T221-S3/081` passes. It proves an Overlay `2D -> 2F -> 2E`
lifecycle with a checked DTA write action, `31` metadata/size agreement with
the preceding `2D` result, and `2C` private create/close. The host-base file
is not mutated by the Overlay path. Direct FCB regressions now assert the
same corrected historical register layouts.

## Interpretation and confidence

The Overlay lifecycle binding is source-derived and selector-local only at the
existing DEM route. Its provider is selector-blind: no BOP decode, guest
pointer, or raw host handle crosses it. Confidence is high for the covered
`2C..31` lifecycle ABI and isolation; it is not a claim that the separate FCB
wildcard package (`50:07/20`), search package, Virtual view, or the complete
29-service DEM package is closed.

## Follow-up

Implement the separately admitted Overlay FCB wildcard mutation package
(`50:07/20`) as one owner unit, then continue the remaining search/Virtual
subfamilies before any native trace.
