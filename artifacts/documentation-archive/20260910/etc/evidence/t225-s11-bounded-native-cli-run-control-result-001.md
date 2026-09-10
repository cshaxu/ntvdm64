# T225 S11 Bounded Native CLI Run-Control Result

## Question

Can the native CLI select the existing engine/mantle instruction-tick watchdog
without changing its ABI, guest image, BOP semantics, or Bochs mechanics, so
that Direct and Readonly runs return a normal classified terminal result?

## Change

src/cli/ntdos64_native_cli.c now accepts one optional argument:

    --instruction-tick-budget positive-decimal

The parser accepts only decimal digits representing an unsigned nonzero
64-bit value. Zero, signs, non-digits, duplicate occurrences, and overflow
are rejected by the existing usage path. When absent, the original
one-million-tick default remains unchanged.

The parsed value is assigned to the existing lifecycle policy and then to the
existing bx_ntvdm_engine_request_v1 instruction_tick_budget field. No engine,
machine-stage, adapter, profile or Bochs interface changed. The existing
validate-only and terminal lines now print the copied budget for reproducible
evidence.

## Source-recovery disposition

This is modern CLI run control, not recovered OpenNT behavior.

| Recovery rung | Disposition |
| --- | --- |
| Original OpenNT source | Not applicable: historical NTVDM does not own the modern x64 CLI's finite-run argument grammar. |
| Smallest existing seam | Selected: preserve the existing engine/mantle fixed-width budget field and expose it at the only CLI parser that constructs the request. |
| External-code intrusion | Rejected: bx-core/Bochs CPU, decoder, memory, device and firmware sources are unchanged. |
| Newly authored behavior | Limited to decimal CLI parsing and diagnostic rendering; it does not define a guest, BOP, DEM, COMMAND or host-capability contract. |

## Build

A fresh disposable formal graph was generated at
build/M0-T225-S11/ninja-r1 and built with Ninja/MSVC x64 /MT, CPU5/Pentium-MMX.
The real linked executable is:

- build/M0-T225-S11/ninja-r1/bin/ntdos64-native.exe
- size: 2,342,400 bytes.

The final Ninja dry run reports no work.

An initial disposable wrapper error was corrected before acceptance: the
single-environment wrapper must forward its command with percent-star. The
accepted build has real objects, archives and the linked executable; it is not
the earlier no-op log-only attempt.

## Validation

All calls used the source-built S10 primary input root and the same profile.

| Case | Result |
| --- | --- |
| Direct validate-only, explicit 123 | accepted; output reports mode 1 and budget 123; exit 0. |
| Readonly validate-only, no argument | accepted; output reports mode 2 and budget 1000000; exit 0. |
| Zero budget | usage rejection; exit 2. |
| 18446744073709551616 | overflow/usage rejection; exit 2. |
| Overlay mode | parser rejection; exit 2. |
| Direct, budget 1000 | normal output terminal=4 detail=0 lifecycle=5 presentation=5 cancellation=0 budget=1000; exit 4. |
| Readonly, budget 1000 | identical normal budget terminal and exit 4. |

The two execution cases use the normal internal machine-stage timer, not the
S10 external process watchdog. Their common early reset diagnostics are
expected at this bounded point. A budget terminal does not claim that NTIO,
50:11, NTDOS, COMMAND or any BOP was reached.

## Boundary

S11 proves controlled return from the existing native run path. It does not
provide the missing continuity observation. The next package, if admitted,
must expose only a copied adapter-owned post-run observation of already
accepted BOP sequence facts or a selector-blind machine-stage fact, with a
separate source/ABI and lifecycle map. It must not combine that work with a
BOP provider implementation or guest semantic repair.
