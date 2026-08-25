# M0 T270 S1 P3 — Bochs formal-build closure

## Clean disposable build

The earlier `build/M0-T270-S1/r001` root was verified idle and removed because
its Ninja log had been interrupted.  A fresh, disposable `r002` graph was
generated outside the sandbox with:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T260S8FullNinjaGraph.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64 `
  -BuildRoot O:\repos.hobby\ntvdm64\build\M0-T270-S1\r002
```

The complete formal build then passed outside the sandbox:

```text
ninja -C O:\repos.hobby\ntvdm64\build\M0-T270-S1\r002 -j 8
...
[314/486] LIB lib/bochs-core.lib
...
[486/486] LINK bin/ntvdm64.exe
exit 0
```

The graph compiled all private `bochs-core-overlay` units as `bochs-core`
members, including `iodev/minimal_port_space.cc`, and linked all registered
fixtures plus the application.

## Focused boundary verification

The following focused checks passed outside the sandbox:

```text
powershell.exe -ExecutionPolicy Bypass -File \
  tests/adapter-bochs/Test-T225S23NativePicBoundary.ps1
t225-s23-native-pic-boundary: native PIC ownership and selector-blind mechanics verified

build/M0-T270-S1/r002/bin/t225-s23-native-pic-lifecycle-fixture.exe
exit 0

build/M0-T270-S1/r002/bin/t225-s23-ram-preflight-fixture.exe
exit 0
```

The fixture CPU reset diagnostics are native Bochs diagnostics; neither
fixture enabled a BOP, OpenNT or device-product semantic path.

## S1 result

The P1 owner correction, P2 normalized disposition ledger, clean formal
Ninja build and focused boundary checks meet S1's source-layout closure
criteria.  This evidence makes no guest-runtime or new-machine-capability
claim.
