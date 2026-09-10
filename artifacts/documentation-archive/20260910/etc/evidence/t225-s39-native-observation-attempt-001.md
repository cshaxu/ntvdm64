# T225 S39 Native Observation Attempt 001

## Question

Can the version-2 default-off, selector-blind BOP snapshot observer complete the
single required paired Direct/Readonly native observation without changing
normal execution behavior?

## Inputs

- Formal MSVC x64 `/MT`, CPU5/P-MMX Ninja graph in
  `build/M0-T225-S39/snapshot-r1/`.
- Immutable S10 bootstrap inputs in
  `build/M0-T225-S10/primary-bootstrap-inputs-r2/`.
- Target: `TARGET.EXE`; mode-specific command lines use the same profile,
  root, startup configuration, startup autoexec and one-million instruction
  tick-budget request.
- The version-2 snapshot changes in `src/bx-vdm/` and `src/cli/`; no provider,
  guest, profile, bx-core or bx-mantle semantic change.

## Procedure

1. Formal native closure built successfully: 206/206, including `bx-vdm.lib`
   and `bin/ntdos64-native.exe`.
2. The focused decline fixture was rebuilt and returned success. Its extension
   records the fixed-width snapshot, disabled behavior and the 129th-event
   overflow rule without changing the declining outcome.
3. The boundary script passed: the observer is absent from bx-core/bx-mantle
   and the v2 ABI/copy/CLI presentation fragments are present.
4. One Direct run was started with `--observe-bop-sequence`; the intended
   sequential Readonly run was part of the same command only after Direct
   return.

## Observations

- Direct did not reach its normal observer print/return path. It repeatedly
  emitted `read_virtual_checks(): read beyond limit` after a requested
  1,000,000-tick budget. Its log counter reached `00141924600` before the
  process was stopped to prevent unbounded disposable-log growth.
- Direct raw log: 90,562,437 bytes, SHA-256
  `848C0B9064F877F312D782E308B3D327C6611C5EF142D8720BA01923773EAD82`.
- Stopping Direct caused the outer sequential command to start Readonly. It
  showed the same repeated diagnostic and was immediately stopped; it did not
  represent an independent completed paired observation.
- Readonly raw log: 18,517,507 bytes, SHA-256
  `79A862B88E587304E60F4461E174448E44F6FC929F1512951E8C2556BB56E00B`.
- No `bop-sequence` presentation was emitted by either interrupted run, so
  this attempt provides no reached `50:36` snapshot and no Direct/Readonly
  parity conclusion.

## Interpretation and confidence

High confidence that the v2 recorder compiles in the formal native closure
and that its focused fixed-capacity and boundary contracts hold. High
confidence that this particular native invocation failed to converge under
its existing tick-budget contract. The failure is outside the recorder's
selector-blind recording contract: no snapshot semantic, provider, guest,
profile or Bochs repair is admitted from it.

## Follow-up

Keep M0 T225 S39 active. Before another native observation, identify the
existing lifecycle/tick-budget path that permitted the repeated limit-read
loop and re-admit any correction separately. Do not create a selector-specific
BOP change or infer COMMAND/NTDOS continuity from this interrupted attempt.