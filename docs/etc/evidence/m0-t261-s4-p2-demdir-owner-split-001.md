# M0 T261 S4 P2 — DEM directory owner split

## Question

Can the reached OpenNT `demdir.c` provider retain its original service and OEM
call sequence while moving BOP invocation out of `opennt-host` and moving the
old OEM directory API shape to `adapter-win32`?

## Change

- Moved the project-owned `bx_ntvdm_demdir_invoke` composition source and its
  packed CDS/service declarations by Git rename to
  `src/opennt-bop/dem/opennt_demdir_composition.{c,h}`.
- Moved only the source-visible historical OEM directory calls
  `CreateDirectoryOem`, `RemoveDirectoryOem` and `SetCurrentDirectoryOem` to
  `adapter-win32/include/legacy-api/opennt_dem_oem_facade.h` and
  `adapter-win32/facade/opennt_dem_oem_facade.c`.
- The facade retains OEM-to-ANSI conversion, parameters, call order and
  public Win32 last-error behavior. `SetEnvironmentVariableOem` remains the
  existing COMMAND composition export because its historical state owner is
  not directory capability.
- Changed only `demdir.c`'s unavailable historical include closure, marked as
  `DIVERGENCE(BOP-DIV-038)`, and registered the public-Win32 replacement as
  `WIN32-DIV-005`.
- Made `demgset_shim.h` explicitly include its actual low-level DEM type
  dependency (`demfile_shim.h`) instead of relying on the former transitive
  directory header.

## Boundary

`opennt-bop` selects and scopes the original `demCreateDir`, `demDeleteDir`,
`demQueryCurrentDir` and `demSetCurrentDir` bodies. `adapter-win32` provides
only their same-named host OEM calls. `opennt-host` no longer owns the
directory BOP composition and does not acquire an upward dependency.

## Verification

- Fresh formal MSVC x64 `/MT` root: `build/M0-T261-S4/r010`.
- The full graph completed 459 recorded Ninja edges and linked all declared
  component libraries, fixtures and native executable.
- A repeated invocation reported `ninja: no work to do.`
- `t230-s4-demdir-direct-import-fixture.exe` returned zero and reported:
  `T230 S4 direct OpenNT demdir import: directory and CDS services verified`.
- `git diff --check` and documentation governance verification passed.

## Result

The original DEM provider retains its service/error control flow; only the
unavailable product-host include closure is split at explicit component
boundaries. This removes one generic DEM shim from `opennt-host` without
changing Redirector, mutation policy, or guest/Bochs behavior.
