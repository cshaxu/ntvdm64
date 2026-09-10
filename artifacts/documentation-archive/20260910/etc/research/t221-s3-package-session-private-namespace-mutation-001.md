# T221 S3 P52 — Package-session private namespace mutation

## Scope

Overlay and Virtual now exercise the complete pathname namespace mutation
quartet through the normal package session after their private pathname search:

- `50:17` renames `C:\\FCBTWO.COM` to `C:\\FCBREN.COM`;
- `50:05` deletes that renamed file;
- `50:04` creates `C:\\FCBDIR`; and
- `50:06` removes the empty private directory.

The service numbering is source-derived, not inferred from the test: OpenNT
`base/mvdm/inc/dossvc.h` defines `SVC_DEMDELETE = 0x05` and
`SVC_DEMDELETEDIR = 0x06`. The package-session route retains that contract.

## Boundary

All four calls cross normal ingress, registry and package-session dispatch with
bounded guest path copies. Overlay selects its relocation/tombstone namespace
backend; Virtual selects its private-root backend. Neither leg writes the host
namespace or introduces a BOP-specific adapter shim.

This is a namespace-mutation family leg, not proof that the DEM package is
closed. Direct/Readonly mutation semantics and the remaining owner families
still require their complete matrix evidence.

## Reproducible evidence

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\M0-T221-S3\151-private-namespace-mutation-corrected `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The generated manifest records MSVC x64 `/MT` source-built link and run exit
codes `0`.

## Remaining work

This P52 leg does not close FCB first/next, Direct/Readonly FCB coverage,
metadata, time, error/misc, GSET, DASD, device/state services, or the final
29-identity matrix.