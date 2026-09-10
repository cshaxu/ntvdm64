# M0 T236 S4 P1 — `nt_fdisk` Source-Recovery Evidence

## Question

Can the composable fixed-volume half of the imported OpenNT DEM DASD/IOCTL
path recover the original `nt_fdisk.c` lifecycle without giving `bx-vdm`
ownership of floppy, INT 13h, FDC, DMA or CMOS behavior?

## Source basis

- `src/opennt/base/mvdm/softpc.new/host/src/nt_fdisk.c`
- `src/opennt/base/mvdm/dos/dem/demdasd.c`
- `src/opennt/base/mvdm/dos/dem/demioctl.c`

## Procedure

Compared the prior shim against the reached fixed-disk lifecycle in
`nt_fdisk.c`, restored the original table and handle-lifecycle shape, then
rebuilt the formal module graph and ran the focused fixture pair.

## Recovered structure

`src/bx-vdm/bop/shim/demdasd_ioctl_shim.c` now follows the reached
`nt_fdisk.c` structure rather than the former one-shot 26-slot volume table:

- an OpenNT-shaped physical-drive table (`drive`, logical letter, geometry,
  alignment requirement, owner PDB, auto-lock state and native handle);
- `nt_fdisk_init` table growth and physical-index assignment;
- `get_fdisk_data`, `get_fdisk_handle`, `close_fdisk`, read/write/verify,
  `nt_fdisk_close` and `HostFdiskReset` lifecycle; and
- original PDB-sensitive reuse versus close/reopen ordering and `pFDAccess`
  accounting.

The raw-volume entrance follows the original owner rule. `demFdiskInit`
iterates all letters and `nt_fdisk_init` attempts every host `DRIVE_FIXED`
letter.  There is no project-specific CLI include/exclude filter.

## Explicit divergences

1. OpenNT uses private NT4 `NtOpenFile`, `NtDeviceIoControlFile`,
   `NtQueryInformationFile` and FAT-only `FSCTL_QUERY_FAT_BPB`.  The shim uses
   documented Win32 volume handles and geometry/free-space queries.  It forms
   the existing `BPB` contract from those public results so a modern fixed
   volume need not be FAT.
2. The pinned NT4/Win2K public header surface cannot name the newer public
   `FileAlignmentInfo` structure.  Public buffered handles therefore retain
   the original zero-alignment branch; no private NT declaration is added.
3. The original floppy/recursive `host_simulate` route remains unchanged in
   imported `demdasd.c`, but its machine implementation is unavailable here.
   It returns an explicit error and transfers to the bx machine/BIOS owner;
   no floppy, INT 13h, FDC, DMA or CMOS behavior is implemented in `bx-vdm`.

## Verification

Generated formal MSVC x64 `/MT` Ninja graph:

```powershell
ninja -C O:\repos.hobby\ntdos64\build\M0-T236-S3\001 `
  bin/t230-s7-demdasd-ioctl-direct-import-fixture.exe `
  bin/t230-s15-direct-host-session-fixture.exe
```

Both targets linked successfully.  The DASD/IOCTL fixture printed:

```text
T230 S7 direct OpenNT DASD/IOCTL import: IOCTL query and raw-drive failure contracts verified
```

The Direct-host-session fixture exited zero. No fixture opens, reads, writes
or formats a real volume.

## Interpretation

The source-shaped fixed-volume composition now has its original host-side
lifecycle structure and does not add a product drive filter.  This proves
compile/link and bounded local contracts, not successful raw I/O against a
particular host volume.

## Disposition

This is a P1 source-recovery increment for T236/S4, not S4 closure.  It does
not claim a real-volume transfer, floppy support, BIOS disk execution, or
guest continuous execution.  Those require their respective admission and
machine-owner evidence.
