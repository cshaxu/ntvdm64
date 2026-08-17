# T225 S19 DEM PDB lifecycle ordering result 001

## Scope

This result verifies only the corrected DEM lifecycle ordering.  It is not a
claim that the 73-slot DEM package, COMMAND, or native guest execution is
closed.

## Inputs and build closure

- Corrected route: `src/bx-vdm/bx_ntvdm_dem_session_lifecycle_provider_v1.c`.
- Focused package fixture:
  `tests/bx-vdm/bx_ntvdm_dem_package_family_v1_test.c`, named entry
  `direct pdb-lifecycle`.
- Formal manifest: `tools/build/t225-s7-full-module-manifest.json`.
- Fresh disposable root: `build/M0-T225-S19/package-lifecycle-r2/`.
- Toolchain: the manifest-selected MSVC x64 `/MT` graph.  The cold build
  completed all 153 edges and linked
  `bin/t225-s19-dem-package-lifecycle-fixture.exe`.

The focused fixture creates a Direct opaque file token owned by copied PDB
`BEEFh`, dispatches the ordinary package route `50:3C`, verifies the void
`RIP + 4` result and a still-live token, then dispatches `50:02` and verifies
that the token is no longer resolvable.  Thus it distinguishes the two
historical ownership points rather than merely testing `release_owner` in
isolation.

## Commands and observations

```text
ninja -C build/M0-T225-S19/package-lifecycle-r2 -j4 \
  bin/t225-s19-dem-package-lifecycle-fixture.exe
  -> 153/153 LINK; exit 0

bin/t225-s19-dem-package-lifecycle-fixture.exe direct pdb-lifecycle
  -> exit 0

bin/t225-s19-dem-package-lifecycle-fixture.exe readonly pdb-lifecycle
  -> exit 97 (the named Direct-only token scenario is explicitly refused)

ninja -C build/M0-T225-S19/package-lifecycle-r2 -n \
  bin/t225-s19-dem-package-lifecycle-fixture.exe
  -> ninja: no work to do; exit 0
```

## Follow-up

The initial full-family invocation exposed a `50:25` DPB failure mismatch.
That issue is now resolved separately by the
[DPB failure result](t225-s19-dem-dpb-failure-regression-result-001.md): the
complete Direct and Readonly family fixture passes.  This P4 record remains
limited to PDB termination ordering.