# T199 S37 DEM S1 FCB/Search Family Baseline 001

## Scope

This record consolidates the FCB/search component governed by OpenNT
`demfcb.c`, `demsrch.c`, and the canonical `demdisp.c` table.  It is not
trace-derived work and does not claim a writable FCB filesystem.

## Component disposition

- `50:0A/0C`: checked SRCHBUF/path/PDB reads, immutable profile search,
  opaque per-PDB continuation and checked guest writes.
- `50:07,20,2C-2D,2F,31`: no projected NT handle or writable filesystem;
  explicit AX=5/CF terminal.
- `50:2E`: historical zero-handle CF-clear completion; nonzero handle is the
  contained terminal.
- `50:30`: original DOS packed date in AX and time in DX.

## Evidence

The current full fixture at
`artifacts/build/t199-s37-dem-package-r85-n1-baseline/` compiles, links and
runs the all-DEM session, including its FCB core and BOP transaction checks:
multi-entry and single-entry first/next, exhaustion, no match, empty/altered
token, foreign PDB and aperture boundary.  These checks are implemented in
`tests/bx-vdm/bx_ntvdm_dem_package_family_v1_test.c` and exercise the actual
adapter transaction route rather than an ambient host directory.

## Boundary

No host pathname, host handle, writable storage, Bochs pointer or DOS kernel
semantic crosses the component boundary.  This is FCB/search component
evidence, not S37 closure or native Bochs trace evidence.
