# T226 S2 formal DEM baseline build result 001

## Question

Can the committed formal MSVC x64 `/MT` Ninja graph build and run the existing
whole-DEM family fixture in both enabled mutation profiles?

## Inputs

- The committed T225 S7 full-module manifest, copied into the disposable S2
  run so unreviewed S58 manifest changes were excluded.
- `tests/bx-vdm/bx_ntvdm_dem_package_family_v1_test.c`.
- The current formal graph generator and pinned CPU5 configuration projection.

## Procedure

A clean run first exposed that the graph created response files relative to a
wrapper-changed working directory. `lib.exe` therefore could not open
`lib/bx-vdm.lib.rsp`; no DEM source was compiled or linked as the cause of that
failure. The graph generator now declares each response file below its absolute
`$build_root` path. A fresh disposable graph was generated, then the DEM fixture
was built and invoked separately as `direct` and `readonly`.

## Observations

- Both generated `rspfile` declarations use `$build_root/$out.rsp`.
- The fresh graph linked `bx-vdm.lib` and
  `t225-s19-dem-package-lifecycle-fixture.exe` successfully.
- The fixture returned zero for `direct` and zero for `readonly`.
- The fixture takes an explicit mode argument; an argument-less invocation
  returns its documented usage exit `93` and is not a regression result.

## Interpretation and limitation

This proves the current formal graph can compile and execute the existing
all-73-identity DEM family fixture under both product modes. It does not prove
that all original providers are code-complete: S2 still has to reconcile every
source/ABI/failure disposition and replace any generic/stale fallback that is
not source-proven.

## Follow-up

Use the retained 73-row DEM recovery ledger to make subfamily dispositions and
negative assertions explicit. Do not use native trace to select the work.