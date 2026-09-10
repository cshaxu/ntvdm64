# NT4 Bootstrap Transaction Work Order

Status: M7 implementation work order, 2026-08-08. This work composes two
already private image plans. It does not make either plan executable.

## Evidence Correction

The implementation fixture originally used a synthetic `0x83E0`-byte NTIO
array and a matching `DI=08AEh` NTDOS location. Those numbers are valid test
data for adjacency and overlap checking only; they are not a fixed NT4
profile fact.

The fixed historical toolchain artifact is `0x8400` bytes. Its linker-derived
`dos_load_seg` is currently `08B0h`, placing the first NTDOS byte at `0x8B00`
when NTIO begins at physical `0x700`. The source says `mov di,dos_load_seg`,
so a future profile must derive the value from the admitted artifact layout,
not from either fixture constant. See
`SVC-DEMLOADDOS-HISTORICAL-CONTRACT.md`.

## Outcome

Create an immutable-by-convention transaction view with exactly two ordered
preloads. It revalidates both complete ranges against one supplied ordinary
RAM capacity, rejects physical overlap using widened arithmetic, and borrows
the already private buffers without copying again.

This transaction is a testable memory-description primitive, not a historical
initial-entry preload. The original request occurs only after NTIO executes;
a future mantle applies the NTDOS half at the stopped transition boundary.

## Required Tests

1. Synthetic compatible NTIO-sized and NTDOS plans compose into ordered
   adjacent views at caller-provided locations.
2. An overlapping DI target rejects without changing an existing empty output.
3. An inadequate supplied RAM capacity rejects even if each input plan was
   previously constructed.
4. No core, host capability, historical object, or normal CLI dependency is
   added.

## Implementation Evidence

On 2026-08-08, `nt4_bootstrap_transaction.[ch]` and the default-disabled
`nt4-bootstrap-transaction-test` target were added. The fixture uses owned
buffers only. It proves adjacency, borrowed private-buffer ownership, overlap
rejection, and capacity revalidation; it does not claim to execute the
historical loader.

The recorded CTest run passed M0, M1, M2, both image planners, and this
transaction fixture. `ntdos64-run` rebuilt unchanged afterwards.
