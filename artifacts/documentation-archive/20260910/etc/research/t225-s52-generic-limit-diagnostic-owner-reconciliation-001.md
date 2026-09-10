# M0 T225 S52 — Generic limit diagnostic source/current owner reconciliation

## Current fact

S51 has two byte-identical Direct/Readonly logs. They contain a repeated
`read_virtual_checks(): read beyond limit` diagnostic after the accepted
`50:36` record, but no tuple containing instruction address, selected segment,
effective offset, access length or architectural state.

## bx-core owner

`src/bx-core/cpu/access.cc:104-162` emits that exact message only when a
valid, present readable segment fails its generic `offset + length - 1 <=
limit_scaled` condition. The message therefore identifies neither an OpenNT
BOP nor a device. `soft_int.cc:24-58` shows one possible consumer, `BOUND`,
but current logs do not establish that `BOUND` produced this invocation.

## Historical comparison

The retained T95 records place an earlier x87/IRQ13/`0xA1800` tail after an
unserved initial BOP 12h. The later source-built route passed that old
interval without admitting FPU, PIC, VGA or adapter behavior
(`t95-s6-post-5011-mechanics-symptom-supersession-001.md`). Its final upper
memory veto was original Bochs VGA-mapped-I/O policy, not an ordinary-RAM or
adapter defect (`t95-s6-post-transaction-original-owner-provenance-001.md`).

## Disposition

No current evidence joins the S51 generic message to the old T95 x87/IRQ13 or
VGA tail. It does not justify a BOP/provider repair, device enablement, a
Bochs semantic patch or a host-mutation change. The retained limitation is
that origin attribution requires an explicitly admitted, selector-blind CPU
mechanical snapshot of the failing access tuple. Until such a packet is
admitted, the correct disposition is no repair.
