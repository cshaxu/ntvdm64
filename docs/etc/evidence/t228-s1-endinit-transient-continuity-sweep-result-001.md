# T228 S1 P17 — EndInit-to-Transient Bounded Continuity Sweep Result

## Question

After the controlled-large environment reaches the two map-supported original
COMMAND `EndInit` `54:0F` calls, does it reach a stable child lifecycle, or
can the following dynamic-transient control state be bounded without selecting
a BOP or Bochs repair?

## Fixed Inputs And Observation Boundary

All three Direct runs used the existing P2/P11 diagnostic executable
`build/M0-T228-S1/cs-transition-ring-20260818a/bin/ntdos64-native.exe`
(SHA-256 `f9d66c5498b68f63ba41692822c5759b447ecf63d209e8c150e214f07f0fe9ff`),
the immutable target `TARGET.EXE` (SHA-256
`69dabbdb754b358ac4fe4b22de04c0e4c93076816f14bb0730caa9fd223996fc`), and
the profile (SHA-256
`b86483ac225bc3a1e5a1c86cbe2099735ac790656eec60d165cefa30a317835b`).

The child environment was cleared and then contained only `SystemRoot` (10),
`ComSpec` (27), `NTDOS64_STARTUP_CONFIG_SOURCE` (85),
`NTDOS64_STARTUP_AUTOEXEC_SOURCE` (87), and `OPAQUE_A` through `OPAQUE_D`
(each 1,000 UTF-16 characters).  It is therefore the P13 controlled-large
size form, not an ambient host environment.

The runs used exactly the admitted budgets 300,000, 1,000,000, and
10,000,000, with BOP sequence, lifecycle ledger, budget terminal position,
fixed-width instruction history, and fixed-width CS-transition observation.
They did not enable provenance, read guest RAM, apply a source/opcode filter,
change a guest input, add a BOP provider, or change CPU semantics.  The full
ledger and byte-addressable log hashes are retained in the disposable
`build/M0-T228-S1/endinit-continuity-sweep-20260818b/ledger.json`.

P15's exact-image COMMAND map places the first two observed `54:0F` sites at
linked `EndInit` offsets `03a6h` and `03cbh`; P16 remains the segment-only
NTDOS allocation-map disposition.

## Results

| Budget | `54:0F` observations | Lifecycle | Terminal / latest retained transition |
| --- | --- | --- | --- |
| 300,000 | two: `0C41:03A6` then `0C41:03CB`, second `EBX=010F` | parent-start only | `0C41:1B25`; latest transition `074A:068B -> 0C41:1B0F` |
| 1,000,000 | the same two plus a third at `0C41:03A6`, with `EBX=4444` | parent-start only | `4444:AB40`; latest transition `0C41:1DC6 -> 4444:4444` |
| 10,000,000 | same three observations as 1,000,000 | parent-start only | `4444:53C0`; the same last retained transition `0C41:1DC6 -> 4444:4444` |

Every run exits `4` through the declared execution-budget terminal.  The
stderr streams are byte-identical reset-only Bochs output.  No child-entry,
normal `4Ch`, or `50:3C` lifecycle record appears at any budget.

## Interpretation And Next Boundary

The map-supported pair of original `EndInit` environment calls is not itself
the terminal: by 300,000 ticks the execution has resumed into the original
COMMAND `0C41` region.  Between that state and the one-million bound, an
additional same-site `54:0F` is reached with the patterned `4444` register
state, followed by the scalar dynamic transition to `4444:4444`.  Extending
the unchanged run tenfold does not yield another lifecycle event or a stable
continuation.

This is a bounded state classification, not an instruction-level cause claim.
The diagnostics copy no bytes and the NTDOS map exposes no private allocator
symbol.  Accordingly, it neither identifies a specific COMMAND nor NTDOS
instruction, nor authorizes a BOP leaf implementation, environment
truncation, synthetic guest write, or address-specific bx-core/bx-mantle
workaround.  The only repair candidate remains the complete original COMMAND
`EndInit` / NTDOS MCB allocation-and-transient-placement owner package, to be
admitted with whole-package source invariants and regression.
