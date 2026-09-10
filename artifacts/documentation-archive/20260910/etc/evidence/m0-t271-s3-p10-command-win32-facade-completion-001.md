# M0 T271 S3 P10 — residual COMMAND Win32 facade completion

Two remaining `opennt_command_composition.c` helpers were not COMMAND
provider logic.  Both were public/obsolete Win32 compatibility work and now
belong to the existing `adapter-win32` COMMAND facade:

- `runtime_opennt_command_environment_variable` retains normal
  `GetEnvironmentVariableA` behavior and the OpenNT hidden `=X:` fallback
  through public `GetCurrentDirectoryA`.
- `runtime_opennt_console_keyboard_layout_name` replaces the unavailable
  private `GetConsoleKeyboardLayoutNameA` export with public
  `GetKeyboardLayoutNameA`, retaining `cmdkeyb.c`'s caller-side algorithm.
- The existing error-dialog facade now directly exports the original
  `RcErrorDialogBox(UINT, CHAR *, CHAR *)` spelling.  This preserves
  `error.h` declarations and lets the source bodies call it without a
  COMMAND-local forwarding wrapper.

The original COMMAND-facing macro spellings remain in the composition header;
only their declaration target changed.  The two divergences are registered as
`WIN32-DIV-010` and `WIN32-DIV-011` in `adapter-win32/README.md`.

Outside the sandbox, formal Ninja root `build/M0-T271-S3/r003` rebuilt the
affected closure.  Both focused executables exited zero:

- `t231-s4-command-console-keyboard-direct-fixture.exe` reported its console,
  keyboard fallback/success and standard-handle-token contract verified.
- `t234-s2-command-dynamic-environment-fixture.exe` reported its OpenNT
  dynamic COMMAND environment composition verified.
- `t231-s5-command-config-direct-fixture.exe` reported its direct OpenNT
  `cmdconf` config, autoexec and terminal-failure paths verified.

The remaining composition code is not relabeled as Win32 facade work: its
call-scoped CCPU/SAS facade, source-specific copied `GetVDMAddr` spans,
COMMAND worker callback, Redirector token boundary and typed terminal path
need separate owner disposition.
