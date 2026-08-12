# T95 S6 Post-Transaction Original-Owner Provenance 001

## Ordered facts

1. r6's one generic transaction entered at `0070:0475` and returned `0479`.
2. Later, the existing opt-in x87 snapshot logs `0000:0293` immediately before
   the original `math_abort` and `DEV_pic_raise_irq(13)` in `fpu.cc`.
3. Original `pic.cc` records IRQ13 in the master PIC request register and
   calls its original service path; `pc_system.cc` then passes INTR to CPU0.
4. Only after that does the log show the repeated invalid `0000:10000`
   prefetch state and ultimately `getHostMemAddr` refusing `0xa1800`.

## Ownership

The x87 compatibility branch, IRQ13 delivery, PIC service and the upper-memory
veto are all original Bochs owners. In particular `misc_mem.cc` rejects direct
access in `0xa0000..0xbffff` because it is VGA mapped I/O. Thus the final
`0xa1800` panic is an original memory-policy result, not adapter ordinary-RAM
write behavior and not authorization to map VGA.

## Remaining gap and next boundary

The available logs prove temporal ordering but do not show which original CPU
transition changed from the coherent `0070:0479` state to the later
`0000:0293` x87 snapshot or `0000:10000` prefetch state. The next diagnostic,
if admitted, must therefore be one passive original CPU interrupt-delivery
snapshot around IRQ acknowledgement/transfer; it must not alter IRQ13, IVT,
memory mapping, or adapter semantics.
