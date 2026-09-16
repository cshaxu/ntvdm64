# T416 S5 — integrated footprint-cleanup verification

## Result

S5 verifies S1--S4 on a fresh MSVC Win32/x86 `/MT`, CCPU40 three-program
graph.  Its 478 explicit product compile/link commands end in `SUCCESS
478/478`, including the `VDM_TIB` map gate.  The formal products and deployed
`O:\winnt` files match byte-for-byte.

| Product | SHA-256 |
| --- | --- |
| `run16.exe` | `8C56D805E7108DCE5E7801003B99D843F07BC1276A4CCD64A85E667DD0E7E229` |
| `basesrv.exe` | `4B1A4E1E557534588C073C6C7E2CDB1A59DE8042C275B213A4DAEA2433ABCAE5` |
| `ntvdm.exe` | `57C3CBE754229F382EE2AA7FD9EECE8497904999D58291614BE4A05519CFB3A6` |

S5 changes no product source.  Its two test-tool corrections only follow
closed facts: the CCPU fixture now references canonical `src/mvdm` after
T414/T415 removed `src/mvdm-host`, and the COMMAND matrix no longer requires
the S4-retired COMMAND continuation recorder.  Broker/native-child reports
remain live evidence.

## Verification

- The fresh T416 Console observer passes 15 real cases: empty COMMAND,
  native zero/missing/seven/streams/EOF, MEM, nested COMMAND, three-depth
  nested MEM, repeated/direct MEM, `COMMAND /c`, direct native seven, and
  EDIT return.  See `O:\winnt\logs\m0-t416-s5-command2-summary.json`.
- Six ConPTY cases pass (80x25, 45x34, 60x50, 120x30, mouse, live resize),
  each with output mode 7, one EXIT echo, and correct MEM blank gaps.  Five
  short-history EDIT runs pass all 20 post-EDIT MEM snapshots.  Raw records
  remain under `O:\winnt\logs\m0-t416-s5-video-*`.
- The actual `c_xcptn.c` fixture passes 27 cases; its brace-removed original
  control fails 101 assertions.  The pinned upstream patch hash passes.
- WOW command-carrier, C-VID vector-contract, version/protocol negative-peer,
  documentation-governance, and `git diff --check` checks pass.  Version-peer
  logs are `O:\winnt\logs\m0-t416-s5-version-negative`.

## Final accounting

The reproducible final paired measurement is
`build/M0-T416/S5/footprint-final-003/summary.json`.

| Cohort | S1 | S5 | Net |
| --- | ---: | ---: | ---: |
| Paired `mvdm` text distance | 4,695 | 4,621 | -74 |
| Paired `opennt-host` text distance | 14,410 | 14,410 | 0 |
| `mvdm-overlay` | 7 files / 266 lines | 7 / 266 | 0 |
| `opennt-host-overlay` | 1 file / 30 lines | 1 / 30 | 0 |
| Raw `adapter-mvdm-host-out` inventory | 10,219 lines | 9,719 lines | -500 |

Adapter inventory is raw physical size, not an assertion that all removed
lines were autonomous behavior.  The paired metric excludes unpaired/binary
paths and adapter source-owner classification.  S3 proved the remaining
overlay providers are required composition seams, so they are neither hidden
relocation nor deletion.

## Limit

This package does not claim complete protected-mode IDT/TSS or WOW/WRITE
compatibility.  The broader private-component elimination/reorganization stays
as the next queued proposal.  Owner-edited Queue/proposal files were not part
of this delivery.
