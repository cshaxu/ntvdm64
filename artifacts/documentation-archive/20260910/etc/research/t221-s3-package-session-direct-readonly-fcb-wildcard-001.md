# T221 S3 P60 — Direct/Readonly FCB wildcard mutation

## Scope

The package-session fixture now covers the FCB wildcard mutation pair from
OpenNT `demfcb.c`:

- Direct `50:07` deletes `FCBD1.DAT` and `FCBD2.DAT` through the wildcard
  `FCBD?.DAT`.
- Direct `50:20` renames `FCBR1.DAT` and `FCBR2.DAT` through
  `FCBR?.DAT → REN??.TMP`, producing `RENR1.TMP` and `RENR2.TMP`.
- Readonly sends both services through the same installed package session and
  receives `CF=1, AX=5` before any namespace enumeration.

All Direct files are generated below the fixture-owned temporary C: root and
removed by the existing bounded cleanup. No ambient host file is selected.

## Source and ABI basis

OpenNT `base/mvdm/dos/dem/demfcb.c` supplies the `demDeleteFcb` and
`demRenameFcb` service family. The source-derived route keeps its guest ABI:
`50:07` reads the wildcard pathname from `ES:DI`; `50:20` reads source from
`DS:SI` and destination template from `ES:DI`. The template behavior is
intentional: literal `REN` consumes source name positions before the two `?`
slots consume `R1`, so `FCBR1.DAT` becomes `RENR1.TMP`.

Readonly is decided by the shared mutation-profile consumer, not by a
path-specific workaround. Existing Overlay/Virtual source and typed
file-view/profile seams remain untouched.

## Reproducible evidence

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\M0-T221-S3\160-direct-readonly-fcb-wildcard-rerun `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The MSVC x64 `/MT` fixture compiled and its bounded executable returned exit
code `0`.

## Remaining work

The FCB wildcard pair is one owner-family leg. FCB search (`0A,0C`), the full
Direct/Readonly search matrix, remaining FCB/handle boundary cases, and the
other DEM families remain open. This record does not claim DEM or S3 closure.