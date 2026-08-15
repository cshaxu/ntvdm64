# T101 S1 post-BOP prefetch-map mechanics audit 001

## Question

What owns the T100 post-close prefetch veto at physical `0xAF6AF`, and does it
admit a Bochs memory/device change?

## Inputs and reproducible calculation

The sole runtime input is
`artifacts/analysis/t100-s1-dem-noop-v4-probe-001/bochs.log`. It records
`50:42` resume at `9346:6351 -> 6355`, then `50:02` resume at `9346:61a3 ->
61a7`, followed by:

```text
prefetch: getHostMemAddr vetoed direct read, pAddr=0x00000000000af6af
```

`0xAF6AF` is within the closed interval
`0xA0000 <= 0xAF6AF < 0xC0000`. The trace profile supplies 4 MiB, so this is
not the CPU's out-of-bounds/bogus-memory branch.

## Source owner map

| Layer | Direct source fact | Disposition |
| --- | --- | --- |
| CPU prefetch | `refs/bochs/cpu/cpu.cc:636-645` asks `getHostMemAddr(..., BX_EXECUTE)` and panics when an in-range page declines direct access. | Native Bochs CPU mechanics. |
| Physical map | `refs/bochs/memory/misc_mem.cc:729-736` defines `0xA0000..0xBFFFF` as PCI/ISA video memory or SMRAM. | Native Bochs PC memory map. |
| Direct-access policy | `misc_mem.cc:750-781` permits that range only for enabled/SMM SMRAM execution, then vetoes ordinary direct reads as VGA mapped I/O. | Native Bochs protection policy. |
| Ordinary device read | `misc_mem.cc:592-602` routes ordinary reads in the same range to the VGA memory callback; this is not a direct executable host page. | Confirms a VGA device does not make the range executable. |
| Caller provenance | T100 has no CPU state or transfer instruction at the panic, only the prior BOP resumes. | Unresolved; no causal attribution to BOP/provider/close is justified. |

The same policy and conclusion were independently recorded for earlier
`0xA1800`/`0xA7035` symptoms in
`t95-s6-post-transaction-original-owner-provenance-001.md` and
`r20-real-ntio-next-boundary-triage.md`; T101 independently re-applies it to
the current `0xAF6AF` trace.

## Interpretation and confidence

The veto is expected native-machine behavior after an attempted instruction
fetch in the VGA aperture. It is **not** evidence for enabling VGA, mapping
the aperture as RAM, changing `getHostMemAddr`, adding a firmware/device
feature, or moving a remedy into the adapter. Those would undermine the
minimum-machine boundary and mask the actual upstream control transfer.

Confidence is high for the address classification and owner. Confidence is
low for the transfer's cause: the retained trace cannot prove whether it is a
near/far return, an interrupt frame, segment state, or some other guest CPU
transition after `50:02`.

## Follow-up

T102 must define and validate one default-off, Bochs-owned CPU transfer
observation that records the final valid control state and the first
transition into the aperture. It may report fixed-width machine facts only;
it must not decode BOP/DOS semantics, change CPU execution, add an adapter
result, or make the aperture executable. No runtime retry is admitted until
that diagnostic contract has a separate source/ABI review.
