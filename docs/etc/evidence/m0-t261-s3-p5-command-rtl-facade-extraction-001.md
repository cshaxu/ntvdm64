# M0 T261 S3 P5 — COMMAND RTL facade extraction

## Question

Can the historical RTL/NTDLL compatibility surface used by recovered COMMAND
sources move to `adapter-win32` without letting that adapter depend on BOP
session state or changing provider behavior?

## Change

- Added `adapter-win32/include/legacy-api/opennt_rtl_types.h`, the one
  host-private OpenNT counted-string ABI declaration used by the facade and
  recovered COMMAND callers.
- Added `adapter-win32/facade/opennt_rtl_compat.c`, owning the reached
  `Rtl*` string conversion, environment, allocation, and
  `RtlNtStatusToDosError` compatibility functions.
- Removed those functions from BOP COMMAND composition.  The retained BOP
  files now own only COMMAND session state, source selection, guest-related
  address calculation, and environment snapshot composition.
- Registered the modern-public-Win32 replacement as `WIN32-DIV-004`; removed
  the transitional BOP-owned status-mapping entry.

## Boundary

`adapter-win32` exposes the same historical names and host-private layouts,
but never sees BOP selectors, guest pointers, guest registers, session
records, or Bochs types.  `opennt-bop` keeps the original COMMAND callers and
their ordering; it supplies only ordinary host-private string records to the
facade.

## Verification

- Fresh formal graph: `build/M0-T261-S3/r005`.
- The new adapter facade and all three affected BOP composition objects
  compiled successfully.
- `lib/adapter-win32.lib` and `lib/opennt-bop.lib` built successfully; a
  follow-up Ninja query reported no work.
- `t231-s6-command-check-binary-direct-fixture.exe` passed.
- `t234-s2-command-dynamic-environment-fixture.exe` passed.
- Component-manifest and production-tree gates passed.

## Scope

This is a recovery-ladder facade extraction, not a replacement COMMAND
provider.  It does not alter COMMAND BOP admission, child/session lifecycle,
or PIF behavior.
