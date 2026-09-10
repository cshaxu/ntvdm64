# T200 S24 — bounded native execution closure

## Question

Can the active, already armed mantle machine execute guest instructions only
under a finite watchdog and return a typed terminal result while the engine
unconditionally releases machine and composition state?

## Contract

The mantle stage owns the execution record (`ips` and nonzero instruction
budget), initializes Bochs PC timing, registers a one-shot native timer, and
enters `cpu_loop`. It returns only one of:

- finite watchdog budget reached;
- accepted generic typed STOP; or
- explicit inactive/input/timer/unexpected-loop failure.

It deactivates and unregisters the timer after loop return. The engine maps
only budget and accepted STOP to its public terminal kinds, then resets stage
and composition before returning. It contains no selector/service decision.

## Focused mechanical verification

The stage fixture writes one `HLT` at the already armed entry and uses a
one-million-tick watchdog. It returned `EXECUTION_BUDGET` and reset cleanly:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T200S22MachineStageProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s24-bounded-stage-r3
```

This is a finite-mechanics witness, not a DOS guest test. A neutral `JMP -2`
probe in the same minimal closure instead raised Windows stack-overflow
`0xC00000FD`; it is retained as a separate mechanical-risk observation and
was not masked with a service/BOP change. The admitted HLT path is the same
native bounded-stop shape already used by the retained finite-run evidence.

## Source-built profile execution

The current composition manifest was source-compiled at
`artifacts/build/t200-s24-current-composition-r1`. The native engine was then
linked with that closure and run twice:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T200S22EngineMachineStageProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s24-engine-native-r3 `
  -CompositionBuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s24-current-composition-r1
```

Both runs reported `terminal=5, detail=1`: engine
`CONTROLLED_GUEST_TERMINAL`, backed by the mantle's accepted generic
`CONTROLLED_STOP`. The log reaches 429874 native ticks before its second
fresh reset. There was no raw #UD, crash, device enablement, BOP patch, or
unbounded run.

## Interpretation and next step

This is the first source-built profile execution through the new engine
lifecycle, but it is only a controlled terminal observation. It does not
identify the terminal selector/service, prove a command lifecycle, or close
NTIO/DOS. The next trace-only package must capture and source-map that typed
terminal event before any component-owned repair is considered.
