# T225 S19 DEM DPB failure regression result 001

## Scope

This result covers the source-derived `50:25` DPB failure split and the
complete existing DEM package fixture.  It does not claim native guest
execution, COMMAND recovery, or completion of T225.

## Inputs

- Source map: [DPB failure source map](../research/t225-s19-dem-dpb-failure-source-map-001.md).
- Corrected source: `src/bx-vdm/bx_ntvdm_dem_full_dpb_service_v1.c`.
- Full package fixture: `tests/bx-vdm/bx_ntvdm_dem_package_family_v1_test.c`.
- Formal disposable root: `build/M0-T225-S19/package-lifecycle-r2/`.
- Toolchain: manifest-selected MSVC x64 `/MT` Ninja graph.

## Procedure and result

```text
ninja -C build/M0-T225-S19/package-lifecycle-r2 -j1 \
  bin/t225-s19-dem-package-lifecycle-fixture.exe
  -> rebuilt DPB object, bx-vdm.lib and fixture link; exit 0

bin/t225-s19-dem-package-lifecycle-fixture.exe direct
  -> exit 0

bin/t225-s19-dem-package-lifecycle-fixture.exe readonly
  -> exit 0

bin/t225-s19-dem-package-lifecycle-fixture.exe direct pdb-lifecycle
  -> exit 0

ninja -C build/M0-T225-S19/package-lifecycle-r2 -n \
  bin/t225-s19-dem-package-lifecycle-fixture.exe
  -> ninja: no work to do; exit 0
```

The full fixture exercises every callable `50:00..48` identity through the
package session in both supported product modes.  In particular, its
unadmitted `50:25` path now receives AX=15 while the admitted-but-unavailable
snapshot path receives AX=5, both with carry set; P4's Direct token ordering
remains green.

## Limitation

This is package-route/profile evidence.  The required bounded native trace is
still a later S19 acceptance stage and may not create new singleton BOP work.