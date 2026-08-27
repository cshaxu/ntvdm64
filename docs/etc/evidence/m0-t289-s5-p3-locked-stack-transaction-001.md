# M0 T289 S5 P3 — Original locked-stack transaction

## Result

The original x86 `dpmi32/dpmiint.c` locked-stack pair now composes through the
same-shaped SoftPC register surface without exposing a partial Bochs frame.
`BeginUseLockedPMStack` retains its source save order (`EIP`, `SS`, `ESP`),
first-level nesting condition, locked selector and `0x1000` stack offset.
`EndUseLockedPMStack` retains its source restore order and `TRUE`/`FALSE`
direction.

The only registered change is that the source writes occur inside one copied
thread-local transaction. `adapter-bochs` validates every candidate selector
and target scalar before changing machine state. A rejected begin/commit
returns through the original non-switched direction and restores the nesting
count. The transaction is selector-blind and contains no DPMI, BOP, OpenNT,
DOS, VDM, guest-memory or Bochs-object classification.

## Verification

The expanded original-DPMI registration fixture enters an actual protected
Bochs stop, binds a session and worker `VDM_TIB`, invokes original
`DpmiPassPmStackInfo`, then invokes the original `BeginUseLockedPMStack` and
`EndUseLockedPMStack` bodies. It proves the exact `SS`/`ESP` switch to the
source `ES` selector and `0x1000`, plus the original-frame restoration and
zero nesting count.

- `build/M0-T289/S5/registration-x64/bin/dpmi-registration-fixture.exe`:
  exit `0`.
- `build/M0-T289/S5/registration-x86/bin/dpmi-registration-fixture.exe`:
  exit `0`.

## Transfer

P4 must apply the same whole-function transaction rule to original interrupt,
IRET and fault bodies, replacing only their `Sim32GetVDMPointer` aliases with
bounded copied stack accesses. It may not use field-by-field live setters,
host LDT/IDT mutation, CCPU execution, or a new mapping manager.
