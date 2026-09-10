# Adapter Startup Firmware Snapshot v0

## Problem

NTIO's historical entry consumes several pre-existing machine values before it
reaches any BOP service: virtual-interrupt bytes at physical `0x714`, IVT
entries `10h`, `15h`, `19h`, `1Bh`, and BDA `0040:0096`. r16 admits only the
last item for one pinned Bochs ROM profile. Capturing each remaining item via
a new one-off fixture, trigger, or Bochs callback would make the adapter a
collection of test hacks.

## Decision

Use one profile-owned **startup firmware snapshot** transaction at one neutral
`UD2` boundary after stock firmware has transferred to a neutral boot sector.
It contains a bounded list of independently declared ordinary-RAM ranges and
one copied CPU/event boundary. All ranges are copied before the adapter makes
any value available. The transaction is all-or-nothing: a failed range,
boundary mismatch, reset, stop or teardown discards every output range.

For the first NTIO descriptor, the profile may declare at most these four
opaque ranges:

| Opaque profile range | Physical span | Reason recorded outside the ABI |
| --- | --- | --- |
| `startup-ivt-slice` | `0x40..0x6f` (48 bytes) | contains vectors 10h, 15h, 19h, 1Bh; consumers select exact offsets later |
| `startup-bda-496` | `0x496` (1 byte) | NTIO reads keyboard flag bit 4 |
| `startup-virtual-interrupt` | `0x714..0x717` (4 bytes) | host preserves this span over initial NTIO placement |
| `startup-bda-memory-kib` | `0x413..0x414` (2 bytes) | source-derived input for later BOP `12h`; captured now, not yet a service result |

The names are evidence/profile labels, never Bochs source constants. The
larger IVT slice avoids four separate physical-read operations while keeping
the declared aperture below 64 bytes. No arbitrary memory-range request is
introduced.

## Boundary

```text
selected profile ranges + exact UD2 stop
   -> adapter validates immutable snapshot request
   -> Bochs generic ordinary-RAM copier processes each declared range
   -> adapter owns bytes and a combined fixed digest
   -> profile evidence decodes/accepts exact fields
   -> later generic startup transaction may preload NTIO
```

Bochs receives only a bounded list of physical range/output spans tied to the
copied `#UD` boundary. It does not receive field names, NTIO addresses, IVT
numbers, BDA meanings, profile paths, BOP selectors or host capability data.
The adapter has no firmware timer, BIOS implementation, DOS filesystem or
DEM/BOP service semantics. The future host startup coordinator consumes a
validated snapshot through adapter-owned value records; it never reads Bochs
memory directly.

## ABI Direction

Do not mutate the existing single-range observation transaction v1. Add a
separate fixed-size `runtime_startup_snapshot_transaction` with:

- ABI header and zero flags;
- one copied `#UD` event and CPU snapshot;
- 1..4 opaque range descriptors, each constrained by the installed profile;
- no pointers, output bytes, paths, firmware labels, BOP selectors or guest
  semantic values.

The runtime owns one fixed output arena and per-range offsets, validates total
output capacity before queueing, and exposes a single one-time completion that
reports a deterministic digest over ordered descriptors and bytes. This is a
new pending-operation kind, so the existing r15 single-range observation
contract remains stable and independently testable.

## Admission Gates

1. C11 ABI/layout and negative tests prove count, range, overflow, aperture,
   output-layout, boundary and one-time lifecycle rules.
2. Adapter runtime test proves a profile can declare the four-range snapshot
   and produces no data before generic copy completion.
3. One generic Bochs consumer loops over the already-queued bounded list,
   invoking its existing all-or-nothing ordinary-RAM primitive for each range.
   It must preflight every range before copying any byte.
4. A CPU5 stock-firmware neutral-floppy probe records one combined digest and
   a field decoder proves the exact values. The r16 `0x496 = 0x10` result is
   revalidated as part of that bundle, not silently reused across profiles.
5. Only then may a distinct generic NTIO preload/entry transaction be
   implemented. Snapshot acquisition must not write guest memory or start
   NTIO.

## Explicit Non-Goals

- no synthetic BIOS, IVT, BDA clock or interrupt behavior;
- no guest DOS/NTDOS filesystem or DEM implementation in the adapter;
- no BIOS `12h` answer, BOP `50:11` policy change, COMMAND launch or target
  process contract;
- no implicit device enablement and no stock `bochs.exe` full-build claim.

## r17 Fixture Declaration

`Invoke-Bochs26NativeResetFixture.ps1 -StockFirmwareStartupSnapshotProbe`
selects the custom `runner-observation-fixture.exe` target, explicitly boots
the same neutral floppy, and supplies exactly the four ranges above through
the BYOB profile. It expects only the opaque
`runner adapter snapshot copied digest=...` log predicate before the bounded
benchmark stop. The fixture does not preload NTIO or add a target command.
