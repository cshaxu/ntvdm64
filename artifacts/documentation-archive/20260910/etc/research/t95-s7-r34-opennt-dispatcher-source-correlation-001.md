# T95 S7 r34 OpenNT Dispatcher Source Correlation 001

## Purpose

Interpret the last source-generic stack-transition records in r34 using the
OpenNT NTDOS source, without assigning any DOS, interrupt, or device behavior
to the adapter.

## Source correlation

The source-built `NTDOS.MAP` places `Redisp` at code offset `260F`.  In
`src/opennt/base/mvdm/dos/v86/doskrnl/dos/msdisp.asm`, the immediately
preceding dispatcher entry (`SaveAllRegs`, lines 458--490) saves the caller's
`SP` and `SS` into `User_SP` and `User_SS`, then executes `getdseg <ss>` to
switch to the DOS data segment.  `Redisp` then begins by assigning
`SP = OFFSET DosData:AuxStack` (lines 493--496).

Thus the observed runtime `0032:260A` is the preceding generated `MOV SS`
within `getdseg <ss>`; `0032:260F` is exactly the mapped `Redisp` entry.  This
is an exact map relationship, not an inferred fixed relocation delta.

The repeated `0032:26B5/26B9` records are separately consistent with the
`LeaveDos` source path (`msdisp.asm` lines 645--646): it restores
`SS = User_SS`, then `SP = User_SP`, before popping the saved register frame
and transferring to `DOIRET` (lines 654--674).

## r34 result

The last generic record is:

```
mov-ss cs=0032 rip=260a old=95ab new=00a7
```

It therefore marks *entry into the DOS dispatcher*, not the return from it.
At that point OpenNT's source requires the current caller stack to be saved
and then replaced by `AuxStack`; the diagnostic intentionally cannot see the
non-`MOV SP,r/m16` `Redisp` assignment.  The later terminal snapshot has
`SS:SP=00A7:0003`, `CS:IP=0000:019D`, and a triple stack-prefetch escalation.

This proves neither a Bochs CPU defect nor a BOP ABI defect.  It does prove
that the terminal `SP=0003` is already the caller stack value as NTDOS enters
the dispatcher, after the r30 read-result repair and after normal close
resume.  The final `0000:019D` transfer makes a low-memory interrupt/stub
contract a concrete next hypothesis, but not yet a demonstrated missing
component.

## Startup-boundary implication

The current adapter v1 startup plan writes only source-built NTIO to physical
`0x700` and sets `CS=0x70` (`bx_ntvdm_startup_session.c:82--89`).  It does not
itself synthesize an IVT, pseudo-ROM, or low-memory OpenNT stub.  That is an
intentional boundary, not evidence that NTIO did not create such state during
execution.  OpenNT owns the required low-memory machinery: its sources expose
`DOSINTTABLE`, low-interrupt stubs, and `DOIRET`/`FIRET` paths.  Bochs remains
the owner of CPU and RAM mechanics; the adapter must not fabricate those
guest structures.

## Next evidence gate

Before adding a device, BOP implementation, or further Bochs diagnostic,
capture and source-correlate the actual interrupt/vector path that leads to
`0000:019D`, including the relevant low-memory bytes and the pre-dispatch
caller `SS:SP`.  Only then classify the requirement as a guest low-memory
initialization omission, a wrong guest write, or a native machine prerequisite.
