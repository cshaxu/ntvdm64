# M0 T261 S4 P6 — DEM CCPU/SAS facade migration

## Change

- Moved the common DEM copied-frame, checked guest-memory, opaque-handle and
  typed-resume facade from `opennt-host/dem/demhndl_shim.{c,h}` to
  `adapter-softpc/opennt_dem_ccpu_sas_facade.{c,h}` by Git history.
- Preserved every historical source-facing helper spelling used by imported
  DEM bodies.  No BOP selector, service number or DOS provider code entered
  `adapter-softpc`.
- Replaced the former direct link to `demerror` with a thread-scoped generic
  post-body hook.  `opennt-bop` installs the `demerror` flush at its DEM
  dispatcher boundary and clears it after the source body returns.  This
  preserves fixed VHE write-back order without an adapter-to-BOP static
  dependency.

## Verification

- Fresh formal MSVC x64 `/MT` root: `build/M0-T261-S4/r014`.
- Full graph completed 461 Ninja edges and then reported `ninja: no work to
  do.`
- `t230-s2-demhndl-direct-import-fixture.exe` verified handle, guest-memory
  and result seams.
- `t230-s8-demerror-lock-direct-import-fixture.exe` verified NTDLL Unicode,
  hard-error setup, dispatcher retry and file-lock contracts.

## Result

The common CCPU/SAS mechanism is no longer a false `opennt-host` capability.
The retained OpenNT `demerror` guest-layout behavior is composed downward
through an opaque mechanical lifecycle seam, leaving the adapter selector-blind.
