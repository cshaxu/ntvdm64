# M0 T194 S38: DEM IOCTL Metadata Composition Audit

Packet: M0 T194 S38  
Disposition: a narrow paired-provider implementation is admitted; no runtime,
build, or source modification occurred during this audit.

## Question

Can the reached `50:21 AX=4408` request be restored as part of a coherent
OpenNT-shaped provider rather than retained as a one-off legacy responder?

## Historical Contract And Provenance

The pinned OpenNT copy
`src/opennt/base/mvdm/dos/dem/demioctl.c` is recorded in
`src/opennt/SOURCE-MANIFEST.json` as `upstream-copy`, SHA-256
`6F480BF758D071B1FD2F76A137F892B77D1EA8D2D8FFB4690D395AEDF5F733FA`.
Its `apfnSVCIoctl` maps both AL `08` and `09` to `demIoctlChangeable`.
`demIOCTL` selects only AL; `demIoctlChangeable` reads BL as a zero-based
drive letter.

| AL | Original result from `demIoctlChangeable` | Provider status |
| --- | --- | --- |
| `08` | CF clear; AX=0 for `DRIVE_REMOVABLE`, otherwise AX=1. | Admitted with `09` as one metadata group. |
| `09` | CF clear; AL=0; DX=1000h for `DRIVE_REMOTE` or `DRIVE_CDROM`, otherwise DX=0800h. | Admitted with `08` as one metadata group. |
| `0D`, `11` | `demIoctlDiskGeneric`/`demIoctlDiskQuery`; BDS state, SI:DX structures, mutable media/device calls and error translation. | Deferred together; no partial success. |

The original function first asks `demGetPhysicalDriveType` and then calls
`GetDriveTypeOem` for unknown drives. That CCPU/Win32 host composition cannot
be independently linked into the CLI runtime. The exact source-derived
replacement boundary is the immutable CLI drive snapshot: it supplies only
the resulting 26 type values and performs its one host observation at adapter
installation, not during a BOP.

## Current Composition

The current legacy `bx_ntvdm_dem_ioctl_changeable_service_v1_dispatch` accepts
only `AX=4408` and an unstructured static `host_drive_inventory.types[26]`
array. Its focused test explicitly rejects `4409`. In
`bx_ntvdm_adapter_runtime.c` it is called only after
`bx_ntvdm_legacy_plane_gate_v1_dem(window, 0x21)`, but it is after the
independent live-snapshot providers and is disabled whenever legacy static
profile inventory is absent. S36's v6 profile has that absence, so the
source-confirmed `4408/BL=02` passed through.

The correct current seam already exists:

```text
typed #UD event + copied window
  -> BOP ingress (50:21) -> provider registry -> DEM plane (GSET)
  -> immutable host_drive_snapshot_v1
  -> paired metadata provider -> typed CPU result
```

`host_drive_snapshot_v1_capture_from_environment` runs once during adapter
installation. Its policy has no BOP call path, opens no host path, and retains
only admitted Win32 type values. `dispatch_dem_drive_snapshot` and
`dispatch_dem_dpb_snapshot` demonstrate the established common-plane and
snapshot-only integration pattern, but neither implements IOCTL metadata.

## Implementation Boundary

S39 may add one adapter-local source-derived provider with all of the following
constraints:

1. Its entry inputs are ingress, provider selection, DEM plane, immutable
   snapshot, copied exception and CPU state; it does not accept a raw type
   array or query the host.
2. It recognizes `50:21` only after the common path proves DEM/GSET ownership,
   then accepts exactly `AX=4408` and `AX=4409` in real-mode vector-6 events.
3. It implements the two register/CF results above, validates BL against the
   snapshot admission/type domain, and otherwise passes through unchanged.
4. Runtime selection replaces the legacy static endpoint. The old source,
   test, CMake target, include and runtime call site are removed together, not
   left as a parallel fallback.
5. Focused tests cover fixed/removable/remote/CD results, an excluded or
   unknown drive, wrong AL/selector/service, and raw-media `0D/11` pass-through.

This is a source-derived rehost because the original CCPU/Win32 composition is
unavailable, not an assertion that the original function is independently
composable. It changes neither Bochs nor OpenNT source and requires no
exception-register entry.

## Follow-up

S39 is admitted as the sole implementation slice. A build or one-shot runtime
observation requires later explicit admission; S39 itself is limited to source
and focused test closure.
