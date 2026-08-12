# T110 S1 selector-02 owner provenance 001

## Proven owner and continuation

The T108 trace reaches `C4 C4 02` at `073B:03FF`. OpenNT's SoftPC BIOS table
in `base/mvdm/softpc.new/base/bios/bios.c` lines 120--124 binds BOP `02` to
`unexpected_int`. It is deliberately not an `MS_bop_*` host-service selector.

The guest source has the matching caller form:

- `dos/v86/doskrnl/bios/spckbd.asm` defines `UNEXP_BOP equ 2` at line 57 and
  emits `bop %UNEXP_BOP` followed by `jmp iret_com` at lines 452--457.
- `dos/v86/doskrnl/bios/spcmse_4.asm` has the same `UNEXP_BOP equ 2` and
  `bop UNEXP_BOP; jmp DOIRET` at lines 1752--1754.

Thus a three-byte BOP resume alone is not its full contract. It enters the
original unexpected-interrupt handler, which then returns guest control to
the IRET continuation. The repeated T108 `02 -> prefetch -> 02` boundary is
consistent with omitting that host-side handler; it is not evidence for XMS,
VdmRedir or a generic adapter failure result.

## Semantic ownership

`base/mvdm/softpc.new/base/system/unexp_nt.c` lines 50--91 implements
`unexpected_int`. It reads the master PIC ISR and mask through ICA ports,
distinguishes software from hardware interrupts, masks/EOIs unexpected
hardware sources (including the slave PIC where applicable), and stores the
cause in the BIOS data area at `BIOS_VAR_START + 0x6b`.

This is SoftPC machine/interrupt composition. Its CPU IRET is guest-owned;
the PIC and ordinary RAM/BIOS-data area are machine-owned. The adapter must
not become an alternate owner for these port, IRQ, EOI, mask or BIOS-data
semantics.

## Decision

Selector `02` is classified **blocked pending minimum machine composition**,
not as an adapter provider. T111 audits whether the original handler can be
contained through a narrow Bochs PIC/RAM platform interface, or whether a
source-derived rehost is justified by an explicit composition blocker. No
implementation is admitted by this provenance result.
