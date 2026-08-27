# M0 T289 S5 P2 — Selector-blind protected transition

## Result

The required machine-level prerequisite for the remaining original DPMI
stack/interrupt/fault paths is now present as a narrowly registered Bochs
overlay exception, `BX-DPMI-MECH-001`.

`adapter-bochs` exposes `machine_facade_commit_same_cpl_protected_frame`.
It first checks the copied expected frame, protected non-paged mode and legal
EFLAGS delta.  The private `bochs-core-overlay` member then validates all six
candidate selectors, descriptor forms and target EIP before the first CPU
mutation. Only a same-CPL protected transition is admitted. It then applies
data segments, SS, CS, GPRs, flags and EIP as one mechanical operation.

The primitive has no DPMI, BOP, OpenNT, DOS, VDM or session identifier. It
creates no LDT/IDT entry, host LDT, monitor hook or second CPU executor.

## Verification

The existing formal S3 graph was rebuilt after adding the overlay. On both
architectures its fixture enters a real Bochs protected-mode stop, applies a
new code selector, data/stack selectors, EIP, ESP and a general register,
then re-enters the CPU loop and stops on the target `#UD`. It also submits an
invalid code selector and proves that every copied CPU field remains unchanged.

- `build/M0-T289/S3/x64/bin/t289-s3-protected-machine-fixture.exe`: exit `0`.
- `build/M0-T289/S3/x86/bin/t289-s3-protected-machine-fixture.exe`: exit `0`.

## Transfer

P3 may connect the original DPMI stack/interrupt/fault bodies through this
mechanical transaction. It must retain their source ordering and checked guest
frame writes; it may not turn source-level `setCS`/`setSS` calls into
field-by-field live CPU mutations.
