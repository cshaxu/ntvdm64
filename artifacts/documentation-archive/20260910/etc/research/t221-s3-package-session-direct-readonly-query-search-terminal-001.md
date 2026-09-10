# T221 S3 P63 — Direct/Readonly query and search terminals

## Scope

This package-session matrix leg covers read-only query and terminal behavior
that does not depend on ambient host C: ordering:

- `50:01` returns the source-shaped read-only metadata result for the declared
  `C:\COMMAND.COM` image;
- `50:44` checks the fixture-owned, real temporary host-C: directory;
- `50:09` pathname first-search and `50:0A` FCB first-search each receive the
  valid 8.3 pattern `NTDNOPE?.ZZZ` below that empty owned directory and return
  `CF=1, AX=18` (`ERROR_NO_MORE_FILES`).

The same cases pass in Direct and Readonly sessions.

## Source and recovery basis

OpenNT `demfile.c` supplies the attribute/check-path owners and
`demsrch.c::demFindFirst` / `demFindFirstFCB` define the first-search failure
contract. Their historical translation units remain tied to CCPU/SAS and the
old host composition, so the existing `dem_whole_provider` source-derived
seam is used. It retains bounded guest gathers, result-register layout and
error behavior through fixed typed transactions; no new BOP provider, guest
ABI, Bochs change or external intrusion is introduced.

The first failed witness used an invalid long DOS component and therefore
exercised parsing rejection instead of no-match. P63 deliberately uses a DOS
8.3-valid pattern, making `AX=18` a meaningful source-shaped terminal test.

## Reproduction

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\166-direct-readonly-query-search-terminal-8dot3 `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The MSVC x64 `/MT` fixture compiled and the bounded executable returned exit
code `0`.

## Follow-up

This fills query and first-search terminal coverage, not the complete 29-item
Direct/Readonly family matrix. Remaining negative token/continuation and
lifecycle legs are still subject to the active T221 S3 closure audit.