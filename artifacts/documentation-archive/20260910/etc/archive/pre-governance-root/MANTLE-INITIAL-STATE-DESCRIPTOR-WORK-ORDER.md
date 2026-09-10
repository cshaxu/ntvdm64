# Mantle Initial-State Descriptor Work Order

Status: M9 implementation work order, 2026-08-08. This is a typed profile
data validator for the first five source-enumerated NTIO inputs. It does not
place data in guest memory.

## Fixed Fields

The descriptor has exactly these profile fields, in the order established by
`NTIO-FIRST-LIVE-IVT-BDA-LEDGER.md`:

```text
incoming IVT 10h, 15h, 19h, 1Bh
incoming BDA byte 0040:0096
```

Each value carries a typed evidence class and nonzero opaque evidence record
identifier. Accepted classes are `source_locked` and `trace_locked`.
`unproven` is a valid descriptor representation but fails admission. Values
themselves are not judged by zero/nonzero content, because `0000:0000` may be
a real observed vector; provenance, not a guessed numeric default, determines
admission.

## Boundary

The descriptor belongs to mantle profile data. Core only later receives an
already accepted initial-memory transaction. The external NTDOS adapter selects
a profile and reports the redacted field/result code; it may not fill missing
values. No source file, JSON profile parser, firmware byte blob, IVT write, or
interrupt handler is part of this task.

## Tests

1. A fully source-locked descriptor accepts, including zero-valued vector
   fields.
2. Each missing/unproven vector rejects with its stable field result.
3. Missing/unproven BDA input rejects.
4. An accepted result is a copied immutable value record; later caller changes
   cannot change it.
5. The target remains outside the normal CLI and core archive graphs.

## Implementation Evidence

On 2026-08-08, the descriptor was implemented as
`src/nt4_mantle_initial_state.[ch]` and tested by the default-disabled
`nt4-mantle-initial-state-test` target. The test admits a fully source-locked
record with zero-valued vectors, then independently rejects each unproven or
missing-record IVT field and the BDA keyboard byte. It also proves that the
accepted record is copied rather than borrowed.

The recorded CTest run passed seven owned fixtures, from M0 through M9. The
normal `ntdos64-run` link response remains only
`CMakeFiles/ntdos64-run.dir/src/runner/ntdos64_run.c.obj`.
