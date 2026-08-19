# T229 S2 P13 — `demCreateFCB` Direct/Readonly conformance

## Source contract

OpenNT `src/opennt/base/mvdm/dos/dem/demfcb.c` implements `demCreateFCB` as
`demFCBCommon(CREATE_ALWAYS)`.  Its success ABI is an opaque handle in
`AX:BP`, DOS time/date in `BX:CX`, and 32-bit size in `DX:SI`.

## Recovery

The existing source-derived FCB handle provider already preserves the result
layout.  This slice adds the missing shared mutation-profile check before its
local backend may reach `CreateFile`: `50:2C` is namespace-content mutation,
so Readonly returns `ERROR_ACCESS_DENIED` (`AX=5`, CF set) before host
creation or truncation.  Direct remains admitted and uses the actual host
fixture only.

## Verification

`tests/bx-vdm/bx_ntvdm_dem_package_family_v1_test.c` runs the route through
BOP ingress, checked guest pathname gathering and the installed whole
provider.  It verifies Direct creation, result layout and paired `50:2E`
close/cleanup, while Readonly preserves the original file and receives the
pre-host refusal.

```text
ninja -C build/M0-T229-S2/formal-direct-readonly-20260818b \
  bin/t225-s19-dem-package-lifecycle-fixture.exe
...fixture.exe direct pdb-lifecycle   => 0
...fixture.exe readonly pdb-lifecycle => 0
```

No Overlay behavior is added; its existing provider seam remains unchanged.
This does not close FCB attributes, error variants, rename, or the entire DEM
package.