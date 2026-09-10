# NTIO First Live IVT/BDA Dependency Ledger

Status: M8 source ledger, 2026-08-08. This is source evidence for the fixed
NT4 profile. It neither installs an IVT/BDA nor admits the old scaffold as a
runtime component.

## Method

Read the first `init` procedure in
`src/opennt/base/mvdm/dos/v86/doskrnl/bios/msinit.asm` and its data owner
`msbio1.asm`. Treat only guest reads that occur before the first
`SVC_DEMLOADDOS` request as first-live dependencies.

## Findings

| Order | Guest access | Source behavior | Required owner | Classification |
| --- | --- | --- | --- | --- |
| 1 | IVT entries `10h`, `15h`, `19h`, `1Bh` | `RomVectors` contains these four vector numbers; `init` copies each four-byte entry from `0000:(vector*4)` into NTIO data before changing any vector | mantle profile initial-state declaration | required profile input |
| 2 | IVT `15h` | `init` installs its own `Int15` target after saving old `15h` | NTIO guest code after entry; core only provides checked RAM write | guest-owned mutation |
| 3 | IVT `19h` | `init` installs its own `int19` target after saving old `19h` | NTIO guest code after entry; core only provides checked RAM write | guest-owned mutation |
| 4 | BDA byte `0040:0096` | test bit 4 to select original versus extended keyboard functions | mantle keyboard/profile declaration | required profile input |
| 5 | IVT/BDA writes | local initialization clears selected vector/data locations before later setup | NTIO guest code | guest-owned mutation |

`msbio1.asm` declares `RomVectors` in precisely this order: `10h`, `15h`,
`19h`, `1Bh`. This is not evidence that all four handlers must execute during
the prefix, but it proves their incoming vector values are consumed and saved.

## Layer Decision

- **Core:** ordinary checked low RAM and atomic entry-plan preloads. It must
  not know these vector numbers or keyboard bits.
- **Mantle profile:** versioned initial-state data supplies the four incoming
  IVT values and BDA byte, with provenance, as part of the same frozen
  transaction that supplies image preloads.
- **Profile firmware/device provider:** later owns any behavior reached through
  those vectors or keyboard policy. It is not admitted merely because NTIO
  saved a vector.
- **NTDOS adapter:** selects the fixed profile and rejects missing or
  unproven initial-state fields. It does not zero-fill or synthesize them.

## Explicit Rejection

`v86/scaffold/i386/fakeinit.c` is useful source evidence that a historical
interim product supplied INT 11h, INT 12h, INT 19h and BDA equipment/memory
values. It is not a valid default for this route: it contains a V86-era
scaffold and an unimplemented-interrupt service encoding. The prior scaffold
trace remains negative evidence only.

## Next Gate

M9 must define a typed mantle initial-state descriptor for these five inputs,
including “absent/unproven” representation and redacted diagnostics. It may
not choose values or implement any vector handler until a profile source or
bounded trace supplies them.
