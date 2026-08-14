# T198 S106 DEM fast-I/O no-op family 001

## Change

`src/bx-vdm/bx_ntvdm_dem_fastio_noop_service.c` is the sole active
implementation of the historical DEM fast-I/O pair. It accepts only canonical
real-mode `C4 C4 50 42` and `C4 C4 50 43` events and returns typed resume at
`fault_rip + 4` with CF clear. It does not inspect handles, positions or guest
memory and writes no GPR.

The active boot-namespace composition calls that family plane before its
namespace plane. The former `50:42` byte-copy dispatch was removed from the
namespace plane. The old fast-I/O helper remains detached historical/research
code; it is absent from the current composition manifest and has no active
call path.

## Verification

- `artifacts/build/t198-s106-fastio-noop-test-r1` source-builds and runs the
  C-island positive cases for both selectors, plus wrong-selector and
  wrong-vector negative cases.
- `artifacts/build/t198-s106-composition-x64-r2` source-builds, links and runs
  the current MSVC x64 `/MT` boot-namespace composition fixture. It is not a
  guest execution.
- r1 exposed an omitted recipe membership for the new source object; r2 fixes
  that recipe and passes. No guest run occurred in either composition build.

## Interpretation

This restores the original `demNotYetImplemented` contract for the entire
fast-I/O pair as one adapter plane. It deliberately does not turn the adapter
into a file service. S107 is the separate one-run observation of the original
NTDOS continuation.
