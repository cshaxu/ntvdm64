# M0 T244 S2 — Headless native 8042 lifecycle closure

## Question

Can the reached `BOP 5F` keyboard follow-on receive a minimal, native Bochs
8042 controller without importing the old Bochs product shell or moving any
VDM meaning into the machine layers?

## Inputs

- Bochs 2.6 `iodev/keyboard.{cc,h}` (adopted under `src/bx-mantle/`),
  source identity preserved by the rename.
- Existing mantle empty port space, native PIC and PC timer.
- T243's source-shaped `spckbd` handoff and T244 S1 source/ABI audit.

## Procedure

`BX-MANTLE-091` was registered before the patch.  The adopted controller was
severed only from plugin registration, SIM runtime parameters, GUI LEDs,
CMOS publication, default mouse/host input and clipboard paste.  The mantle
factory owns the singleton and restores the stub before port-space cleanup.
The remaining controller registers `60h/64h`, uses a fixed one-microsecond
native timer and raises normal IRQ1 through the existing PIC.

The formal MSVC x64 `/MT` Ninja graph was regenerated in the disposable
`build/M0-T244-S2/formal-r2/` tree and ran
`t244-s2-headless-8042-lifecycle-fixture.exe`.

## Observations

- Before composition, port `64h` is rejected by the empty port space.
- After composition, the controller status is readable on `64h`.
- Upstream keyboard command `F2h` produces the first original `FAh` ACK.
- With IRQ1 unmasked, the native timer produces PIC IAC vector `09h`; normal
  EOI is accepted on `20h`.
- An auxiliary-device `D4h` command yields neither a synthetic mouse byte nor
  AUX status; the guest retains its controller timeout/error path.
- Cleanup removes the port handlers and restores the keyboard stub.  The
  same sequence succeeds twice in one process.

## Interpretation

The headless `60h/64h` / timer / IRQ1 device slice is code, link and local
runtime complete.  It is a selector-blind machine capability, not a BOP
implementation.  No claim is made for host keyboard input, mouse, GUI,
CMOS, PIT/IRQ0, video, firmware, monitor IRET/ICA behavior or full interactive
guest continuity; those have separate owners.

## Follow-up

T244 can close this headless lifecycle slice.  A later PC/console owner
package may add an independently admitted input or firmware capability only
after a reached caller and its own boundary review.
