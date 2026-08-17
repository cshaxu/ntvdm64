# T225 S19 Direct/Readonly DEM Package Regression Result 001

## Question

Does the reconciled DEM package session exercise the complete original `50:00..48`
selector table under both supported mutation profiles without an Overlay fallback?

## Inputs

- Original owner table: `src/opennt/base/mvdm/dos/dem/demdisp.c`.
- Package fixture: `tests/bx-vdm/bx_ntvdm_dem_package_family_v1_test.c`.
- Focused source closure: `tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 -Fixture dem-package`.
- Fresh disposable build: `build/M0-T225-S19/dem-package-direct-readonly-r10/`.
- MSVC x64 `/MT`; no bx-core, bx-mantle, guest image, native trace, or Overlay mode was linked.

## Procedure

1. Parameterize the fixture for `direct` and `readonly`; no Overlay invocation is admitted.
2. Align its declared target with the current boot-namespace profile ABI.
3. Exercise the real host C: root only for the Direct/Readonly CWD policy path. The fixture runs in its own child process; its Direct `SetCurrentDirectoryW` state cannot persist into the caller.
4. Link the exact compiled objects in the fresh directory with `link.exe /MT` closure and execute the same binary once for each mode.

## Observations

- Link exit: `0`.
- Direct fixture exit: `0`.
- Readonly fixture exit: `0`.
- The fixture covers every numeric service `50:00..48`, its retained original no-op/failure terminals, the FCB transaction path, GSET/drive observations, CWD policy, pipe/FCB unavailable families, and source-shaped computer-name behavior.
- Direct `50:1A` uses the original CF-only success contract; Readonly returns the contained access-denied `AX=5, CF=1` contract. `50:41` accepts only the original host-name success or source-defined empty-name fallback.

## Interpretation

This is whole-package route and profile-regression evidence, not proof that every
historical DEM provider is independently reusable or that native guest execution
is complete. It proves the selected current package session has no Direct-to-Readonly
or Overlay fallback for the covered table and that Direct/Readonly use their distinct
source-shaped CWD outcomes.

The former probe wrapper’s sequential compile can exceed the app execution window
before it reaches link. The result above therefore links its already compiled source
closure in the same fresh directory. The fixture PE uses `.bin` only because the
execution environment removes some disposable unsigned `.exe` outputs; it remains
the explicit path passed to the child process and is not a product artifact.

## Follow-up

Continue S19's legacy adapter-runtime route inventory and owner-subfamily migration;
run one bounded native trace only after those complete package checks pass.