# NTDOS Load Transaction Preflight Work Order

Status: M6 implementation work order, 2026-08-08. This work is a private-copy
transaction declaration for the NTDOS image. It is not an implementation of
the historical `demLoadDos` service.

## Source Fact

`src/opennt/base/mvdm/dos/dem/demmisc.c`, `demLoadDos`, obtains its load
address with `GetVDMAddr(getDI(), 0)`: `DI` is the real-mode load segment and
the offset is zero. It then reads NTDOS in 16 KiB chunks. The historical code
has no bounded preflight for its complete file length, so the modern research
route must validate the full destination interval before any guest write.

## Outcome

Given transient, identity-checked NTDOS bytes, a caller-supplied `DI`, and a
declared ordinary-RAM capacity, build a private immutable-by-convention copy
transaction:

```text
DI << 4 + complete NTDOS byte count
  -> checked ordinary-RAM interval
  -> one private preload declaration
```

The module neither recognizes a service byte nor opens NTDOS itself. A later
mantle profile adapter invokes it only after the original historical caller or
an explicitly admitted source-derived replacement transition has reached the
load boundary.

## Tests

1. A caller-selected nonzero `DI` maps to `DI << 4`, preserving the source
   buffer and copying all bytes privately.
2. Empty image and complete-range overflow reject without allocating output.
3. A nonzero `DI` near the upper capacity boundary cannot wrap.
4. No target links historical objects, core archives, or the normal CLI.

## Implementation Evidence

On 2026-08-08, the transaction planner was implemented in
`src/nt4_ntdos_load_plan.[ch]` and tested by the default-disabled
`nt4-ntdos-load-plan-test` target. It uses only owned five-byte test input and
proves a caller-selected `DI:0000` destination, source-buffer immutability,
complete private copy declaration, and atomic rejection for empty or
out-of-capacity ranges. The historical source defines that `DI` is live guest
input; it does not define the fixture's numeric value. See
`NTIO-IMAGE-LAYOUT-PROVENANCE-CORRECTION.md`.

The fixture also verifies that a short `FFFF:0000` load remains valid under a
one-MiB capacity when its end is within range. This prevents the wrapper from
inventing 20-bit wrap or an artificial rejection; a smaller capacity rejects
the same interval correctly.

The recorded CTest run includes M0, M1, M2, the NTIO preflight, and this
planner. All five tests passed, and `ntdos64-run` remained a single-object
normal CLI target.
