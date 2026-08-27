# M0 T289 S5 P1 — Original DPMI registration composition

## Result

This is a bounded S5 checkpoint, not S5 closure. The two original registration
entrypoints in `dpmiint.c` now retain their original provider route through a
checked private binding:

- `DpmiSetProtectedmodeInterrupt` reads its source-defined ten-byte `SS:SP`
  record and records the original interrupt-handler fields.
- `DpmiSetFaultHandler` reads its source-defined eighteen-byte `SS:SP` record
  and records the original fault-handler fields.

The historical durable `Sim32GetVDMPointer` alias is not enabled. The matching
private overlay takes one selector-blind protected-span copy, validates the
fixed record and stores it in the current worker's source-shaped `VDM_TIB`.
Valid records keep the original `NtVdmControl(VdmSetInt21Handler, ...)` call
order and `AX=0` success result. Malformed spans or indices outside the source
table capacities fail without modifying AX or the table.

## Verification

`tools/build/New-T289S5DpmiRegistrationNinja.ps1` creates a formal `/MT`
fixture using a real minimal Bochs protected-mode stop and the original
`dpmiint.c` object. Both architectures pass:

- `build/M0-T289/S5/registration-x64/bin/dpmi-registration-fixture.exe`:
  exit `0`.
- `build/M0-T289/S5/registration-x86/bin/dpmi-registration-fixture.exe`:
  exit `0`.

The fixture verifies interrupt registration, fault registration and the
out-of-range interrupt no-mutation case. Its dormant-path
`Sim32pGetVDMPointer` stub exists only to link the whole original translation
unit; no IRET, fault dispatch, stack switch or route invokes it.

`tools/build/New-T289S5DpmiInterruptAuditNinja.ps1` also rebuilds the whole
selected S5 source group (`stack.c`, `dpmiint.c`, `dpmimscr.c`, `modesw.c`,
`savestat.c`, `data.c`, `register.c`) as a source archive on x86 and x64.
Neither graph selects `src.old`, a DOSX host object, a host LDT implementation
or BOP ingress.

## Remaining S5 boundary

Original interrupt/fault dispatch and IRET paths alter CS/SS/EIP/ESP/EFLAGS
together after writing a guest IRET frame. The current S3 copied-frame contract
intentionally rejects segment-selector changes. S5 therefore stays active
until a selector-blind Bochs-private prevalidated atomic transition is designed
and proven. Field-by-field setters must not be substituted for that
transaction.
