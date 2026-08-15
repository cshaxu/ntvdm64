# T202 S1 DEM Shared-Profile Consumer Composition

T202 S1 installs the common mutation profile at the DEM package-session and
boot-namespace-composition boundary. The package consumer registers a DEM
owner/class against the shared record and resolves the session-selected mode
without accepting a selector, service number, host path, guest address, host
handle or caller-supplied mode.

The session keeps the consumer optional while existing profile backends are
being migrated; installing it is one-shot and copied. An unregistered mutation
class is rejected. The seam itself performs no host I/O or provider dispatch.

Reproduction:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T202S1DemProfileConsumerProbe.ps1 `
  -RepositoryRoot (Get-Location).Path `
  -BuildRoot artifacts\build\t202-s1-dem-profile-consumer-r1

powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot (Get-Location).Path `
  -BuildRoot artifacts\build\t202-s1-dem-profile-session-r2 `
  -Fixture dem-profile
```

Both MSVC x64 `/MT` fixtures passed. The latter links the actual DEM package
session and boot-namespace composition closure; its mantle mechanical-action
stub is a decline-only test seam, not a machine backend.
