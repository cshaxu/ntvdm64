# T202 S2 DEM Namespace/CWD/Volume Source and ABI Map

## Package boundary

This is one DEM owner package, not a collection of trace-selected BOP fixes.
Its original sources are `demdir.c` and `demgset.c`; its modern platform seam
is the selected-drive snapshot, volume snapshot and adapter-private root-handle
namespace established in S30.

| Original service group | Original contract | Direct profile | Readonly | Overlay | Virtual |
| --- | --- | --- | --- | --- | --- |
| `50:13 demQueryCurrentDir` | Validate the selected drive; validate a CDS path; reset an invalid path to root and update per-drive current-directory state. | Validate only under an admitted real root; copy the resolved DOS path/state back. | Same read/validation, no host mutation. | Read/validate base then use VDM overlay CWD state. | Resolve entirely against virtual-volume state. |
| `50:18 demSetCurrentDir` | `SetCurrentDirectoryOem` then update `=X:` environment state; source failure remains a DOS error. | Resolve an 8.3 DOS path under an admitted root and set VDM session CWD; no ambient process CWD mutation. | Reject mutation through the common profile result. | Store per-drive CWD in overlay, after base-path validation. | Store in virtual-volume state. |
| `50:1A demSetDefaultDrive` | Verify `DL` agrees with the path drive, then `SetCurrentDirectoryOem`; invalid drive/path preserves error. | Same drive/path guard, then selected-root VDM session CWD update. | Reject mutation through the common profile result. | Overlay per-drive CWD update. | Virtual-volume CWD update. |
| `50:0D,0E,0F,10,14,15,19,1B,1C,25,41,46` GSET/volume state | Drive count/type, media/volume, boot/default drive, free space, date/time, DTA, DPB and machine identity use host/machine observations and source-specific errors. | Use copied selected-drive/volume snapshot and permitted host observations; preserve excluded-drive failure. | Same read-only observation. | Overlay only mutable context/time classes; immutable volume facts remain copied observations. | Virtual-volume facts are virtual state. |

## Existing code disposition

| Surface | Current behavior | Required T202 action |
| --- | --- | --- |
| `bx_ntvdm_host_namespace_v1` | Opens selected real roots once and safely enumerates relative 8.3 directories; no host path/handle crosses its boundary. | Retain as direct profile root owner; add a checked relative-directory validation/query contract rather than exposing paths or handles. |
| `bx_ntvdm_dem_current_dir_service_v1` | Reads a CDS/root request but collapses successful `50:13` to root and only accepts root-shaped `50:18/1A` inputs. | Replace with package CWD state over the shared profile. It is not a direct-host implementation. |
| `bx_ntvdm_dem_default_drive_service_v1` | Synthesizes failure after a one-byte read. | Retire/migrate into the complete CWD/default-drive package. |
| `bx_ntvdm_dem_gset_plane_v1` and volume snapshot | Provides copied drive/volume facts and selected original-shaped service helpers. | Retain as immutable observation owner; route mutable context through common profile resolution. |
| boot/readonly namespace | Deterministic fixture/legacy profile state. | Retain only for declared readonly or virtual profile; never use it as implicit direct host C:. |

## API and ownership verdict

`SetCurrentDirectoryW`, `GetCurrentDirectoryW`, `GetFileAttributesW`,
`GetLogicalDrives`, `GetDriveTypeW`, `GetDiskFreeSpaceW` and
`GetVolumeInformationW` are ordinary user-mode APIs. They are not prohibited
by the non-invasive CLI model. The direct provider must still avoid changing
ambient process CWD: VDM session CWD is capability state anchored to admitted
root handles. Registry BootDir reads are permitted only when explicitly
selected; direct startup may instead use the CLI-admitted drive state and must
never fabricate a host C: when it was excluded.

The old `GetVDMAddr` and register macros are not usable APIs. Their only
modern replacement is the existing copied, checked guest gather/write ABI.
No unavailable historical API blocks this package: the missing work is the
source-derived state composition and exact DOS record/failure mapping.
