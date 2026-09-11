# Positional NE-to-WOW entry recovery

## Candidate

Recover the OpenNT `BINARY_TYPE_WIN16 → BaseSrvCheckWOW` selection at the
current standalone app/BaseVDM boundary while keeping the public contract
`ntvdm32.exe <Win16-NE path>`.

## Source basis and boundary

Original `BaseSrvCheckVDM` selects `BaseSrvCheckWOW` for Win16. Original
`GetWowKernelCmdLine` obtains its KRNL386 bootstrap target from the actual
worker command line. The selected local BaseVDM facade already represents DOS
and WOW owners, but its broker serialization was DOS-only.

The candidate permits a fixed-width copied WOW owner, derives an internal
source-shaped `-w -a <package KRNL386>` argv for a classified NE launch, and
tests DOS non-regression. It does not introduce a new WOW loader, guest
modification, CPU change, shared-WOW pool or cross-process broker.

## Evidence and disposition

[P15 entry-recovery evidence](../etc/evidence/m0-t404-s3-p15-positional-ne-wow-entry-recovery-001.md)
records the source-first ledger and results. It proves the record-selection
recovery but also proves that an actual worker command-line/process bridge is
a further material boundary: a synthetic in-memory SoftPC argv cannot satisfy
the original `GetCommandLine()` consumer. That follow-up requires separate
admission.
