# M0 T310 S22 P1 — host-storage source boundary

## Question

Which original host-storage contracts belong to the selected Win32/x86 CCPU40
SoftPC profile, and which must remain distinct rather than being replaced by a
new generic media provider?

## Inputs and procedure

Read the selected mirror forms of `nt_fdisk.c`, `nt_rflop.c`, `gfi_ibm.c` and
`gfi_sflp.c`; searched the selected MVDM mirror and adapter roots for each
reached `host_fdisk_*`, `nt_fdisk_*`, `nt_floppy_*` and `host_rpc_*` symbol;
then compared current mirror divergence markers with the component register.

## Observations

## Original source paths

S22 reaches four distinct original contracts.  They must not be collapsed into
a new generic disk provider:

- `softpc.new/host/src/nt_fdisk.c:107-177` supplies the original
  `host_fdisk_*` CCPU controller endpoint.  Its source-defined successful
  configuration geometry is paired with `host_fdisk_rd`/`host_fdisk_wt`
  returning zero: the selected controller has an exact **no fixed disk**
  outcome, not a missing implementation to replace.
- `softpc.new/host/src/nt_fdisk.c:279-780` supplies the separate DEM direct
  DASD package: `nt_fdisk_init/read/write/verify/close`, per-PDB ownership,
  lock/retry, heartbeats and aligned I/O.  It opens a real `\\\\.\\X:` device and
  uses original NT/Win32 file and disk-control calls.
- `softpc.new/host/src/nt_rflop.c` supplies the corresponding real-floppy
  package: `nt_floppy_*`, media change/format/check-verify, per-PDB ownership,
  idle closing and the original FDC worker handoff.  It opens the original
  real-drive device name and preserves FDC failure/status control flow.
- `softpc.new/base/disks/gfi_ibm.c` and `gfi_sflp.c` call `host_rpc_*` for the
  historical remote-floppy service.  No selected MVDM definition exists in
  the mirror or current adapter set; it is a distinct remote-service boundary,
  not local removable-media I/O.

## Existing mirror adaptations

The real-media bodies already retain only two registered, source-shaped
differences:

- `MVDM-HOST-DIV-006` replaces `pusCurrentPDB` with a numeric guest-location
  descriptor and a fresh two-byte session lease at each `nt_fdisk`/`nt_rflop`
  ownership check.  The value remains a DOS scalar; no host pointer crosses
  the interface.
- `MVDM-HOST-DIV-069` keeps the original aligned fixed-disk scratch-buffer
  lifetime but represents its private `malloc` backing as `PBYTE` and performs
  alignment in `uintptr_t`.  It is host-local allocation, not a mapped
  identity.

The selected local-file/device APIs (`CreateFile`, `ReadFile`, `WriteFile`,
`DeviceIoControl`, close and error paths) are public Win32 forms.  The older
`NtOpenFile`/`NtFsControlFile` discovery sequence remains in the original body
and must be assessed through the existing OpenNT-shaped Win32 declaration
surface before any source change.  No caller currently justifies changing the
source-defined `host_fdisk_*` no-disk controller outcome.

## P1 disposition and next work

| ID | Original endpoint family | Current disposition | Next owner work |
| --- | --- | --- | --- |
| S22-001 | `host_fdisk_*` controller callbacks | direct original no-media outcome | retain and prove exact failure direction; do not route to `nt_fdisk_*` |
| S22-002 | `nt_fdisk_*` direct DASD | original body with existing mapping-manager binding | audit native NT/Win32 declaration/handle/order contract and select a safe negative proof |
| S22-003 | `nt_rflop_*` local removable media | original body with existing mapping-manager binding | audit worker, device-open, format/media-check and source failure paths |
| S22-004 | remote `host_rpc_*` | unresolved historical remote-service edge | establish exact unavailable disposition; no invented remote protocol |

This P1 records source boundaries only.  It does not activate a real host
drive, fabricate media, enable remote floppy transport, or claim guest media
execution.

## Interpretation, confidence and follow-up

Confidence is high for the four-way owner split because the endpoint bodies
and their call forms are present in the selected original sources. The next
packet part verifies the reached NT/Win32 declaration surface and exercises
only source-shaped negative outcomes before any real-device operation is
considered.
