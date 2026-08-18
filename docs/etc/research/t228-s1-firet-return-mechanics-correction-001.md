# T228 S1 P28 — OpenNT FIRET Return-Mechanics Correction

## Question

Does P24's zero successful post-`RSP_COMMIT` IRET record indicate a missing
Bochs interrupt-return behaviour on the current x86 guest path?

## Inputs

- `src/opennt/base/mvdm/inc/vint.h:111..168`
- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msdisp.asm:625..674`
- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msproc.asm:337..371`
- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msctrlc.asm:1083..1085`
- P23, P24 and P25 retained records.

## Source Facts

`LeaveDos` restores the user frame and transfers to `DOIRET`; `DOIRET`
expands `FIRET`. That name does **not** mean an unconditional x86 `IRET`.
In the normal x86 branch, when neither the MIPS/RI flag, trap flag, nor a
pending virtual interrupt requires architectural `IRET`, `FIRET` restores the
saved guest flags and performs `retf 2`, explicitly discarding the saved
flags word. Its `iret` instruction is only the exceptional branch.

`EXEC` function `4B05` also deliberately rewrites its near return address to
`LeaveDos`. Therefore the COMMAND/NTDOS lifecycle can complete its normal
guest return path without ever reaching P23's real/V86 IRET16/IRET32
post-commit observer.

## Interpretation

P24's paired `interrupt-returns count=0` remains an accurate description of
that particular observer. It is not negative evidence against the normal
OpenNT x86 interrupt-return path and cannot exclude a successful normal
`LeaveDos -> DOIRET -> FIRET` completion. P25's former wording that a normal
INT 21 completion *expects* a completed real-mode IRET is superseded by this
correction.

No BOP resume, provider, CPU, Bochs, adapter, profile, guest-input, or runtime
change follows. The active question remains the complete original COMMAND /
NTDOS environment, MCB, transient-placement and relocation lifecycle.

## Follow-up

Future T228 work must use an owner-package source/ABI/failure comparison that
can distinguish the guest lifecycle state itself. It must not treat the P23
IRET ring as a completion oracle or derive a leaf repair from its zero count.