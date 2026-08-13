# M0 T194 S39: Paired DEM IOCTL Metadata Provider Source Closure

Packet: M0 T194 S39  
Disposition: source and focused-test target migrated; compilation/execution is
intentionally deferred to S40.

## Change

The legacy static `bx_ntvdm_dem_ioctl_changeable_service` source, header,
test and CMake target were removed. Its runtime call was replaced by
`bx_ntvdm_dem_ioctl_metadata_provider_v1`, which accepts only the common BOP
ingress, registry selection, DEM-plane record, immutable
`host_drive_snapshot_v1`, copied exception event and copied CPU state.

The provider preserves original paired semantics:

| Input | Result |
| --- | --- |
| `50:21`, `AX=4408` | Resume after four BOP bytes; CF clear; AX=0 for removable, otherwise AX=1. |
| `50:21`, `AX=4409` | Resume after four BOP bytes; CF clear; AL=0; DX=1000h for remote/CD-ROM, otherwise 0800h. |
| Other AL, unavailable snapshot drive, wrong selector/service/plane, non-real-mode or invalid typed input | Pass through unchanged. |

The original `50:21` parent remains the DEM `RAW_MEDIA` component because its
other AL branches are raw-media operations. The provider checks that parent
identity then admits only `08/09`; `0D/11` are not selected. It performs no
host query, path access, guest-memory access, device operation, or
Bochs/OpenNT source change.

## Test Surface

`bx_ntvdm_dem_ioctl_metadata_provider_v1_test.c` constructs the common
ingress/registry/plane path and an immutable snapshot. It covers
fixed/removable, remote/CD-ROM/local results; raw `0D/11`, an excluded drive,
and wrong selector/service rejection. The legacy recognizer register no longer
names the removed direct byte recognizer.

Static scans found no remaining live source, test or CMake reference to
`dem_ioctl_changeable_service`; retained historical evidence continues to name
it as a superseded record. `git diff --check` passed for the source change.

## Limitation And Follow-up

S39 explicitly excludes a build, so its new focused target has not yet been
configured, compiled or run. S40 is admitted solely to create an isolated
MinGW C11 test build and execute that target; it must not relink Bochs, build a
runtime derivative, launch the CLI, or observe a guest.
