# DOS File Open/Exec Containment Ledger

Status: M25 source-derived file-path and DEM seam record, 2026-08-08.

## Result

The bounded `COMMAND.COM /C <owned DOS command>` route cannot bypass the
historical DOS file/process stack. Sysinit, COMMAND.COM, and NTDOS retain
distinct responsibilities:

```text
profile namespace record
  -> NTDOS pathname/SFT/PSP/arena logic
  -> historical DEM open/read/seek/close service boundary
  -> modern contained host-file adapter
```

The host adapter supplies file-object behavior. It must not reimplement DOS
path parsing, handle-table management, PSP ownership, COM/EXE loading, or
process return semantics.

## First-Cut Operations

The source establishes these operations before a direct known-path command can
run:

| Operation | NTDOS/guest source | Historical DEM service | First-cut need |
| --- | --- | --- | --- |
| open | `dos/file.asm:197-218` | `SVC_DEMOPEN` (`0x12`) | required |
| read | `dos/fcbio2.asm:343-352`, `msproc.asm:1397-1405` | `SVC_DEMREAD` (`0x16`) | required |
| seek | sysinit uses `INT 21h` `4202h` / `4200h` at `sysinit1.asm:1581-1608`; DEM implements file-pointer service | `SVC_DEMCHGFILEPTR` (`0x00`) | required |
| close | sysinit and COMMAND.COM close probe/loaded files | `SVC_DEMCLOSE` (`0x02`) | required |
| execute | `dos/msproc.asm:$Exec` | consumes the above DOS file operations | required, DOS-owned |

`sysinit1.asm:1517-1620` proves that `COMMAND.COM` is opened, size-checked,
then launched by DOS `EXEC`; it is not loaded by a private bootstrap reader.
`msproc.asm:283-520` proves that execution validates the request, records the
guest filename and parameter block, creates/copies an environment, allocates
DOS memory, and performs DOS-owned loading. `msproc.asm:1397-1437` shows that
the loader reads through DOS file services while temporarily assigning system
ownership to allocated regions.

## Relevant Historical DEM ABIs

The original boundaries are explicit and must remain observable:

- `demOpen` receives a full guest path in `DS:SI`, open mode in `BL`, EA data
  information in `ES:DI`/`AL`, and returns carry plus a 32-bit file handle,
  DOS time/date, and file size. See
  `src/opennt/base/mvdm/dos/dem/demfile.c:117-146`.
- `demRead` receives its handle in `AX:BP`, byte count in `CX`, destination at
  `DS:DX`, current position in `BX:SI`, and a zero-flag seek condition. It
  returns carry plus read count/error. See
  `src/opennt/base/mvdm/dos/dem/demhndl.c:93-113`.
- `demClose` receives the same split handle and a possible file-position
  update in `CX:DX`; it reports carry plus error. See `demhndl.c:28-90`.
- `demChgFilePtr` receives split handle, `CX:DX` target, and `BL` origin,
  returning new `DX:AX` or a carry/error. See `demhndl.c:342-363`.

The original NTDOS file implementation stores the returned 32-bit value in
the SFT (`file.asm:212-218`). Consequently a 64-bit native host handle cannot
be copied into the historical ABI. This is a concrete modern-host compatibility
problem, not a reason to alter NTDOS or put DOS handles into core.

## Contained Namespace Contract

The first profile needs only a deliberately small namespace:

1. One declared mounted DOS drive and an immutable starting directory.
2. A canonical DOS path resolver for absolute and profile-relative paths,
   case-insensitive fixed-EN-US matching, `.`/`..` handling, and rejection of
   namespace escape, UNC, device, volume, and host-root paths.
3. A manifest-backed immutable file table containing NTIO/NTDOS placement
   roles, `COMMAND.COM`, the profile's synthetic CONFIG/AUTOEXEC entries, and
   the owned command plus declared data files.
4. Read-only open/read/seek/close behavior with stable file size and
   DOS-visible file-not-found/path-not-found/access-denied outcomes.
5. No create, write, delete, rename, directory create/remove, lock, pipe,
   physical-drive, redirector, or host directory discovery behavior.

The first direct known-path command does not require directory enumeration.
`SVC_DEMFINDFIRST/NEXT`, FCB operations, and write/create/delete/rename are
separate on-demand services, even though the wider historical dispatcher
contains them. 8.3 alias generation is likewise deferred until a selected
profile names a file whose canonical manifest name cannot itself satisfy the
DOS 8.3 form.

## Modern Host Shim Boundary

The x64 shim needs an owned per-session **opaque 32-bit DEM file token table**
behind the historical split-register values. Each token maps to a contained
read-only host object plus cursor and lifetime state. It must reject stale,
cross-session, forged, or out-of-range tokens. It must not expose a raw Win32
`HANDLE` to NTDOS, and must keep allocation/close ownership inside the shim.

This is an ABI-preserving adaptation of the historical `demOpen`, `demRead`,
`demChgFilePtr`, and `demClose` host dependencies. It is not a new DOS SVC
set: the original DEM dispatcher, selector ordering, and NTDOS SFT flow remain
the normal path. The exact integration location and buildability of these
historical functions is the next task.

## Deferred Semantics

- writes, sharing/locking, critical-error retry, attributes, timestamp
  mutation, file creation/deletion/rename, FCB I/O, and redirected pipes;
- directory find-first/find-next and generalized 8.3 alias enumeration;
- network redirector, physical media, block controller, FDC/HDC, and DOS
  device paths;
- `SVC_CMDCHECKBINARY`/`SVC_CMDEXEC` native process classification and host
  transfer.

## Boundary Result

M25 changes the implementation order: a useful first runtime cannot be formed
from only entry state plus a generic memory provider. It must restore the
original DEM file service path and put exactly its legacy Win32-handle
assumption behind a modern, contained, tokenized shim. This remains outside
core and preserves the future mantle/core split.
