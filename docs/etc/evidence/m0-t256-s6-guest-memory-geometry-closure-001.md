# M0 T256 S6 — Guest-Memory Geometry Closure

## Result

The running session now carries a fixed-width `guest_memory_bytes` contract
from `ntdos64-native --guest-memory-kib` through bx-vdm engine composition to
the selector-blind machine-stage constructor.  The default remains 1 MiB.
Admitted values are 1..16 MiB in 64 KiB units.

No adopted Bochs or OpenNT source changed.  The mantle receives only capacity;
it does not learn DPMI, a BOP, selector policy, a reserved interval or a host
pointer.  Existing low-memory BOP shims remain unchanged.

## Formal verification

Fresh external graph: `build/M0-T256-S6/formal-r2`.

- `t256-s6-guest-memory-geometry-fixture.exe`: exit 0.  Proves default
  1 MiB, malformed/subgranular and oversized request rejection, then a 2 MiB
  stage whose first extended address (`0x100000`) accepts ordinary-RAM
  copied write/read before clean reset.
- `t256-s6-engine-geometry-contract-fixture.exe`: exit 0.  Proves engine
  default, 2 MiB admission and invalid geometry rejection.

## Remaining boundary

This provides capacity only.  A future range-reservation package must still
obtain an explicitly reserved interval from session composition, and the
OpenNT DPMI mirror must replace host pointer/`XMEM_BLOCK *` identity with
session records.  No `53:xx` body is claimed by this closure.
