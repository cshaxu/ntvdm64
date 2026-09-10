# M0 T225 S55 — full-package paired integration result 001

## Question

After rebuilding the complete current MSVC x64 `/MT`, CPU5/P-MMX module graph,
does the existing guest-startup/EXEC/parent-return package exhibit a
Direct-versus-Readonly difference before a guest child lifecycle is reached?

## Inputs and procedure

`tools/build/New-T225S7FullNinjaGraph.ps1` generated the fresh disposable
root `build/M0-T225-S55/lifecycle-r1` from the tracked full-module manifest.
Ninja built all 346 declared outputs, including `ntdos64-native.exe`.
`tools/build/Stage-T225S51GuestExecIntegrationInputs.ps1` staged the locked,
original-toolchain source-built NTIO, NTDOS, COMMAND and SHARE-as-TARGET inputs
below `build/M0-T225-S55/inputs-r1`.

The native CLI ran once in each supported mode with the same profile and a
finite `--instruction-tick-budget 1000000`.  No BOP-sequence, first-fault,
instruction-history or other diagnostic option was enabled.

## Observations

| Mode | Exit | stdout SHA-256 | stderr SHA-256 | stderr bytes |
| --- | ---: | --- | --- | ---: |
| Direct | 4 | `6442c836088d95943d52e506e84895fecddfad88647e8d324da1979f592637f8` | `8c6d118ec8d2fada9ebcbc3e89c5da6effa8f271da8a41c2e016aaeaa4758e8c` | 489935 |
| Readonly | 4 | `6442c836088d95943d52e506e84895fecddfad88647e8d324da1979f592637f8` | `8c6d118ec8d2fada9ebcbc3e89c5da6effa8f271da8a41c2e016aaeaa4758e8c` | 489935 |

The final stderr lines in both runs were repeated original Bochs
`read_virtual_checks(): read beyond limit` diagnostics.  There was no CLI
ordinary completion or native proof of child entry, `50:3C`, or `54:0B`.

## Interpretation and follow-up

The two supported mutation modes are byte-identical for this package run; the
result neither selects a host-capability repair nor attributes a new BOP
failure.  The earlier Direct-only 10M attempt was stopped by its wall-clock
watchdog and is deliberately not retained as evidence.

The next S55 analysis remains the complete original bootstrap/device-header
lifecycle: NTIO resident state, NTDOS temporary relocation, `DosInit`,
`charinit`, and `DEVIOCALL2`.  The static candidate described in T225 S31 is
not a dynamic identity proof and does not authorize a CPU wrap, synthetic CON,
prewritten guest state, or leaf BOP repair.