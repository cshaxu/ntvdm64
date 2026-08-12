# SH-05 Contained Directory Search Contract

Status: research contract, not an admitted runtime implementation. 2026-08-06.

## Scope

This contract covers the narrow host seam beneath OpenNT's original
`base/mvdm/dos/dem/demsrch.c` search implementation. It does not replace
`demFindFirst`, `demFindNext`, `demFindFirstFCB`, `demFindNextFCB`,
`SearchFile`, `CopyDirInfoToDosData`, or any DTA/FCB/PSP/find-list ownership.

The required historical flow remains:

```text
DEM SVC body -> SearchFile -> FileFindOpen/Next/Reset/Close
             -> contained directory cursor provider
             -> configured host-directory root
```

`demsrch.c` currently keeps `FFINDLIST`, `FFINDDOSDATA`, DTA/FCB conversion,
8.3 selection, wildcard filtering, result ordering, `FFindId`, timeout
cleanup, and DOS error conversion. Those remain historical responsibilities.

## Source Facts

`FileFindOpen` stores a directory handle, an NT-style `FILE_BOTH_DIR_INFORMATION`
buffer, `PathName`, `FileName`, and cursor state in `FFINDLIST`. `FileFindNext`
continues consuming that buffer and calls the original `CopyDirInfoToDosData`.
The latter decides attribute filtering, short-name fallback, DOS result name,
file date/time, size, and persistent `(FileIndex, FileName)` reset state.

The historical source reaches the host through `RtlDosPathNameToNtPathName_U`,
`NtOpenFile`, `NtQueryDirectoryFile`, `NtClose`, and in the fast reset path
`NtVdmControl(VdmQueryDir)`. The last call is unavailable by policy: the
research runner must not use V86/VDM host APIs or rely on the removed NTVDM
kernel protocol. Calling the remaining native functions directly is also not
admissible because an uncontained DOS path could open or enumerate arbitrary
host directories.

## Provider Boundary

The future default-disabled provider is a versioned C ABI inside the OpenNT
compiler/CRT island. It is configured with explicit per-session DOS drive
roots before any search is opened. It has no process-global current directory
and never infers a host path from the running executable or an environment
variable.

Conceptual operations are:

```text
open(dos_path_and_pattern, buffer_capacity) -> cursor or NTSTATUS
query(cursor, file_both_directory_buffer, capacity, restart_scan) -> NTSTATUS
reset_after(cursor, last_file_index, last_file_name) -> NTSTATUS
close(cursor)
```

The returned records must use the fixed `FILE_BOTH_DIR_INFORMATION` wire
layout expected by the original `CopyDirInfoToDosData`; the provider does not
produce a DOS DTA, FCB result, 8.3 alias, or SVC result. A provider may retain
a private cursor object behind the historical `DirectoryHandle` field only
when its lifetime, close, and invalid-handle behavior are explicit and
bounded. It must not cast a host handle to a guest-visible DOS handle.

`reset_after` is required even for a first implementation. It must reproduce
the historical observable contract: after the remembered `(FileIndex,
FileName)`, the next query resumes after that same entry; absent/deleted
remembered entries yield the corresponding no-more-files/error result rather
than silently restarting from the root. The provider must not call
`NtVdmControl`, and it may not depend on native directory-file indices being
stable across unrelated host mutation.

## Containment And Failure Rules

- All path admission first resolves an explicit DOS drive mapping, rejects an
  unconfigured drive, host-absolute/UNC/device paths, `..` escape, and reparse
  traversal before opening the directory.
- Enumeration is limited to the admitted root subtree. No drive discovery,
  volume enumeration, host root fallback, or device search is permitted.
- DOS device-name handling remains in the original `FileFindFirstDevice`
  branch. The provider must not make a host device available merely because a
  name resembles one.
- The provider returns mapped `NTSTATUS` values only. `demsrch.c` continues to
  decide `SetLastError(RtlNtStatusToDosError(status))`, carry, DTA/FCB writes,
  and cleanup.
- It must allocate and free only on its own side of the compiler/CRT ABI.
  No C++ object, CRT allocation, or exception crosses into the MinGW CLI.
- Requested buffer capacity is bounded and must be validated before record
  construction. A malformed record chain, record larger than capacity, or an
  unmappable name fails atomically without advancing the cursor.

## Required Overlay Discipline

The historical `FileFindOpen`, `FileFindNext`, `FileFindReset`, and
`FileFindClose` are the only eligible overlay points. An overlay may replace
their direct native-call leaves with the provider operations, while preserving
the original functions' signatures, `FFINDLIST` data flow, status handling,
and calls to `CopyDirInfoToDosData`.

It may not replace `SearchFile` or any SVC handler, introduce a separate
FindFirst/FindNext dispatcher, or change DOS search state. The overlay must
be maintained as an explicitly manifested local input, separate from the
read-only upstream `demsrch.c` source.

## Admission Evidence

Before SH-05 is eligible for a historical runtime link, a default-disabled
fixture must compile the original/overlay `demsrch.c` and prove all of:

1. A configured `C:` root produces two sequential original DTA results
   through `demFindFirst` and `demFindNext`.
2. A resumed search preserves `FFindId`, does not restart from the first
   entry, and handles a removed remembered entry deterministically.
3. An unconfigured drive, `C:\..\escape`, UNC/device path, and reparse
   traversal reach DEM's existing failure path without a host open.
4. DTA and FCB tests preserve original attribute filtering and 8.3 fallback.
5. Static import audit proves the target has no `NtVdmControl`, V86 monitor,
   direct `NtOpenFile`, direct `NtQueryDirectoryFile`, or direct host-path
   admission outside the contained provider.
6. The default modern build neither compiles nor links the provider, overlay,
   fixture, or historical objects.

This is intentionally a larger proof than the SH-11 directory fixture. The
OEM facade does not cover directory enumeration or the state that makes DOS
FindFirst/FindNext compatible.
