# M0 T310 S20 P3 — console-input contract result

## Question

Does the recovered public Win32 console boundary preserve the selected
source-shaped failure directions without pretending that an interactive
console, menu or fullscreen product shell is available?

## Procedure

Generated `build/M0-T310/S20/host-input-contract-x86` with
`New-T310HostInputNinja.ps1`, then ran its single focused fixture.  It calls:

- `ReadConsoleInputExW` with an invalid flag, expecting
  `ERROR_INVALID_PARAMETER` before any console endpoint use;
- `VDMConsoleOperation` with an unadmitted selector, expecting
  `ERROR_CALL_NOT_IMPLEMENTED`; and
- `ShowConsoleCursor` with an invalid output handle, expecting the original
  negative failure sign.

It also ensures that the original cdecl `GetConsoleInputWaitHandle` entry
returns a non-null process input endpoint in the test process.

## Observation

The fresh Win32/x86 `/MT` Ninja graph compiled four edges and printed:

```text
PASS: console input compatibility failure contracts
```

## Interpretation

The test proves only the binding's local failure contract.  It neither
injects keyboard input nor claims a window/fullscreen implementation.  Those
would require integrated controller/device execution and remain mandatory S49
or S27 work respectively.
