# T221 S3 P43 — Whole-provider session integration

## Scope

This record adds a narrow integration witness for the installed
`dem_whole_provider` inside the normal DEM package session.  It is not a
replacement for the required 29-identity, four-profile family regression.

## Fixture

`tests/bx-vdm/bx_ntvdm_dem_whole_provider_session_v1_test.c` constructs a
normal boot namespace plane, registers the three shared DEM mutation classes,
admits host `C:`, installs it through
`bx_ntvdm_dem_package_session_v1_set_host_namespace`, and sends real-mode
`C4 C4 50 xx` through ingress, registry, and package-session dispatch.

The fixture proves:

- `has_whole_provider` becomes true only after the shared profile and admitted
  host namespace are installed.
- `50:00` with a zero opaque token returns the provider's source-shaped
  invalid-handle result (`AX=6`, `CF=1`), rather than generic CLI unavailable
  (`AX=5`).
- `50:47` and `50:48` retain the explicit Redirector-deferred invalid-handle
  result.  They do not become file-provider fallbacks.

## Reproducible evidence

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\M0-T221-S3\135 `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The produced manifest records MSVC x64 `/MT`, a source closure that excludes
`bx-core`, `bx-mantle`, Bochs binaries and an OpenNT runtime, and reports
compile/link/run exit code `0`.

## Remaining work

P43 verifies the package installation seam only.  It does not demonstrate
every selected namespace, handle, FCB and search identity in all Direct,
Readonly, Overlay and Virtual profiles.  That owner-family matrix remains the
S3 exit criterion; native trace remains prohibited until it closes.
