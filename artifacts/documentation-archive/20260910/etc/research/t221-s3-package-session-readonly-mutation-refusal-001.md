# T221 S3 P58 — Readonly package-session mutation refusal

Readonly now proves capability-based refusal through normal ingress/session:
`50:1E` write on a declared token, and `50:03/22/04/05/06/17` namespace
mutations all return `CF=1, AX=5`. Paths are syntactically valid test-owned
C: paths, so failure is profile policy rather than file absence. Existing
Overlay/Virtual branches are not invoked or changed.

Reproduce with:
```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\M0-T221-S3\157-readonly-mutation-refusal `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```
The MSVC x64 `/MT` manifest records link/run exit `0`.

This is a mutation-family negative leg, not two-profile DEM closure.