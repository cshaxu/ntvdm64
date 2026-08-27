# M0 T286 S5 — minimal memory and checked-RAM boundary review

## Decision

M1 and M2 remain private `bochs-core-overlay` mechanics.  No RAM body is
safe to move into `adapter-bochs`, and no VDM/XMS/address-mapping semantics
are admitted by this packet.

`adapter-bochs` already owns the only external decisions that are safe to
externalize:

- `adapter_bochs_minimal_machine_c::initialize(guest, host)` supplies the
  selected guest and backing allocation sizes and the one-shot lifecycle
  ordering;
- `machine_facade` exposes only copied, typed read/write/preflight requests;
- no adapter caller obtains `BX_MEM_C`, a raw vector pointer, a block table,
  or a retained host mapping.

## M1: SIM-free initialisation

The upstream `BX_MEM_C::init_memory` enters the Bochs SIM parameter tree and
state-registration product path.  The retained
`BX_MEM_C::init_memory_without_sim` must instead initialise the same private
memory object members used by the CPU and device code:

- aligned `actual_vector`/`vector`, ROM and bogus ranges;
- `len`, `allocated`, sparse block table and used-block count;
- the one-megabyte handler table and SMRAM/PCI flags.

Those fields are private `BX_MEM_C` state and are subsequently consumed by
ordinary CPU physical access, ROM/MMIO vetoes and the overlay's checked-copy
operations.  Moving this body outward would either expose private Bochs state
or recreate the same state in an adapter.  The existing core declaration plus
private overlay definition is therefore the minimum permitted split.

## M2: checked ordinary/backing RAM

`ordinary_ram_readable`, `ordinary_ram_writable`, and copied ordinary-RAM
access must check the actual Bochs map before copying.  Their required private
facts are A20 transformation, guest RAM length, memory-handler chains,
optional monitor vetoes, host-memory resolution and write-stamp invalidation.
The backing-RAM forms likewise require the private sparse vector and handler
table.  None can become a generic guest allocator, an XMS/UMB policy or a
session mapping-manager operation.

The public, selector-blind adapter façade remains the only external boundary:
it accepts fixed-width address/count/buffer values and returns success or
failure without exposing a memory pointer.  It does not name BOP, OpenNT,
DOS, VDM, WOW or XMS.

## Correction and verification

The focused ordinary-RAM boundary fixture had still inspected the former
`bochs-core/memory/misc_mem.cc` location after the body was deliberately
privatised.  It now inspects
`bochs-core-overlay/memory/checked_ram.cc`, preserving the same required
whole-span safety assertions and forbidden-semantic scan.

Static review additionally confirms:

- `minimal_machine.cc` is the sole current production caller of
  `init_memory_without_sim`;
- `machine_facade.cc` is the sole typed production caller of the ordinary-RAM
  copy/preflight methods;
- every checked-copy implementation remains in the private overlay;
- no checked-RAM body includes or names an MVDM or product-service concept.

This closes S5.  S6 must remeasure the overlay/mirror totals, refresh the
exception registers and run the focused formal x86/x64 validation graph.
