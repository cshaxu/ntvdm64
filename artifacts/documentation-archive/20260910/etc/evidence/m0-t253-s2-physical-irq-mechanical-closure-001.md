# M0 T253 S2 — selector-blind physical-IRQ mechanical closure

## Result

The native minimal machine now exposes one fixed physical-IRQ request:
`bx_ntvdm_mantle_request_physical_irq_v1(uint32_t irq)`.  It accepts only
`0..15` during an active machine stage, obtains the already-owned native PIC
through its private mantle lifecycle, and invokes `raise_irq()`.  It exposes
no vector, IVT address, CPU object, guest pointer, callback, selector, BOP,
DOS, VDM, Redirector or host handle.

This is mantle composition only. `src/bx-core` and the adopted Bochs CPU/PIC
semantics are unchanged.

## Native regression

Fresh external MSVC x64 `/MT`, CPU5/P-MMX Ninja graph:

`build/M0-T253-S2/formal-r1`

Build target and execution:

`bin/t253-s2-physical-irq-fixture.exe`

passed with exit code zero. The fixture proves:

1. inactive-stage and out-of-range requests reject;
2. a valid physical IRQ14 request is accepted by the native PIC;
3. while the slave line remains masked, PIC acknowledgement does not expose
   vector `76h`;
4. after unmasking slave IRQ6, the retained request acknowledges as original
   OpenNT network vector `76h`; and
5. reset restores inactive rejection.

This preserves real 8259 behavior: masking delays delivery; it does not turn a
valid device request into a semantic failure.

## Transfer

S3 may now implement only the bx-vdm side: copied `57:23/24` completion
records, bounded descriptor/result writes, public overlapped pipe waits and a
request of physical IRQ14. The original guest `int5c.asm` remains the sole
owner of `57:26`, ANR frame construction and IRET. S3 may not add a CPU-vector
bypass or move guest callback semantics into mantle.
