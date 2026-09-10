# M0 T96 S3 DEMDASD Source Contract 001

## Question

Can either original absolute-disk BOP service be safely treated as a
read-only namespace operation, or does `demdasd.c` require a distinct raw
block-media composition?

## Inputs and procedure

The primary input is pinned OpenNT
`refs/opennt/base/mvdm/dos/dem/demdasd.c`, SHA-256
`7438F5D4DF1803664E66F16B7464D719C8E4341D1D9C2E7721CE7184BDF08942`, with
the adjacent `demdasd.h` structure/API declaration and guest DOS
`v86/doskrnl/dos/mscode.asm` BOP call sites. The immutable DEM map identifies
service 41 `SVC_DEMABSDRD` as `demAbsRead` and service 42 `SVC_DEMABSDWRT` as
`demAbsWrite`.

The review read both public handlers, their shared `demAbsReadWrite` path,
the BDS initialization/read/write/BPB/format/verify helpers, and the guest
DOS callers. It classified the dependencies by their actual owner rather than
by whether a branch happens to be named "read".

## Original contract

| Service | Entry and result | Whole-component dependencies | CLI disposition |
| ---: | --- | --- | --- |
| 41 `DEMABSDRD` | AL drive; either CX/DX plus DS:BX buffer or a guest `DISKIO` structure. Success clears CF; failure sets CF and maps host error into AX. | BDS, checked-but-historical `GetVDMAddr`, physical sector geometry, media-change/BPB state, raw host floppy/fixed-disk read, and potentially an INT 13h hook path. | Deferred as raw block media. |
| 42 `DEMABSDWRT` | Identical input shape; writes sectors and has the same AX/CF error contract. | The same BDS, buffer, geometry, INT 13h, and host backend graph, plus host block-media mutation. | Explicitly unavailable in the read-only profile; no adapter success or silent write discard. |

The apparent read/write split is not a composition boundary. `demAbsReadWrite`
owns both; `demDasdRead` and `demDasdWrite` each depend on `PBDS` state and
may refresh removable-media BPB state. Fixed media call `nt_fdisk_read` or
`nt_fdisk_write`; floppy paths inspect IVT 13h using `sas_loadw` and, if a
guest handler has replaced the vector, save registers, set an INT 13h caller
address, and call `host_simulate`. Initialization itself probes BIOS floppy
services and enumerates fixed disks through `nt_fdisk_init`.

Consequently the component's missing inputs are not an adapter address/length
copy alone: they include a raw-media ownership policy, BDS lifecycle, sector
geometry, media-change/error translation, Bochs/guest INT 13h interaction,
and mutable host device backend. None is part of the declared CLI drive
projection or of the adapter's fixed-width BOP ABI.

## Decision

`demdasd.c` is a single deferred **raw block-media provider family**, shared
with the AL=0D/11 branch of `demioctl.c`. It is neither admitted as a source
provider nor rehosted as a namespace shortcut. This preserves the original
read/write coupling and avoids importing SoftPC `host_simulate`, SAS/IVT, BDS,
or physical host devices into adapter.

If a future profile explicitly admits virtual block media, it must first
declare a Bochs-owned machine device plus a separate fixed-width block
capability ABI and prove the matching 41/42 and 0D/11 failure/geometry
semantics together. It must not reuse host drive projection as a raw disk.

Confidence is high: both guest BOP call sites and all decisive dependencies
are direct source observations. No runtime behavior or historical source file
was modified.
