# M0 T236 S7 DEM/COMMAND v1 retirement record

## Question

After the imported OpenNT DEM and COMMAND routes became the formal bx-vdm
implementation, do any superseded v1 implementations or executable
test/tool inputs remain for those owners?

## Scope and classification

The retirement covers the v1 DEM/COMMAND namespace, search, mutation-profile,
startup-configuration, boot/configuration and package fixtures. It removes
their source files, direct tests, fixtures, old build manifests and old probes.
The removed tests were not current v2 coverage: they referenced inputs already
absent from the v1 tree, and they were absent from the formal module manifest.

The retained bop-v1 files have other owners (XMS, DPMI, Redirector,
machine, debugger/VDD or top-level) or are generic historical comparison
support. They remain excluded from the formal bx-vdm module and are not
selected by DEM/COMMAND routing.

## Procedure

1. Enumerate every src/, tests/ and tools/ reference to the retired
   DEM/COMMAND v1 APIs and paths.
2. Delete the obsolete implementation, test, fixture, probe and manifest
   inputs together.
3. Add a deterministic gate that rejects a revived reference and rejects any
   bop-v1 formal module input.

## Observations

- The formal t225-s7-full-module-manifest.json has no bop-v1 input.
- The retirement gate finds no retired API reference or DEM/COMMAND v1 path
  below src/, tests/ or tools/.
- The existing COMMAND v2 route-precedence gate still passes.

## Interpretation

The current DEM and COMMAND product route no longer has an executable v1
fallback. Git history retains the retired experimental work; it is no longer
a source, test, tool or build dependency. This is source-tree cleanup only,
not a claim of cross-owner guest continuity.

## Follow-up

The reconciled S ledger, eight-target formal regression and external-owner
transfer list are recorded in the [T236 closure](../../history/m0-t236-closure-20260821.md).
