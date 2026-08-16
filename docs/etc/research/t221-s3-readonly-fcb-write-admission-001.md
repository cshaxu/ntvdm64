# T221 S3 P64 — Readonly FCB I/O write admission

## Finding

`50:2F` is `demFCBIO` (`demfcb.c`). Its `BX=0` branch writes the registered
DTA bytes to the file, whereas `BX!=0` reads into the DTA. The prior local
backend applied shared profile admission while opening a token, but a read-only
FCB token could later reach its write branch. That was a genuine profile
boundary gap.

## Correction

The existing FCB handle partition now resolves
`BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT` through its existing
`dem_file_view_v1` before the direct-token `WriteFile` branch. Only a
`DENIED_READONLY` decision returns source-shaped access denied (`CF=1, AX=5`).
Direct remains admitted. Overlay and Virtual reach their already-existing
private branches before this check, so no frozen-profile behavior is changed.

This is a shared provider-view enforcement correction, not a selector-local
or FCB-specific capability substitute. The original translation unit cannot
compose directly because of CCPU/SAS and historical host dependencies; the
smallest retained source-derived compatibility seam preserves its write/read
selection, DTA transport and error ABI.

## Evidence

The whole-provider fixture opens `kernel32.dll` with an FCB read token in the
Readonly session, submits `50:2F` with `BX=0`, and verifies `CF=1, AX=5`.
The subsequent `BX=1` read and close still succeed, proving the denial does
not invalidate the read token.

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\167-readonly-fcb-write-admission `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The MSVC x64 `/MT` fixture compiled and the bounded executable returned exit
code `0`.

## Follow-up

The repair closes this readonly mutation gap only. T221 still requires its
complete 29-identity two-profile matrix and closure audit.