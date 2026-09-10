# T199 S37 DEM GSET DPB Source ABI 001

## Question

What exact OpenNT contract must the immutable CLI capability preserve for
`50:25 demGetDPB`, and what parts must remain unavailable without a BDS or a
host disk device?

## Inputs

- `src/opennt/base/mvdm/dos/dem/demgset.c`: `demGetDPB`, `demGetDpbI`, and
  `demGetDiskFreeSpace`.
- `src/opennt/base/mvdm/dos/dem/demdasd.h`: packed `DPB` and `BPB` layouts.
- `src/opennt/base/mvdm/dos/dem/demdisp.c`: `SVC_DEMGETDPB` at `50:25` and
  `SVC_DEMGETDPBLIST` at `50:46`.
- Current immutable volume and drive snapshots under `src/bx-vdm/`.

## Source contract

`demGetDPB` reads drive from `AL` and a packed 35-byte `DPB` destination at
`DS:DI`. On successful `demGetDpbI`, it returns `AX=0`, CF clear. A removable
BDS whose BPB reload fails produces CF set with AX unchanged. A failed
`demGetDiskFreeSpace` takes `demClientError(INVALID_HANDLE_VALUE, drive)`;
the original eventual DOS error is therefore inherited from the historical
host API error state and is not a free-standing fixed constant in this
routine.

For a successful volume record, `demGetDpbI` writes these packed DPB fields:

| Offset | Field | Source rule |
| --- | --- | --- |
| 0, 1 | `DriveNum`, `Unit` | requested drive |
| 2 | `SectorSize` | reduced DOS-visible bytes/sector |
| 4 | `ClusterMask` | sectors/cluster minus one |
| 6 | `ClusterShift` | count of right shifts until cluster size is odd |
| 8 | `FATSector` | fake branch: 1 |
| 10 | `FATs` | fake branch: 2 |
| 11 | `RootDirs` | fake branch: 63 |
| 15 | `MaxCluster` | total clusters plus one |
| 17 | `FATSize` | fake branch: 512 |
| 19 | `DirSector` | fake branch: 1 |
| 21 | `DriveAddr` | fake branch: `1212*64*1024+1212` |
| 25 | `MediaID` | fake branch: `0xF8` |
| 26 | `FirstAccess` | fake branch: 10 |
| 27 | `Next` | `0xffffffff` |
| 31 | `FreeCluster` | not assigned by the historical fake-DPB branch |
| 33 | `FreeClusters` | reduced DOS-visible free clusters |

The fake-DPB branch intentionally does **not** fabricate a BPB or BDS. It
also does not assign `FirstDataSector` (13) or `FreeCluster` (31-32) before
its final `FreeClusters` assignment (33-34); unassigned bytes must remain outside
the rehost write transaction. `50:46` is a separate source rule: it writes
only Drive/Unit/Next and therefore cannot serve as a full-DPB implementation.

## Disposition

The admitted immutable volume snapshot can support the success path using the
historical fake-DPB branch, after applying the same DOS-visible cluster
reduction already used by `50:0E`. It must not claim a BDS, BPB, INT13, raw
drive, or BIOS device.

For an absent/unadmitted volume, the provider cannot truthfully replay the
original ambient `GetLastError` chain. It therefore reaches the existing,
documented `demClientError` CLI replacement: AX=5 and CF set. This is an
explicit source-derived terminal result, not a fake-DPB or a raw pass-through.

`artifacts/build/t199-s37-dem-package-r41/` records the source-built MSVC x64
`/MT` regression. It verifies the fake-DPB field values from an admitted A:
record, preservation of the historical fake branch's unassigned
`FirstDataSector` and `FreeCluster` bytes, AX=0/CF clear, and the unadmitted
B: AX=5/CF termination.

## Follow-up

Keep `50:25` and `50:46` independently regressed: the former writes a full
source fake-DPB subset from immutable volume geometry, while the latter writes
only Drive/Unit/Next over its 35-byte packed stride. Continue GSET with the
machine-identity and drive-state contracts.
