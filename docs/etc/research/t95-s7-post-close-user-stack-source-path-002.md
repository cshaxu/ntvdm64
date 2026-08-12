# T95 S7 Post-Close User-Stack Source Path 002

## Question

What original OpenNT code owns the ordinary path after r30's verified
`50:02` close resume, and what state can legitimately produce its later
real-mode stack use?

## Inputs

- r30 trace: `artifacts/analysis/t95-s7-runtime-trace-025-20260811-001`.
- NTDOS source: `upstream/opennt/base/mvdm/dos/v86/doskrnl/dos/handle.asm`,
  `mscode.asm`, and `msdisp.asm`.
- NTDOS map: `upstream/opennt/base/mvdm/dos/v86/doskrnl/dos/NTDOS.MAP`.

## Source path

The trace's accepted close resume is `61A3 -> 61A7`.  The mapped `$Close`
source reaches `CloseOK` in `handle.asm:150-152`, which sets `AH=close` and
transfers to `SYS_RET_OK`; it has no further DEM BOP.

`mscode.asm:505-520` implements `SYS_RET_OK`: it invokes `Get_User_Stack`,
clears the saved user's carry bit, writes the return AX field, and returns.
`Get_User_Stack` in `msdisp.asm:770-776` loads `DS:SI` from the far
`User_SP` state.  The map identifies `SYS_RET_OK` at `0000:27F9`,
`LeaveDOS` at `0000:269F`, `Get_User_Stack` at `0000:2738`, and the DOS data
symbols `User_SP=0794:0586` / `User_SS=0794:0588`.

The dispatcher exit `LeaveDos` at `msdisp.asm:625-674` is the source-owned
live-stack transition: it loads `SS` from `User_SS`, `SP` from `User_SP`,
pops the saved user registers, then performs the original `DOIRET`.  It also
updates `User_SP` and `User_SS` from `Nsp` and `Nss` only after selecting the
current user frame.  Therefore no adapter BOP result has authority to invent
or repair that frame.

## r30 correlation

r30 proves the close BOP reaches `61A7` with an empty GPR delta and then no
intervening BOP occurs before native exception escalation.  Its later
`SS=00A7`, `SP=0003` is consequently downstream of the normal NTDOS return
path, not a direct output of `50:42` or `50:02`.  The retained log lacks an
ordinary-instruction trace or a snapshot of `User_SS:User_SP` at `LeaveDos`,
so it cannot identify the first wrong writer or assert that the source data
itself is wrong.

## Classification and follow-up

The remaining candidate class is the source-owned user-frame/return path or
an earlier guest-memory/state writer.  It is not evidence for another BOP,
adapter register result, Bochs CPU/FPU/PIC/VGA feature, or host capability.
The rejected r27/r28 CPU-loop logger cannot be revived.  Before another
runtime observation, an original same-configuration observation facility or
a separately proven object closure must be shown capable of recording the
`LeaveDos` user-frame transition without changing CPU semantics.
