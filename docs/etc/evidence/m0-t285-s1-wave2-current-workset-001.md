# M0 T285 S1 — Wave 2 current workset reconciliation

## Question

Do the current canonical component roots still contain the complete,
source-audited Wave 2 host-control and DEM workset, and can its admitted
selector-disabled original source islands still compile on both host
architectures?

## Inputs

- `docs/etc/operations/m0-t280-s1-host-control-dem-workset-ledger.tsv`
  (the prior complete 81-row source disposition).
- Current `src/mvdm-host/dos/dem/**` and
  `src/mvdm-host/softpc.new/host/src/**` mirror paths.
- T284's canonical `adapter-mvdm-host-in`, named
  `adapter-mvdm-host-out/*`, `adapter-bochs` and `session` boundaries.

## Procedure

1. Ran `New-T285S1Wave2CurrentWorkset.ps1` to require all selected source
   paths, hash each current file, convert only superseded adapter root names,
   and reject a changed 22/59 package denominator or any stale root.
2. Generated and built the selector-disabled complete original DEM static
   package and original `nt_bop.c` static package with MSVC `/MT` and Ninja,
   each for x86 and x64 under `build/M0-T285/S1/`.
3. Added the repository-local `VsDevCmd` wrapper to both existing graph
   generators. The first invocation established that a bare Ninja shell has
   no `cl.exe`; the wrapper invocation is the passing compiler result.

## Observations

- The current workset has 81 rows: 22 DEM and 59 host-control paths.
- Final dispositions are 49 `adapter-backed`, five `binding-only`, six
  `overlay-required`, and 21 `not-host-runtime`.
- `original-dem-package.lib` exists for x86 and x64.
- `original-bop-dispatch.lib` exists for x86 and x64.
- The imported original sources emit expected historical/width warnings; the
  static archives are not a runtime-link or BOP-enable claim.  In particular,
  x64 pointer-shaped warnings remain governed by the workset's named mapping
  and adapter rows.

## Interpretation and follow-up

The source denominator and canonical names are sound.  S2 is the first legal
implementation group: a complete original `nt_bop.c` ingress/control binding
map.  It must not activate a selector until all of that source group's
external calls have a same-shaped adapter or explicit later-owner result.
