# M0 T280 S10 — reached `ntioapi` ABI closure

S10 recovers the complete **reached** `ntioapi.h` surface for the three
original DEM consumers as one package. It does not include the full historical
header into modern `winternl.h`: that direct form was attempted in S9 and
proved to expose unrelated private NT I/O declarations and duplicate layouts.

## Required closure inventory

The source ledger must cover, at minimum, all reached declarations and calls
in `demsrch.c`, `demerror.c` and `demgset.c`:

- `IO_STATUS_BLOCK`, `OBJECT_ATTRIBUTES`, `OBJECT_NAME_INFORMATION`,
  `FILE_BOTH_DIR_INFORMATION`, `FILE_FS_DEVICE_INFORMATION` and the reached
  information-class/flag/status constants;
- `NtOpenFile`, `NtQueryDirectoryFile`, `NtQueryVolumeInformationFile`,
  `NtQueryObject`, `NtOpenSymbolicLinkObject`, `NtQuerySymbolicLinkObject`,
  and `NtClose`;
- x86/x64 layout requirements and lifetime rules for returned host data;
- each modern public or `ntdll` binding option, original failure rule and
  the only legal session/mapping boundary.

## Boundary

The result may introduce a same-named, reached-subset adapter header and
facade implementation only if the ledger proves every declaration and call
shape. It must preserve original source call order and explicit failure codes;
it cannot use raw guest pointers, global process state or a second mapper.
All temporary NT/Win32 records remain host-local. A `GetVDMAddr` span is a
separate `adapter-softpc` lease and must not be smuggled through this ABI.

## Exit criteria

A single source/ABI/failure ledger covers every reached `ntioapi` declaration
and call, identifies direct modern binding versus same-shaped facade versus
explicit unavailable behavior, and records one complete minimal implementation
plan. No DEM body becomes enabled merely by the inventory.
