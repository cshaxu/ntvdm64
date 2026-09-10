# M0 T282 S12 — COMMAND bounded copy binding

Recover the exact, synchronous copy forms needed by S9 before modifying the
three original COMMAND call sites.  The adapter owns a bounded C-string read
and an exact host-to-guest write over the existing numeric real-mode location
and session guest-memory lease.

## Scope

- One-byte-at-a-time bounded C-string read through fresh synchronous leases.
- One exact-length host-to-guest copied write lease.
- x86/x64 positive/negative lease tests.

## Non-goals

No COMMAND mirror modification, raw pointer compatibility macro, new mapping
manager, selector/BOP route, host environment policy or machine change.

## Exit criteria

The two source-independent bounded copy forms pass x86/x64 and give the
following mirror binding S an exact non-pointer primitive for each S9 span.
