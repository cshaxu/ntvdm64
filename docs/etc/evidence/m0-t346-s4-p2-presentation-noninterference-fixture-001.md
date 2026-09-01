# M0 T346 S4 P2 — Presentation-window bounded noninterference fixture

## Scope

This record verifies the selected app presentation lifecycle's local session
invariant.  It is not a guest-startup, DOS graphics, controller or general
runtime-continuity result.

## Source facts

- `src/app/entry.c` prepares and opens the optional app window while the
  session is `READY`; the original SoftPC entry is invoked only later through
  `app_machine_shell_run`.
- `src/app/presentation_window.c` owns a separate public window thread.  Its
  only input endpoint is a separately opened `CONIN$` handle; repainting is
  requested through a session video-event callback and `PostMessageW`.
- The window source neither invokes `mvdm_softpc_execution_run_original_entry`
  nor changes the selected machine backend.  CPU, BOP and controller state
  remain outside the app window ABI.

## Verification

The existing `tests/app/presentation_window_fixture.c` now selects
`SESSION_MACHINE_BACKEND_SOFTPC` before preparing the window, then verifies
the same selection both after the window becomes active and after it closes.
It also retains the existing active-session and dispose checks.

The disposable `build/M0-T346` verification used the generated formal x86
MSVC launcher, then compiled, linked and ran that fixture against the current
`app-machine-shell.lib`, `softpc-win32-bindings.lib` and `session.lib`:

```text
run-ninja-parallel.cmd original-softpc-process.exe
ninja: no work to do.
presentation_window_fixture.c
exit 0
```

## Result and limitation

The bounded lifecycle preserves the selected SoftPC backend and active session
state.  This removes the app-window lifecycle itself as an untested mutation
of those two composition values.  It does **not** establish that an original
guest reaches a graphics mode, that the earlier fixed startup timeout is
caused by or independent of every host condition, or that graphics repaint
has occurred from a DOS workload.  Those remain T346/S4's unmet runtime
criteria.
