# M0 T280 S10 — reached `ntioapi` ABI evidence

The exact OpenNT carrier remains byte-identical in
`opennt-platform-abi/source/opennt/public/sdk/inc/ntioapi.h`.  The task is not
to edit or partially mirror it; it is to establish the smallest compatible
surface that lets the three original DEM bodies retain their call order,
layouts and NTSTATUS branches.

## Direct-source findings

- `ntioapi.h:459`, `591`, and `1009` define the three reached record groups:
  `IO_STATUS_BLOCK`, `FILE_BOTH_DIR_INFORMATION`, and
  `FILE_FS_DEVICE_INFORMATION`.
- The original function declarations begin at `NtOpenFile` line 1209,
  `NtQueryDirectoryFile` line 1229 and `NtQueryVolumeInformationFile` line
  1295. `demerror.c` additionally reaches the object and symbolic-link paths.
- The current Windows `winternl.h` exposes `OBJECT_ATTRIBUTES`,
  `IO_STATUS_BLOCK`, `NtClose`, `NtOpenFile`, and `NtQueryObject`, but not a
  complete compatible declaration/layout set for the reached directory,
  volume and symbolic-link operations.
- Direct inclusion of full historical `ntioapi.h` after current `winternl.h`
  was attempted in S9. It failed on missing historic dependencies and on
  duplicate modern layouts; it is therefore not an admissible source form.

## Recovery decision

The first usable rung is a single `adapter-win32` reached-subset facade—not
eleven service-specific helpers. It will retain source spellings, parameter
order, data layouts and NTSTATUS failures. Where a same-shaped `ntdll` export
is safely available, the facade may bind it. Where it is not, it returns an
explicit source-shaped unavailable result until a separately audited public
Win32 backend can preserve the reached contract.

No record, handle, or temporary pointer in this ABI is a mapping-manager
identity. The only DEM values that use `session.guest_memory` are separately
identified `GetVDMAddr`/`Sim32*` guest spans behind `adapter-softpc`.
