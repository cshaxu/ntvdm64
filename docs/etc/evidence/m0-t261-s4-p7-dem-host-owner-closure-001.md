# M0 T261 S4 P7 — DEM host owner closure

## Question

Do the remaining DEM support inputs have one correct production owner, with no
`opennt-host` to `opennt-bop` reverse dependency, and can the repaired static
library graph and directly imported DEM bodies build and execute their focused
fixtures?

## Inputs

- T261 S1 source-truth audit and T261 S4 P1 boundary map.
- The live `src/opennt-host/dem/` and `src/opennt-bop/dem/` trees.
- `tools/build/t260-s8-component-manifest.json` and a fresh MSVC x64 `/MT`
  Ninja root.
- The retained OpenNT DEM owners under `src/opennt-bop/dem/` and their
  corresponding focused fixtures.

## Procedure

1. Reclassified every remaining `opennt-host/dem` file by owner rather than
   retaining a generic `*_shim` path.
2. Moved the imported-DEM compilation/route compatibility surfaces and the
   BOP-side error, DASD and MISC composition units to `opennt-bop/dem/` using
   Git rename history.  Renamed the remaining host drive-policy declaration to
   `opennt_dem_drive_policy_facade.h`.
3. Repaired all production, fixture and formal-manifest include/source paths.
4. Registered DEM MISC, raw-DASD/FDC and error-composition departures in the
   owning `opennt-bop` register; retained only the two actual host capability
   exceptions in the `opennt-host` register.
5. Scanned production source for the old paths, generic host DEM shim paths
   and an upward `opennt-host` to `opennt-bop` dependency.
6. Generated fresh `build/M0-T261-S4/r016` and ran its full Ninja graph plus
   the declared direct-import DASD/IOCTL, error/lock and MISC fixtures.

## Owner result

- `opennt-host/dem/` now contains only the host drive admission capability,
  host namespace capability, and the named drive-policy facade declaration.
  `HOST-DIV-010` and `HOST-DIV-011` are their explicit source-derived
  records.
- `opennt-bop/dem/` owns the imported DEM compatibility/composition surfaces:
  `opennt_dem_common_compat`, `opennt_demfile_compat`,
  `opennt_demgset_compat`, `opennt_demsrch_fcb_compat`,
  `opennt_demerror_lock_composition`, `opennt_demmisc_*`, and
  `opennt_demdasd_ioctl_*`.
- `BOP-DIV-042` through `BOP-DIV-047` now describe the moved MISC, raw-DASD
  and error-composition non-composable historical endpoints.  In particular,
  the retained `nt_fdisk_*` source-shaped subset is not misrepresented as a
  direct OpenNT host import; FDC/recursive-SoftPC behavior remains explicit
  failure, not fabricated machine success.
- The source scan returned no `opennt-host` production source dependency on
  `opennt-bop`, no old moved path, and no `*_shim.*` file below
  `src/opennt-host/dem/`.

## Verification

- Fresh formal root: `build/M0-T261-S4/r016`.
- The graph completed 462 recorded Ninja edges.  A subsequent `ninja -C`
  reported `ninja: no work to do.`
- `t230-s7-demdasd-ioctl-direct-import-fixture.exe` passed its CCPU accessor,
  IOCTL, boundary and raw-drive failure contracts.
- `t230-s8-demerror-lock-direct-import-fixture.exe` passed its NTDLL Unicode,
  hard-error setup, retry and file-lock contracts.
- `t230-s9-demmisc-direct-import-fixture.exe` passed its lifecycle, loader and
  diagnostic service bodies.
- `git diff --check` passed.  The earlier `r015` was deliberately excluded:
  it had two concurrent Ninja invocations in the same disposable root and is
  not closure evidence.

## Interpretation and follow-up

S4 is closed: every DEM support input is now either a named host capability,
a named OpenNT BOP composition/compatibility surface, a lower adapter
dependency, or an explicitly registered unavailable historical endpoint.
This is an owner/layout closure, not a claim that FDC, VDD/debugger,
Redirector or a full original `nt_fdisk.c` product shell is recovered.  Those
owner packages remain outside S4.  The next admitted S is the separately
bounded `adapter-bop` ingress split; it must not change this DEM result.
