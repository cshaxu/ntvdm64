# M0 T236 S4 P2 — Fixed-Volume Lifecycle and Transport Evidence

## Question

After restoring the OpenNT-shaped `nt_fdisk` table, do fixed-disk DEM calls
retain their required host lifecycle and checked guest-memory transport without
claiming the floppy/INT 13 machine path?

## Inputs

- `src/opennt/base/mvdm/softpc.new/host/src/nt_fdisk.c`
- `src/opennt/base/mvdm/dos/dem/demdasd.c`
- `src/opennt/base/mvdm/dos/dem/demioctl.c`
- `src/opennt/base/mvdm/dos/dem/demmisc.c`

## Procedure

1. Restored `fdisk_heart_beat`, `FdiskTerminatePDB` and `HostFdiskReset` in
   the DASD shim using the original table/owner-PDB/idle-close order.
2. Reconnected imported `demDiskReset` to `HostFdiskReset`; its floppy reset
   remains separately owned by the machine package.
3. Extended the scoped CCPU/SAS replacement so an absolute-disk or track
   request can carry its original packed request block plus one sector payload
   through two bounded, checked guest-RAM spans.
4. Built a non-mutating `nt_fdisk` fixture that attempts only fixed-volume
   initialization, sector-zero read and close; it never writes, formats or
   locks a volume.

## Observations

The formal Ninja target set linked and ran:

```text
T230 S9 direct OpenNT demmisc import: all lifecycle, loader and diagnostic service bodies verified
T236 S4 nt_fdisk readonly: host raw-read unavailable error=5
```

`ERROR_ACCESS_DENIED` occurred on the host's `\\.\C:` raw read after
initialization.  This is the expected public Win32 privilege boundary for the
current token.  The fixture closed its handle and returned success after
recording that source-visible unavailable outcome.

## Interpretation

The fixed-volume code path now has a source-shaped table lifecycle, reset/PDB
cleanup and a correct two-span guest transport.  The local test proves that
raw access denial is propagated rather than converted into a fake sector
transfer.  It does **not** prove a successful raw read, write, formatting or
lock operation under an elevated or otherwise authorized host token.

## Follow-up

- The `50:21` fixed-disk generic/query and `50:29/2A` code paths may use this
  lifecycle and transport.
- A future privilege-authorized test environment may add success-path read
  evidence; it must remain read-only unless a separate destructive-media test
  admission exists.
- Floppy, hooked INT 13, `host_simulate`, FDC/DMA/CMOS and device-specific VDD
  branches remain with their respective machine/device owners.
