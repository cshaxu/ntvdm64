# M0 T261 S4 P4 — DEM global-setting owner split

## Change

- Moved the service selector, scoped DTA registration and direct imported
  `demgset.c` invocation source by Git rename to
  `opennt-bop/dem/opennt_demgset_composition.c`.
- Moved the two same-named historical OEM host queries,
  `GetComputerNameOem` and `GetDiskFreeSpaceOem`, into an `adapter-win32`
  public-Win32 facade.
- Retained host-drive admission and namespace policy in `opennt-host`; the
  BOP composition merely calls the imported provider through declared seams.
- Retained the thread-scoped clock-test writer with the BOP session
  composition.  Production still calls `SetLocalTime`; the writer exists only
  to prevent fixture mutation of the host clock.
- Marked the sole imported include divergence as `BOP-DIV-040` and registered
  the public API replacement as `WIN32-DIV-007`.

## Boundary

`opennt-bop` owns service-number interpretation and the copied guest DTA
state that later DEM providers consume.  `adapter-win32` owns only the
same-shaped public host queries.  `opennt-host` retains drive admission, so no
generic global-setting BOP composition or upward BOP dependency remains there.

## Verification

- Fresh formal MSVC x64 `/MT` root: `build/M0-T261-S4/r012`.
- The full graph completed 460 Ninja edges, linked all component libraries,
  declared fixtures and the native executable, and then reported
  `ninja: no work to do.`
- `t230-s6-demgset-direct-import-fixture.exe` returned zero:
  `DTA registration, drive, clock and search continuation verified`.
- `git diff --check` and documentation governance verification passed.

## Result

The original `demgset.c` body remains the DEM semantic owner.  Its prior
generic host shim is now split into explicit BOP composition, public-Win32
facade and host drive-policy owners, without creating a second mapping or
handle manager.
