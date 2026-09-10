# T141 S1 original DEM-entry lifecycle contract 001

## Original contract

`SVC_DEMENTRYDOSAPP` (`50:36`) dispatches in original `demdisp.c` to
`demEntryDosApp` in `demmisc.c`. NTDOS `msproc.asm` issues it immediately
before it clears InDos, changes to the application `SS:SP`, establishes DS/ES
from the PDB, restores initial AX, and `retf`s to the application entry.

The original handler reads DX as PDB and, after the first call only, invokes
`VDDCreateUserHook(PDB)`. In a normal non-debug build it then returns `VOID`:
it has no guest-memory operation, CPU result, stack rewrite, interrupt, or
machine-device action. With no registered VDD user hook, its net behavior is
an empty return.

## Current composition

The existing source-derived adapter endpoint is limited to the exact real-mode
`C4 C4 50 36` form and returns at `fault_rip + 4` with an empty CPU delta. The
contained CLI profile admits no VDD loading/registration facility, so its
empty-hook disposition is source-equivalent to the original no-hook path.
It neither reads PDB nor reimplements VDD behavior.

## Disposition

`50:36` needs no new BOP provider and is not a cause of the T130 late fault.
The remaining causal surface is the following original NTDOS guest transfer:
`mov SS,AX; mov SP,DI; ...; retf`. T142 is limited to static source/trace
correlation of that transfer; no guest/host behavior is changed.
