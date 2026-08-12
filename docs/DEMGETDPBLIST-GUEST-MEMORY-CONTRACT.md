# DEMGETDPBLIST Guest-Memory Contract

Status: M20 source evidence and neutral transaction requirement, 2026-08-08.

## Historical Service

`SVC_DEMGETDPBLIST` is `0x46`. NTDOS `DosInit` first initializes character
devices, sets `BP` to its free DOS-data offset and `ES` to the DOS-data
segment, then invokes this service.

The original `demGetDPBList` receives `ES:BP`, converts it with
`GetVDMAddr`, scans `PhysicalDriveTypes[0..25]`, and includes only
`DRIVE_REMOVABLE` and `DRIVE_FIXED` entries. For each included drive it writes
to guest memory:

```text
DPB.DriveNum = drive letter index
DPB.Unit     = drive letter index
DPB.Next     = far pointer to the following DPB
```

It terminates the final `Next` with `FFFFFFFFh` and sets `BP` to the first byte
after the resulting list. If no drive is included, BP remains unchanged.
The source explicitly describes these as intentionally fake/minimal DPBs; it
does not provide full disk geometry or implement a hardware controller.

## Required Neutral Transaction Shape

A future profile-local projection may derive the ordered included-drive list
from M19's immutable namespace record. Before it resumes guest execution it
must prepare one bounded transaction containing:

- the original `ES:BP` destination captured from stopped guest state;
- the complete byte range for every emitted packed DPB;
- the final `BP` patch; and
- no mutation for an empty list.

The generic execution boundary must validate the complete destination as
ordinary writable guest RAM before the first byte is written, then commit all
DPB bytes and the BP patch atomically. It must reject provider/ROM mappings,
range overflow, offset wrap, and any attempt to follow a host pointer.

This is a generic checked-memory plus constrained-register-patch requirement.
Core still does not need to know `DPB`, drive letters, service `0x46`, or DOS
drive classes; those remain profile/mantle semantics.

## Consequence

M19's namespace record needs only the subset required here: ordered local
removable/fixed membership and stable drive indices. Geometry, boot media, and
actual file/block operations remain separate, on-demand contracts. The next
NTDOS initialization dependencies determine whether they are needed for the
bounded command profile.
