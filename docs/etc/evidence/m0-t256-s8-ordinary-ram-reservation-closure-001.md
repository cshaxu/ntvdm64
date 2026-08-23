# M0 T256 S8 — Ordinary-RAM Reservation Closure

## Outcome

The explicit ordinary-RAM reservation lifecycle is code and focused-formal
closed. It is a selector-blind bx-mantle facility, not a DPMI or BOP provider.

## Contract

- The copied engine/stage fields `reserved_memory_base` and
  `reserved_memory_bytes` are both zero by default.
- A nonzero interval is 64 KiB aligned, begins at or above 1 MiB, is wholly
  inside the selected 1–16 MiB RAM capacity, and is preflighted readable and
  writable ordinary RAM before lifecycle activation.
- The fixed record allocates a bounded copied address with a power-of-two
  alignment and opaque non-pointer ID, or releases by that ID. It neither
  exposes backing memory nor encodes a provider identity.
- The finite record table first-fits without overlap. Reset clears the
  configuration and every live record.

## Evidence

Fresh formal graph: `build/M0-T256-S8/formal-r2`.

The focused `t256-s8-ordinary-ram-reservation-fixture` passes:

1. inactive lifecycle rejection;
2. malformed 64 KiB interval rejection;
3. zero/default configuration rejection;
4. 2 MiB stage with explicit `[1 MiB, 1 MiB]` interval;
5. two 64 KiB first-fit allocations at `00100000` and `00110000` with distinct
   opaque IDs;
6. checked ordinary-RAM backing write/read;
7. release then first-fit reuse; and
8. reset returning to lifecycle rejection.

`t256-s6-engine-geometry-contract-fixture` also passes in the same graph,
including containment and alignment rejection for the new copied fields.

## Boundary

`BX-MANTLE-097` was registered before implementation. No adopted Bochs source,
OpenNT source, BOP selector, DPMI term, host allocation or raw pointer enters
the implementation. The later bx-vdm DPMI mirror may request this facility,
but it alone owns any translation into source-derived block state.
