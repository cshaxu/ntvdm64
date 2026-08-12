# T95 S7 Post-DEM Close User-Stack Provenance 001

## Question

Does the reached `50:02` DEM close response itself explain r24's later
real-mode `SP=0003` stack fault?

## Source correlation

The reached source-built NTDOS bytes at `61A3` are `C4 C4 50 02`. The matching
OpenNT source, `doskrnl/dos/handle.asm:137-152`, restores `BP`, clears the SFT
reference count, branches only on CF, then transfers to `SYS_RET_OK`.

`doskrnl/dos/mscode.asm:504-549` shows that `SYS_RET_OK` calls
`Get_User_Stack`, clears only the saved user carry bit, and writes only the
saved user AX field. It neither loads SS:SP nor pops an interrupt frame.

The actual restoration is later in `doskrnl/dos/msdisp.asm:644-668`:

1. `LeaveDos` executes `mov SS,User_SS` and `mov SP,User_SP`.
2. It restores the saved registers and reaches the original IRET path.

`User_SP` is originally captured at `msdisp.asm:477-478` when the DOS system
call dispatcher takes ownership of the caller's frame. `Get_User_Stack`
itself is the direct `lds SI,DWORD PTR User_SP` at `msdisp.asm:770-776`.

## Result

r24 has a correct close resume at `61A3 -> 61A7` with `SP=091E`, but its later
fault dump has `SP=0003`. Since the close responder preserves general
registers and writes only CF, it cannot directly supply that value. The
source-defined path instead makes `SP=0003` evidence that the saved
`User_SP:User_SS` pair or the preceding DOS system-call entry/frame has become
invalid before `LeaveDos` restores it.

This is not authority to synthesize a stack, alter the close result, add a
BOP service, or add a Bochs device. The next admissible source investigation
is the specific entry which captured `User_SP` for this reached close, its
interrupt/call frame, and the ownership of its initial SS:SP setup.
