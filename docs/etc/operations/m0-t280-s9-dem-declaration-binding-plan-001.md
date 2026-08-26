# M0 T280 S9 — DEM declaration binding form

S9 binds only the exact **storage** declaration carriers imported by S8 into
the existing same-shaped `adapter-win32` include facade. It changes only the
facade's declaration visibility: no historical API implementation, source
body, BOP, or guest-memory mechanism is enabled. The complete `ntioapi.h`
surface is explicitly deferred to its own source-shaped ABI package.

The original `devioctl.h` carrier already mirrored by `opennt-platform-abi`
defines `DEVICE_TYPE`, which the exact OpenNT `ntddstor.h` requires. The
smallest modern build-form adaptation is for the existing facade to include
that same original carrier before original `dem.h` reaches `ntdddisk.h`.
The declared compile root puts platform DDK/SDK include roots after
`adapter-win32`; normal `<nt.h>` therefore remains the adapter-owned facade,
while the exact nested storage declarations resolve from the platform mirror.

## Exit criteria

- adapter-win32 records the one declaration-only divergence;
- a focused x86/x64 syntax fixture proves original `dem.h` resolves the
  original storage carrier rather than the current SDK substitute;
- no DEM `.c` body is compiled, linked, or invoked; and
- no mapping-manager operation or host API call is added.
