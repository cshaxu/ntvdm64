# T200 S22 — native engine machine-stage closure

## Question

Can the native engine assemble the already admitted bx-vdm inputs into one
minimal Bochs machine preentry transaction, while retaining no raw Bochs
object, executing no guest instruction, and restoring the narrow NTIO
preserved range after the startup payload write?

## Design

`bx_ntvdm_machine_stage_v1` is mantle-owned and process-local. Its fixed-width
request has two copied selector-blind mechanical actions plus an opaque
preserve/restore range. It performs this exact order:

```text
initialize minimal machine
  -> external initial-state action
  -> copy preserved range
  -> NTIO startup-payload action
  -> restore preserved range
```

The machine remains mantle-private while the stage is active. The engine sees
only the status code, then unconditionally resets the stage and direct
composition. It has no CPU object, memory object, BOP identity, or guest
pointer.

## Inputs and procedure

First, the full current composition manifest was source-compiled (without
link or guest run):

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T198S94SourceBuiltNormalReturnNativeProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s22-current-composition-r2 `
  -CompileOnly
```

Then the current engine units were compiled and linked against that exact
object closure and the retained CPU5 native-core closure:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T200S22EngineMachineStageProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s22-engine-machine-stage-r1 `
  -CompositionBuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s22-current-composition-r2
```

The focused mantle transaction additionally passed independently at
`artifacts/build/t200-s22-machine-stage-r3`.

## Observations

The engine fixture passed two consecutive direct calls. Each call initialized
the CPU5 minimal machine and completed all mechanical work before cleanup.
The focused fixture verified that the startup payload remains at `0x700` while
the separately written four bytes at `0x714` survive its overlapping payload
write. The stage rejects a second begin while active and has no active machine
after reset.

All retained records report MSVC x64 `/MT`, `machineInitialized: true`,
`guestExecution: false`, and `cpuLoopEntered: false`.

## Boundary result and follow-up

This closes only the mechanical preentry stage. It does not apply the
real-mode entry delta, enter `cpu_loop`, observe a BOP, claim BIOS/POST
equivalence, or close COMMAND/DOS execution. The next lifecycle question is
how a mantle-private active stage may apply the already checked real-mode
entry contract before any bounded execution is admitted.
