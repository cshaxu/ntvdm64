# M0 T236 S4 P3 — Authorized VHDX Raw-Volume Success Evidence

## Question

Can the recovered fixed-volume half of the OpenNT DEM DASD path complete a
real public-Win32 raw read, verify, write, exact restore and close lifecycle
when given a deliberately disposable fixed volume?

## Inputs

- `src/opennt/base/mvdm/softpc.new/host/src/nt_fdisk.c`
- `src/opennt/base/mvdm/dos/dem/demdasd.c`
- `src/opennt/base/mvdm/dos/dem/demioctl.c`
- User-designated, mounted `O:\assets\ntvdm64-test.vhdx`, drive `T:`

## Procedure

1. Confirmed through elevated read-only Windows storage queries that `T:` is
   the mounted, online, writable, FAT32, 256 MiB file-backed virtual disk at
   the supplied VHDX path.
2. Replaced the former C:-specific, success-on-denial fixture with one that
   requires an explicit environment-selected test letter.  It operates only
   at offset 8 MiB: read, `IOCTL_DISK_VERIFY`, write a two-byte reversible
   perturbation, read it back, restore the exact saved 512 bytes, read it
   again, then close.
3. Generated a fresh formal MSVC x64 `/MT` Ninja graph under
   `build/M0-T236-S4/p3-vhdx-raw-r1/`, built the fixture and executed it with
   `NTDOS64_T236_S4_TEST_DRIVE=T` under the authorized elevated token.
4. Rebuilt and ran the existing imported `demdasd`/`demioctl` focused
   regression in the same graph.

## Observations

```text
T236 S4 nt_fdisk: T: init/read/verify/write/restore/close verified
T230 S7 direct OpenNT DASD/IOCTL import: IOCTL query and raw-drive failure contracts verified
```

The test did not address the boot sector, FAT, root directory or a host disk.
Its one modified sector was restored byte-for-byte before close.

## Interpretation

The `nt_fdisk` compatibility seam now has a positive, public-API evidence
path for the fixed-volume lifecycle on a disposable medium.  This proves the
backend used by the imported `50:29`/`50:2A` source can read, verify, write,
restore and close a host volume without a private NT protocol or Bochs device
semantic.

It does not itself prove a guest-native BOP transaction: the imported
`demdasd.c` BDS/INT-25/26 entry and `demioctl.c` request-layout routes retain
their independently tested direct dispatch/checked-span boundaries.  Nor does
it admit destructive `50:21` format or boot-sector `SETMEDIA` testing.

## Follow-up

- Keep `50:21` destructive-format/media operations explicitly outside this
  fixture; any such test needs a separately named operation and restoration
  procedure.
- Floppy, hooked INT 13, `host_simulate`, FDC/DMA/CMOS and VDD-specific IOCTL
  branches remain machine/device-owner work, not bx-vdm behavior.
- The S4 closure review must reconcile the positive VHDX backend result with
  each imported `50:21`, `50:29` and `50:2A` dispatcher disposition.
