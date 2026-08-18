# T228 S1 P23 — Real/V86 IRET Completion Observation Result

## Result

The registered `BX-CORE-089` mechanical diagnostic profile is implemented and
locally closed.  It records a fixed copied post-return state only after the
original `IRET16` or `IRET32` real/V86 path has completed its existing
`RSP_COMMIT`.  It neither identifies nor changes an interrupt, BOP, OpenNT,
DOS, service, guest address or guest byte.

## Implemented boundary

- `src/bx-core/cpu/ctrl_xfer16.cc` invokes the default-off record macro only
  after successful real/V86 `IRET16` state commit.
- `src/bx-core/cpu/ctrl_xfer32.cc` does the same for `IRET32`.
- `src/bx-mantle/bx_ntvdm_interrupt_return_observation_v1.*` owns versioned
  copied storage, opt-in capacity, reset, chronological wrap and copied query.
- The formal generator accepts `-InterruptReturnDiagnostic`; without it, the
  macro is defined as a no-op and the default target carries no active record.

The source review confirms the only macro call sites are the two IRET functions
and occur after `RSP_COMMIT`.  The code contains no BOP, OpenNT, DOS, SoftPC,
service, vector, selector or address predicate, no guest-memory/stack-frame
read, no adapter callback and no execution/result mutation.

## Focused diagnostic closure

Fresh CPU5/Pentium-MMX MSVC x64 `/MT` diagnostic graph:

- Root: `build/M0-T228-S1/p23-iret-observation-20260818b`.
- Generator: `New-T225S7FullNinjaGraph.ps1 -InterruptReturnDiagnostic`.
- Manifest SHA-256:
  `47fdf1a8d4af74f14499cdc0bf8582519a2c3ba2fe0ab480b146fe96e92f33b4`.
- Focused fixture SHA-256:
  `e4931479017429eb9ffac975490d60df915ac3cb561356f33bb269b4035fbdaf`.
- `t228-s1-interrupt-return-observation-fixture.exe` exited `0`.

The fixture proves: disabled capacity records nothing while ordinary IRET16
still reaches its UD2 controlled stop; enabled real-mode IRET16 records the
post-return `0:0108` state with AX=`1234h`, BX=`5678h` and width 16; enabled
real-mode operand-size IRET32 records post-return `0:0200` with the same
register values and width 32; protected-mode IRET returns normally but records
nothing; copied ring wrap and reset retain chronology.

## Default graph closure

Fresh default (no diagnostic option) CPU5/Pentium-MMX MSVC x64 `/MT` graph:

- Root: `build/M0-T228-S1/p23-default-formal-20260818a`.
- `bin/ntdos64-native.exe` linked successfully.
- Manifest SHA-256:
  `72ffa3b3be7a6bb902a539d401b6357ed5b55587e42c71a001e05bfb775b7916`.
- Native target SHA-256:
  `aec18047a5e3ef402eb9eea1db01a19177ca030f5f694a04a64cf78fad1b2086`.

The default graph was compiled without
`BX_NTVDM_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION`; the focused diagnostic
fixture is intentionally not run in that graph because zero records are its
expected disabled behavior.

## V86 disposition and limitation

Both original IRET functions call the same record macro after their existing
V86 helper (`iret16_stack_return_from_v86` or
`iret32_stack_return_from_v86`) returns and the shared `RSP_COMMIT` succeeds.
The fixed bare-machine fixture has no admitted V86 entry setup, so this P has
source-path coverage rather than a dynamic V86 witness.  It does not claim V86
runtime integration.  A future complete owner-domain observation may exercise
that existing path if it is reached; no separate V86 setup or CPU change is
selected here.

## Conclusion

This profile is diagnostic-only.  It permits one later bounded immutable
Direct/Readonly observation to correlate mechanical post-IRET facts with the
already established whole COMMAND/NTDOS owner map.  It selects no BOP leaf,
adapter provider or CPU compatibility repair.