# T228 S1 P2 terminal instruction-history result

## Question

Can the immutable Direct/Readonly terminal be observed without BOP recognition, guest-RAM provenance, or CPU semantic change?

## Inputs and procedure

Fresh current-source CPU5 MSVC x64 `/MT` Ninja graphs were generated with `New-T225S7FullNinjaGraph.ps1`. The default graph completed 315 targets and rejects the diagnostic CLI option. The derivative history graph completed 316 targets with `BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY=1`; provenance was not enabled. The existing focused fixture passed in both graphs. `Invoke-T228S1GuestExecIntegration.ps1` then ran the hash-locked T228 input manifest `c888239b9de415f5c5f38b8d8dcacf5a30c102ab1465189c1ffc1a97fb1b9b6a` at 100000000 ticks in Direct and Readonly.

## Observation

Both runs exited 4, with identical stdout SHA-256 `2a7b97702ee40fa9ff7fdd4332f9a143b7a453ea6699e5467c652648bcd36eb7` and identical stderr SHA-256 `905ddf181613eb9b79dfeec0a507def81b0c26ffb1a1518ce10e4a19b0166406`. Each preserved the sole phase-1 `50:36` ledger entry and terminal `5A5F:9F51`.

The copied 16-record scalar ring is ordered from `5A5F:9F33` through `5A5F:9F51`, in two-byte increments; every record has `SS=0C41`, `SP=FDCB`. It contains no guest bytes, selector/service identity, decoded opcode, or provenance.

## Interpretation

This is a bounded runtime control-flow observation, not evidence for a BOP leaf repair. Direct/Readonly parity excludes mutation profile at this boundary. T228 must next classify the complete guest/control-flow or bx-core/bx-mantle continuity owner domain before any repair is admitted.