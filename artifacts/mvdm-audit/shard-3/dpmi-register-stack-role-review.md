# Shard 3: DPMI register width and DOSX stack

Proposed status: **partial** for both `dpmi32/register.c` and
`dpmi32/stack.c`. The [observer](dpmi-register-stack-image-observer.py) and
[observations](dpmi-register-stack-image-observations.json) verify exact
primary and secondary original bytes, one DOS and two WOW catalog identities,
selected Ninja edges, VALID dependencies, static-library membership and
released NTVDM machine code: eight register and four stack functions are
bound with relocation words masked. Source retention is 100%; a whole-behavior
restoration percentage is unmeasurable from these observations.

The observer separately resolves six actual linked x86 call relocations:
`DpmiInitApp` -> `DpmiInitRegisterSize`; `DpmiSegmentToSelector` ->
`DpmiSwitchToDosxStack` / `DpmiSwitchFromDosxStack`; and `NoTranslation` ->
both switch functions and `DpmiSimulateIretCF`. These demonstrate the
selected initial image wiring, not that all guest paths are reached.

The original register module chooses AX/BX/CX/DX/SI/DI width based on
`CurrentAppFlags`; `IgetSP` instead reads the active SS LDT descriptor's
Default_Big bit. The original stack module allocates/frees frames on the DOSX
stack, creates real-mode interrupt frames, and merges the DOS return carry
flag into a protected-mode return frame. The NTVDM/CCPU and LDT providers own
register and memory state; `Sim32GetVDMPointer` owns pointer translation.

Still needed: actual 16/32-bit app × SS Default_Big combinations; selector
validity and handle lifetime; nested switches and frame balance; real-mode
stack-end and wrap (original source explicitly questions wrapping and has an
assertion); invalid guest pointer and callback failure; restored CS/IP/SP and
flags including the separately registered INT21 raw-input ZF adaptation;
and real guest mode transitions. A source-linked boundary fixture and a
separately identity-bound guest witness are needed; neither object identity
nor successful build is behavioral acceptance. Guest and product images were
not changed.
