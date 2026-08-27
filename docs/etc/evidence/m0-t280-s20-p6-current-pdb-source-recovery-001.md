# M0 T280 S20 P6 — current-PDB source recovery

## Recovered source surface

The reached original `pusCurrentPDB` pointer has been removed.  `DS:DX` now
installs numeric `current_pdb_location`; every consumer asks the shared
`adapter-softpc` boundary for one fresh two-byte guest lease and receives a
plain `USHORT` PDB segment.

The conversion includes every reached original use identified by P1:

* DEM find-list heartbeat and PSP record creation;
* DEM optional-current-PDB paths in `VDDAllocateDosHandle`,
  `VDDReleaseDosHandle` and `VDDRetrieveNtHandle`;
* SoftPC `nt_fdisk` read/write/verify ownership; and
* SoftPC `nt_rflop` read/write/format/verify, delayed FDC owner capture and
  change-line refresh.

No value is mapped to an opaque identity: the PDB stays a numeric DOS segment
and is converted to the original `segment << 16` form only after its fresh
two-byte lease has been released.

## Failure disposition

Where the historical source returned a status, inability to acquire the PDB
retains that existing failure direction: `0`, `FALSE`, `FAILURE`, or the
negative `ERROR_INVALID_ADDRESS` return used by `VDDAllocateDosHandle`.
The FDC delayed-worker path does not create a worker with an invented owner
PDB when the read fails.

## Build evidence and transferred interface closure

The formal x64 PDB-consumer Ninja graph confirms `demfile.c` parses with the
new boundary.  Its two original SoftPC bodies proceed past the converted PDB
uses but remain blocked by independently missing historical product-shell
interfaces:

* `nt_rflop.c` reaches `cpu4.h` and requires the selected historical
  `CpuInt_c.h` CCPU profile; this is a SoftPC/CCPU product-shell selection,
  not a PDB or guest lease API;
* `nt_fdisk.c` reaches original NT I/O declarations such as
  `FSCTL_QUERY_FAT_BPB_BUFFER`, `FILE_ALIGNMENT_INFORMATION`,
  `NtQueryInformationFile` and `NtFsControlFile`, which are not yet in the
  current same-shaped `adapter-win32/ntioapi.h` subset.

These are tracked interface requirements for the corresponding adapter/host
package; this record does not represent a full SoftPC build closure.  The
PDB source conversion itself has no remaining `pusCurrentPDB` reference.
