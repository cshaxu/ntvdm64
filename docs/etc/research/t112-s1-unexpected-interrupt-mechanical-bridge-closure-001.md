# T112 S1 unexpected-interrupt mechanical bridge closure 001

## Finite original-source dependency manifest

`base/mvdm/softpc.new/base/system/unexp_nt.c` requires only `inb`, `outb`,
`sas_store`, diagnostics, fixed historical types and the PIC/BIOS constants
from its direct headers. Its runtime operations are PIC ISR/IMR reads, OCW3,
mask and EOI writes at `20h`, `21h`, `A0h`, `A1h`, then one BIOS-data byte
write at `BIOS_VAR_START+6Bh`.

Bochs `bx_devices_c::inp/outp` is the native registered-port path
(`iodev.h` lines 447--448; `devices.cc` lines 838 and 871), and `pic.cc`
registers exactly those ports at lines 62--73. Ordinary RAM copy remains a
Bochs mechanical primitive.

## Boundary decision

The current adapter result ABI returns a result only after adapter dispatch.
The original function instead needs synchronous, ordered I/O operations while
the #UD callback remains active. A broad adapter-callable port API would make
the adapter a re-entrant device client and permit arbitrary device semantics.
It is rejected. No new Bochs intrusion is justified yet.

T113 therefore first proves whether the unmodified original function has a
small compatible compile island. No shim, link, handler or runtime action is
admitted by this record.
