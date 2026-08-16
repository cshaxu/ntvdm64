# T221 S3 P66 — Direct/Readonly stale-token lifetime

## Question

Does a released opaque `AX:BP` file token retain the original DEM invalid-handle
failure contract in both admitted host-mutation profiles, without falling back
to a profile default or a host handle?

## Inputs

- OpenNT `src/opennt/base/mvdm/dos/dem/demhndl.c::demClose` and its paired
  handle-service family;
- `src/bx-vdm/bx_ntvdm_dem_file_session_v1.c`, whose generation-bearing token
  invalidates a released slot;
- `src/bx-vdm/bx_ntvdm_dem_handle_partition_v1.c`, which emits
  `ERROR_INVALID_HANDLE` (`AX=6`, `CF=1`) when the typed lookup fails;
- `tests/bx-vdm/bx_ntvdm_dem_whole_provider_session_v1_test.c`.

## Procedure

The package-session fixture opens the declared `C:\\COMMAND.COM`, reads one
byte through the checked guest-RAM path, and closes the opaque token.  For
Direct and Readonly only, it then submits `50:00` (`demChgFilePtr`) with that
released token and requires `CF=1, AX=6`.

The historical `demhndl.c` translation unit cannot be independently linked:
it is coupled to the legacy SoftPC/CCPU register and VDM-address composition
(`getAX`, `getBP`, `GETHANDLE`, `GetVDMAddr`) and Redirector host shell.  The
existing source-derived provider preserves the handle ABI, close ordering and
invalid-handle failure rule behind typed adapter tokens.

## Observation

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\169-direct-readonly-stale-token `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The focused MSVC x64 `/MT` fixture compiled and its executable returned exit
`0`.

## Interpretation

Direct and Readonly now share a verified token-lifetime terminal: a released
file capability cannot be reused, cannot become a raw Win32 handle, and does
not fall through to a profile-default outcome.  No provider behavior changed;
this is a two-profile package-regression addition.  Overlay and Virtual code
and behavior remain frozen.

## Follow-up

Continue the T221 29-identity Direct/Readonly matrix closure audit.  This
record does not claim DEM package closure or authorize trace-led BOP work.
