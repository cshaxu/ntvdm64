# M0 T198 S60: NTIO `BOP 5F` SoftPC Keyboard-Handoff Audit

## Observed contract

The clean source-built witness at
`artifacts/build/t198-s60-spckbd-observation-r1` reaches a three-byte
`C4 C4 5F` BOP at `8dc8:45a6` with the following copied state:

| Field | Value |
| --- | --- |
| AX | `beef` |
| BX | `9885` |
| CX | `0004` |
| DX | `0000` |
| DS:SI | `073b:4454` |
| ES:DI | `0000:041d` |
| FLAGS | `00000046` |

The byte after this three-byte BOP is guest opcode `72`; it is not a BOP
service number.

## OpenNT source map

`base/mvdm/inc/bop.h` names selector `5F` `BOP_UNIMPINT`.  In
`dos/v86/doskrnl/bios/spckbd.asm`, `InstSpcKbd` builds the keyboard/interrupt
table, supplies `DS:SI`, `ES:DI`, `CX`, and `AX=VERSIONID`, where
`VERSIONID equ 0BEEFh`, then executes `bop 5fh`.  The immediately following
`jc isk_int9` selects the x86-only path when CF is set.

## Existing provider and disposition

`bx_ntvdm_spckbd_init_service_v1_dispatch` already validates exactly this
real-mode, vector-6, `C4 C4 5F`, `AX=BEEF` input.  It advances by three bytes
and sets CF.  It neither parses the table nor provides keyboard, PIC, 8042,
IRQ, host input, firmware, or Bochs behavior.

The provider was only reachable through the detached historical adapter
runtime.  S61 may compose that existing exact provider directly at the
top-level composition boundary.  Its CF=1 outcome intentionally preserves
OpenNT's `isk_int9` x86 fallback instead of pretending that the SoftPC
C-BIOS keyboard handoff succeeded.

