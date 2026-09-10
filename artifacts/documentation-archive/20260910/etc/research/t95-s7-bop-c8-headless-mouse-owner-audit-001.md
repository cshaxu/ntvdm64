# T95 S7 BOP C8 Headless Mouse Owner Audit 001

## Question

What does the r17-discovered `C4 C4 C8` require, and can the first CLI profile
continue without importing the historical Windows mouse subsystem?

## Evidence

The r17 `06` fault-frame snapshot is `C4 C4 C8 33 C0 8E C0 26` at the original
`8DC8:78CB` boundary.  The inventory maps `C8` to
`v_host_mouse_install1` when `NTVDM` is selected.

`dos/v86/doskrnl/bios/spcmse_4.asm:5310-5388` constructs `mio_table` in guest
memory and invokes `BOP 0C8h`.  Its immediate continuation reads the then
installed INT 33h vector, saves it in guest jump patches, and finally replaces
INT 33h with the guest's wrapper.  `softpc.new/host/src/nt_mouse.c:282-287`
shows the historical host entry sets host mouse state, registers an EOI hook,
and calls `mouse_install1`.  That latter routine reads the guest table and
writes INT 71h/33h vectors, unmasks mouse IRQs, and installs further host
mouse behavior.

## Ownership conclusion

Windows pointer integration, EOI hooks, hardware mouse IRQs and SoftPC mouse
state are excluded from the headless CLI profile.  But the guest's own
continuation has a narrow, proved dependency on the initial INT 33h mapping.
The candidate is therefore not a Bochs mouse device and not a host mouse
implementation: it is an adapter-owned, bounded mapping transaction derived
from the already constructed guest `mio_table`.

## Exact headless transaction contract

The r17 listener records the actual call as `CS:BX=8DC8:77BF`; therefore the
source-created table begins at physical `0x9543F`.  `spcmse_4.asm` writes the
first pair as `mouse_io offset, original guest DS segment`.  Its immediate
post-BOP continuation reads only IVT `33h*4` through `33h*4+3`, copies those
four bytes into guest-local jump data, then overwrites INT 33h with its own
guest wrapper.  Thus the necessary first-phase contract is exactly one
readable, non-wrapping four-byte range at `CS*16+BX`, followed atomically by
one ordinary-RAM four-byte write to IVT physical `0xCC`, carrying those bytes,
and checked resume at `RIP+3`.

The historical `mouse_install1` also writes INT 71h and configures PIC/IRQ9,
but no reached continuation before the next BOP reads INT 71h; those actions
are rejected from this profile pending a distinct source-reached proof.
Failure to validate the exact real-mode C8 window, range, gather completion,
or IVT write remains listener pass-through, never synthetic mouse success.
The service uses the already admitted generic checked gather-read/multi-write
mechanics. No host pointer, GUI, interrupt injection, IRQ unmask, EOI hook,
broad mouse service, or Bochs feature is admitted.

## Runtime result

The immutable r18 trace
`artifacts/analysis/t95-s7-runtime-trace-017-20260811-001` records the reached
`selector=c8` with the audited `CS:BX`, then exactly `gather copied ranges=1
bytes=4` and one `multi-write transaction committed writes=1 bytes=4` at the
same boundary. The source-built binary is SHA-256
`DC5B7550FFC5DBB120432C422929C7F505DD72062B9454B39C20EDA05957D9A2`.
This proves the C8 mapping transaction, not a mouse device or input feature.
The following path reaches INT 6 whose first captured frame begins
`C4 C4 17 5A`; BOP 17 is the next source-first audit target. C8 is no longer
a cause candidate for that later exception path.
