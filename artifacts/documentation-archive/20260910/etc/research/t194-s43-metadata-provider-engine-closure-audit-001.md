# M0 T194 S43: Metadata Provider Engine Closure Audit

## Result

The S39 provider requires a narrow current-adapter derivative, not a Bochs
rebuild or new device/archive closure. The retained S32 engine has 68 adapter
objects and five CLI objects. Its only Bochs replacement is the previously
enabled generic-listener `cpu\exception.o` (SHA-256
`AE901987786C9C8743170F4319943AE0A97C4B8EDAC94C98BB9E64CB4FF4786D`).
`main.o`, CPU, iodev, memory and GUI archives remain retained inputs.

## Exact Delta

`New-T98S1CurrentAdapterEngineDerivative.ps1` is the sole source-list owner.
Its 68-entry adapter list still names deleted
`bx_ntvdm_dem_ioctl_changeable_service.c`; the retained S32 root consequently
contains its stale source/header/object. The S39 source closure changes the
list exactly as follows:

| Remove | Add | Object consequence |
| --- | --- | --- |
| `bx_ntvdm_dem_ioctl_changeable_service.c` | `bx_ntvdm_dem_ioctl_metadata_provider_v1.c` | replace one adapter object; total remains 68 |
| old runtime source hash | current `bx_ntvdm_adapter_runtime.c` | recompile the existing runtime object |

The new provider calls ingress, registry, DEM plane, drive policy, exception,
CPU state/delta/result APIs. Every corresponding source already appears in the
68-entry list. The five CLI sources are unchanged. There is no `host_namespace`
admission, new external library, guest-memory transport, device, archive, or
Bochs source/object requirement.

## Candidate

S44 may change only the generator's source list and its static invariants,
then generate a fresh root. Its expected makefile compiles 68 adapter and five
CLI objects, with the existing `-BopRegisterObservation` form rebuilding only
`cpu\exception.o`; it retains all other Bochs objects/archives. A separate S
must perform the actual MSVC build/link, and a later separate S must authorize
any observation.
