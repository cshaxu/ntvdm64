# T420 S28 disks capability closure

## Current profile and source authority

S28 closes the ten selected original bodies in
`src/mvdm/softpc.new/base/disks`: `floppy.c`, `floppy_i.c`, `diskbios.c`,
`fdisk.c`, `fla.c`, `gfi.c`, `gfi_ibm.c`, `gfi_mpty.c`, `gfi_sflp.c` and
`gfi_test.c`.  Each has the same SHA-256 as its pinned OpenNT counterpart;
there is no disk overlay, adapter-owned disk algorithm, virtual image, or
new guest medium.

The product intentionally uses the original no-attached-media host profile.
The selected OpenNT `config.c` supplies `"?"` for hard disk 1 solely so the
disk BIOS can initialize; its own comment states that DOS emulation should
not subsequently issue disk BIOS calls.  The original `nt_fdisk.c` makes
`host_fdisk_rd` and `host_fdisk_wt` return failure.  This is an original
profile boundary, not a missing image implementation.

## Complete capability disposition

| Family / selected body | Current-profile disposition | Evidence |
| --- | --- | --- |
| Original startup, controller publication and reset: `diskbios.c`, `fdisk.c`, `fla.c`, `floppy_i.c` | Real pass | Original `reset.c` calls `hda_init()` and `disk_post()`; the real guest witness below calls the published INT 13h reset and returns to COMMAND. |
| No-media floppy selection: `gfi.c`, `gfi_mpty.c`, `floppy.c` | Real selected profile | Original `main.c` calls `gfi_init()` and current original `config.c` reports zero active floppies, selecting the original empty-provider function table. |
| IBM/slave/test floppy providers: `gfi_ibm.c`, `gfi_sflp.c`, `gfi_test.c` | Source-proven profile exclusion | The current original configuration selects neither an IBM/slave drive nor the test provider; no current caller activates their provider table. |
| Hard-disk transfer: `diskbios.c::disk_read/write`, `fdisk.c` | Original no-media profile exclusion | The unchanged OpenNT configuration and `host_fdisk_rd/wt` explicitly exclude a backing store.  A diagnostic AH=02 probe was deliberately not promoted into acceptance: it violates that original post-initialization precondition.  S28 adds neither a disk image nor a replacement error policy. |
| Worker termination: `terminat.c -> host_fdisk_term` | Real pass | The complete transcript-gated COMMAND matrix leaves no package-owned worker after every normal and failure route. |

`nt_vflop.c` is historical original source outside the selected manifest.  It
is not a justification to introduce a virtual floppy feature into this
profile.

## Real guest witness

`tests/observation/disks_capability.asm`, built by
`tools/build/Build-T420S28DisksGuestTest.ps1`, is a test-only COM program.
It invokes original INT 13h AH=00 for drive 80h and requires a carry-clear
return before exiting through DOS.  It installs no handler and does not
create, attach, read or mutate any medium.

The formal-package deployment transcript at
`O:\winnt\logs\m0-t420-s28-reset-r3.raw` contains:

```text
S28_STEP_RESET S28_INT13_RESET_OK S28_DISKS_OK
```

The prompt after the marker proves return to the same interactive COMMAND
session.  The observer rejects a missing marker or a guest command-resolution
failure.

## Formal build and regression

Fresh graph `build/M0-T420/S28/formal-x86-r1` was generated from current
source and its serial formal execution built all five products: `run16.exe`,
`basesrv.exe`, `ntvdm.exe`, `dtmgr.exe` and `VDMREDIR.dll`.  The deployed
artifact hashes are recorded in
`O:\winnt\logs\m0-t420-s28-formal-artifacts.json`.

The fresh deployed `m0-t420-s28-command-r2` matrix passed all 17 established
transcript-gated routes: empty and native COMMAND, expected missing-command
recovery, native stdout/stderr and EOF, direct/interactive/nested/repeated
MEM, direct and nested COMMAND `/c`, direct guest exit 7, and EDIT return
followed by MEM.  The machine-readable summary is
`O:\winnt\logs\m0-t420-s28-command-r2-summary.json`.

S28 therefore closes the complete selected disks unit as real current-profile
initialization/reset/termination behavior plus source-proven no-media provider
exclusions.  It makes no product-code change and adds no virtual-disk
capability.
