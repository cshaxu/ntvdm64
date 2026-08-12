# T134 S1 post-handoff fault-frame provenance 001

## Question

What does the corrected T130 trace prove about the post-handoff
`0BC1:FFFF` / `EIP=10000h` fault, and does it admit a BOP, adapter, reset, or
device correction?

## Evidence

- T130 trace lines 395--398 record ordinary guest activity at `50:36`, then
  `prefetch: EIP [00010000] > CS.limit [0000ffff]`. Bochs' existing
  real-mode vector trace records the ensuing general-protection delivery as
  vector `0Dh`, with pre-delivery `CS:IP=0BC1:FFFF`,
  `SS:SP=0BC1:FFF8`, and target `0B43:0088`.
- The same trace's next observation is selector `02h` at `073B:03FF`; the
  independently admitted machine-composition component returns outcome `1`
  (handled resume). The prefetch/vector/BOP sequence then repeats without a
  new service result or adapter-side host action.
- In original OpenNT
  `base/mvdm/dos/v86/doskrnl/bios/spckbd.asm`, `InstSpcKbd` installs INT 0Dh
  as `unexp_int`. `unexp_int` executes `bop %UNEXP_BOP` and transfers to its
  guest IRET continuation. This source contract accounts for the observed
  `0Dh -> selector 02h -> handled resume` chain.
- The BOP observation is deliberately after vector delivery and guest handler
  entry. Its `073B:03FF` snapshot and handler stack cannot identify the
  earlier instruction which made `IP=FFFFh` advance beyond the real-mode
  limit. The T130 trace contains neither bytes/mapping for `0BC1:FFFF` nor a
  predecessor control-transfer record.

## Disposition

The exact failure chain is **guest post-handoff control state**:

`0BC1:FFFF` instruction fetch -> original CPU general-protection delivery ->
original INT 0Dh / `unexp_int` -> original BOP `02h` / guest IRET -> same
invalid fetch.

This proves neither a BOP-service error nor an adapter host-service action.
It also does not reopen T133's satisfied native POST/IVT/INT10 lifecycle, and
does not prove a missing Bochs device or mapping. `outcome=1` shows only the
already-admitted original handler/resume contract; it is not the predecessor
of the invalid transfer.

The exact upstream owner remains **unidentified**. The smallest missing
observation is a selector-independent, pre-fault record at the original
Bochs segment-limit/prefetch boundary that preserves the existing pre-fault
`CS:IP` and enough original transfer context to identify the instruction that
produced `10000h`. No source change is admitted by T134. A separate task
must first determine whether an existing registered diagnostic can supply
that record; otherwise it must register a strictly default-off Bochs-local
observation before any implementation or runtime retry.
