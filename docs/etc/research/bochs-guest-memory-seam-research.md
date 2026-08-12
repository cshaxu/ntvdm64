# Bochs Guest-Memory Seam Research

## Question

Can the adapter obtain checked guest-memory copies without importing Bochs
memory semantics or exposing Bochs objects?

## Inputs And Procedure

Reviewed the pinned Bochs 2.6 memory and CPU access paths:

- `memory/misc_mem.cc`: `BX_MEM_C::dbg_fetch_mem` and `dbg_set_mem`;
- `memory/memory.cc`: physical page read/write implementations;
- `cpu/paging.cc`: CPU physical-access helpers;
- `memory/memory.h`: `BX_MEM_C` ownership and debugger-oriented declarations.

## Observation

The debugger methods already copy bytes but take `BX_MEM_C`/CPU internals and
have debugger-oriented behavior. CPU physical access additionally relies on
current CPU translation state. Neither is an adapter ABI and neither may be
exposed as a pointer/object across the boundary.

## Decision

The adapter owns only the numeric aperture validator. If byte copying becomes
necessary, a registered Bochs seam must accept copied scalar address/length
and caller-provided byte buffers inside the same UCRT island, first validate
against the declared physical aperture, and then use native Bochs memory
ownership internally. It must not expose `BX_MEM_C`, `BX_CPU_C`, raw guest
mapping, debugger API, or paging policy. Read and write require separate
admission; no memory seam is implemented by this record.
