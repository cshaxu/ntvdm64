# T225 S27 P2 — COMMAND Parent-Return Focused Result

## Question

Does the source-derived COMMAND parent-return record expose an opaque ordinary
completion fact only after a valid no-next-command return, while retaining the
existing declared-reentry behavior?

## Inputs

- S27/P1 source/ABI/failure map.
- The formal manifest `tools/build/t225-s7-full-module-manifest.json` with
  `t225-s27-command-parent-return-fixture` admitted as a `bx-vdm` + `cli`
  target.
- Fresh disposable root `build/M0-T225-S27/command-parent-return-r2`.

## Procedure

1. Generated the formal MSVC x64 `/MT`, CPU5/P-MMX Ninja graph:

   ```powershell
   powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T225S7FullNinjaGraph.ps1 `
     -RepositoryRoot . `
     -BuildRoot build/M0-T225-S27/command-parent-return-r2
   ```

2. Built `bin/t225-s27-command-parent-return-fixture.exe` through Ninja.
3. Ran the fixture, then ran `ninja -n` for the same target.

## Observations

- The fresh graph built all 164 required nodes and linked the fixture.
- The fixture exited `0` and printed:

  ```text
  bx-ntvdm COMMAND CMDINFO delivery, environment retry and terminal lifecycles verified
  ```

- It proves all of the following at the existing COMMAND boundary:

  | Case | Result |
  | --- | --- |
  | declared next command | CF set; no terminal record; opaque ordinary-completion query false |
  | exhausted declared plan | CF clear; AX low byte is DX low byte; one valid exhausted-plan terminal record; opaque query true |
  | duplicate return | rejected after the one valid return |
  | wrong service | rejected |
  | pre-delivery return | rejected |
  | environment retry | retains existing CF/AX capacity behavior |

- `ninja -C ... -n bin/t225-s27-command-parent-return-fixture.exe` reported
  `ninja: no work to do.`

## Interpretation

The new query does not manufacture a DOS result: it consumes only the
already-validated source-derived `cmdReturnExitCode` no-next record. It has no
Bochs, CPU, firmware, guest-memory, host-handle, host-child, or profile-mode
behavior. The formal full `bx-vdm` closure compiles with the change.

## Limitation and follow-up

This focused result does **not** yet prove the outer composition's
post-cleanup consumption of the parent-completion fact, its distinction from
an actual `54:00` top-level terminal in one composition fixture, Direct versus
Readonly composition parity, or bounded native observation. Those are the
remaining S27 family steps; no native trace is authorized by this record.