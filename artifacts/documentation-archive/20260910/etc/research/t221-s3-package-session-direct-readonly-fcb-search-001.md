# T221 S3 P61 — Direct/Readonly FCB search continuation

## Question

Does the installed whole-provider package session preserve OpenNT FCB search
state across `50:0A` (`demFindFirstFCB`) and `50:0C` (`demFindNextFCB`) for
both currently admitted host-mutation profiles?

## Source-first disposition

The original owner is OpenNT `base/mvdm/dos/dem/demsrch.c`:
`demFindFirstFCB` reads `DS:SI` SRCHBUF and `ES:DI` wildcard path, initializes
its find-state fields, and fills the SRCHBUF; `demFindNextFCB` consumes that
same SRCHBUF state. The original translation unit is not independently
composable in this CLI process because it binds CCPU/SAS pointer conversion,
Win32-era find-list allocation, and historical host composition.

The retained smallest compatibility seam is the existing source-derived
`bx_ntvdm_dem_fcb_search_service_v1` plus `dem_search_partition_v1`: it copies
only the fixed 53-byte SRCHBUF, 128-byte pathname and 2-byte PDB value through
checked mechanical actions, then uses the package-owned typed search session.
It retains the input order, continuation storage and no-more-files result; no
Bochs change or new guest ABI is introduced. No external-code intrusion or
newly authored replacement is used.

## Procedure and observations

The package fixture initializes its existing DTA/PDB transport, sends
`50:0A` with `DS:SI=0000:0600` and `ES:DI=0000:0700` (`C:\*.COM`), verifies
that the returned SRCHBUF contains nonzero continuation fields at offsets
32–39, then sends `50:0C` against the same `DS:SI` buffer. Both Direct and
Readonly resume successfully.

The assertion intentionally does not name the first host-C: match. Direct and
Readonly merge admitted declared images with the actual host root, whose sort
contents are environment-dependent; the stable contract is the typed
continuation and its successful consumption, not an incidental first name.

## Reproducible evidence

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\162-direct-readonly-fcb-search-rerun `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The MSVC x64 `/MT` fixture compiled and its bounded executable returned exit
code `0`.

## Follow-up

This closes the positive FCB search pair only. The complete Direct/Readonly
29-identity matrix, remaining terminal/no-match legs and other DEM families
remain open. Existing Overlay/Virtual code, evidence and selector-blind seams
remain retained without new behavior.