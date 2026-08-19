# T230 S2 — Direct `demhndl.c` Import Map

## Imported source

- Source: `src/opennt/base/mvdm/dos/dem/demhndl.c`
- Baseline commit: `d450bfc1b7711234072eebfdfa23735ea509e7c5`
- Destination: `src/bx-vdm/bop/opennt/dem/demhndl.c`
- Role: the directly imported source baseline for the complete DEM
  handle-service package.

The former partial implementation is not a runtime dependency.  It was moved
to `src/bx-vdm/bop/reference-v2/dem/demhndl.partial-v2.{c,h}` solely for
comparison while the imported translation unit is adapted.

## Complete original function set

| Original function | `demdisp.c` service |
| --- | --- |
| `demChgFilePtr` | `50:00` |
| `demClose` | `50:02` |
| `demFileTimes` | `50:08` |
| `demRead` | `50:16` |
| `demWrite` | `50:1E` |
| `demCommit` | `50:27` |
| `demPipeFileDataEOF` | `50:47` |
| `demPipeFileEOF` | `50:48` |

## Required shim boundary

The imported file invokes historical CCPU/SAS register and guest-pointer
helpers (`get*`, `set*`, `GetVDMAddr`, `Sim32FlushVDMPointer`,
`Sim32FreeVDMPointer`), the historical `GETHANDLE(AX,BP)` representation,
`demClientError`, and optional VdmRedir named-pipe calls.  These are the only
classes admitted for a T230 S2 shim:

1. copied CPU register read / typed-result write;
2. checked guest-memory span acquire / commit / release;
3. session-local opaque token to native-width `HANDLE` lookup and release;
4. source-shaped DEM error publication; and
5. an explicit Redirector-unavailable disposition for the optional named-pipe
   branch.

No v1 provider, mutation profile, legacy dispatcher or selector knowledge is
an admissible shim input.  Each edit to the imported `demhndl.c` must retain an
adjacent comment with this source path, the unavailable historical helper and
the named replacement shim.

## Implemented seam and local witness

`bop/shim/demhndl_shim.{h,c}` now supplies one bounded, scoped invocation
record. It has no selector decoder, provider registry, mutation policy or v1
dependency. The shim:

- reads only the copied real-mode CPU record and produces a typed resume
  result;
- resolves the OpenNT `AX:BP` handle word pair through a session-local opaque
  token, so no native-width `HANDLE` crosses the guest ABI;
- performs guest spans through explicit checked read/write callbacks; its
  read bounce buffer is seeded before a host read, preserving OpenNT's
  direct-pointer failed/short-read behavior;
- preserves the original `demClientError` register contract and records the
  original INT 24 class as a pending hard error for the later `demerror.c`
  package, rather than inventing a hard-error handler; and
- reports Redirector pipe state only through an optional opaque pipe callback.
  With no Redirector package composed, `IsVdmRedirLoaded()` is false, exactly
  selecting the original ordinary-file path.

The only executable-source divergence is the adjacent `CloseHandle` call in
the imported file: it is replaced with the named shim close operation so the
original close also retires the opaque guest token. The existing `demmsg.c`
mirror supplies `demPrintMsg`; no message behavior was recreated in the shim.

`tests/bx-vdm/t230_s2_demhndl_direct_import_fixture.c` source-builds and runs
through the formal Ninja graph. It executes all eight imported service entry
points, proves host write → guest read round-trip, guest-memory commit, typed
CF/ZF result bits, opaque-token close/retirement, and an original
`ERROR_INVALID_HANDLE` failure result. The fresh disposable recipe was:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\build\New-T225S7FullNinjaGraph.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T230-S2\direct-demhndl-r2
ninja -C O:\repos.hobby\ntdos64\build\M0-T230-S2\direct-demhndl-r2 `
  lib/bx-core.lib bin/t230-s2-demhndl-direct-import-fixture.exe
& O:\repos.hobby\ntdos64\build\M0-T230-S2\direct-demhndl-r2\bin\t230-s2-demhndl-direct-import-fixture.exe
```

The fixture prints `T230 S2 direct OpenNT demhndl import: handle,
guest-memory and result seam verified` and exits zero. This is local Direct
package evidence only; it does not claim selector routing, full `demerror.c`
INT 24 composition, Redirector composition, or native guest integration.
