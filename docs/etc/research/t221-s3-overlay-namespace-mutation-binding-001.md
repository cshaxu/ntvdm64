# T221 S3 Overlay namespace-mutation binding

## Question

Can `50:04`, `50:05`, `50:06`, and `50:17` enter the same completed Overlay
mutation backend through the ordinary DEM namespace route while retaining
their original path/register/failure shape and all profile dispositions?

## Inputs

- Original owner sources: `demdir.c` and `demfile.c`.
- Backend record: `t221-s3-overlay-mutation-backend-001.md`.
- Route and binding:
  `src/bx-vdm/bx_ntvdm_dem_namespace_route_partition_v1.c` and
  `src/bx-vdm/bx_ntvdm_dem_namespace_partition_v1.c`.

## Binding

The namespace route continues to gather bounded OEM paths through its existing
checked guest-read transaction. The namespace partition retains:

- `demCreateDir`'s `BX:SI` EA refusal;
- `demRename` destination gather, cross-drive (`AX=11h`) and same-name
  (`AX=5`) decisions;
- startup-image protection and existing `demClientError`-shaped failure
  conversion.

Only after the package-level mutation profile resolves to Overlay does the
partition call the selector-blind mutation backend. Direct retains admitted
host operations; Readonly remains access denied; Virtual remains explicitly
unavailable. No handler recognizes a profile independently.

## Verification

MSVC x64 `/MT` whole-provider regression passed:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T202S4DemWholeProviderProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\058
```

The family fixture proves Overlay route success for COW directory create/remove,
host-base file rename and subsequent delete, without changing the temporary
host file. Existing Direct, Readonly, and Virtual regression sections retain
their respective host, `AX=5`, and explicit-unavailable outcomes.

## Limit

This closes the Overlay binding of this namespace-mutation subfamily only.
It does not close `50:01/44`, metadata/time, FCB/search, Virtual, open-handle
delete/rename sharing, advanced relocation conflicts, or the whole DEM
package. Native trace remains out of scope until T221 package regression is
complete.
