# T111 S1 unexpected-interrupt composition audit 001

## Required operations and owners

`unexp_nt.c` performs byte reads/writes at PIC ports `20h`, `21h`, `A0h` and
`A1h`, then stores one cause byte at `BIOS_VAR_START + 6Bh`. The original
handler owns the policy: it selects ISR, masks an unexpected source and emits
EOIs. Guest code owns the following IRET. Bochs must remain owner of the
actual PIC state, port dispatch, CPU and RAM.

Bochs `iodev/pic.cc` lines 62--73 registers those four ports with the native
8259 model; lines 183 onward implement the registered-port read semantics and
lines 261 onward implement writes. `memory/memory.h` lines 129--145 exposes
ordinary-RAM copy validation/copy primitives. These are the right machine
owners, not host services.

## Rejected routes

- Directly link `unexp_nt.c`: rejected now. It imports the SoftPC platform
  `inb`, `outb` and `sas_store` interfaces, not Bochs interfaces.
- Reimplement the branch in the adapter: rejected. That would make the
  adapter own PIC mask/EOI and BIOS-data semantics.
- Call the Bochs PIC object directly: rejected. The standard port-handler
  arrays in `iodev.h` are private and the PIC's direct methods are conditional
  implementation detail, while normal guest I/O uses registered dispatch.

## Recommended direction

The only viable candidate is contained reuse: compile the original handler
against a small SoftPC platform shim, where `inb/outb` invoke one new typed
**Bochs-owned generic port transaction** and `sas_store` invokes the existing
ordinary-RAM mechanism. The shim performs no IRQ, PIC, BIOS or IRET policy;
it merely carries the original operations to their owners.

That requires a narrowly documented Bochs mechanics ABI because no such
generic external port-dispatch entry is currently admitted. T112 must prove
the exact source/link closure and ABI bounds before an exception may be
registered or code changed.
