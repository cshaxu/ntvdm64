# M0 T271 S3 P19 — COMMAND app-session binding extraction

## Owner decision

The former `opennt-bop/ingress/command_runtime_session.{c,h}` contained no
OpenNT dispatcher or provider body.  It did all of the following before an
OpenNT COMMAND session was bound:

- copied app-selected application, command-tail, drive and code-page values;
- read app launch-environment configuration for CONFIG/AUTOEXEC input;
- selected an admitted PIF startup path; and
- reset/bound the single composed provider session during engine lifecycle.

Those are final product composition responsibilities.  They now live in
`app/command_session_binding.{c,h}`.  The public names are also changed from
the misleading `runtime_command_runtime_session_*` spelling to
`app_command_session_*`.

`opennt-bop` still owns the original `CMDInit`, `MS_bop_4`, COMMAND globals,
dispatcher table and provider calls.  The app binding only invokes the
declared native-session boundary after copied inputs have been prepared.  It
does not inspect BOP bytes, selector/service values, guest memory or CPU
state.

## Source changes

- The former files were moved by `git mv` to the app component.
- `startup_composition` no longer contains a misnamed DEM function which
  binds COMMAND state.
- `engine_run` invokes app-owned bind/reset functions.
- The formal component manifest compiles the source as `app`, not
  `opennt-bop`.

## Verification

Outside the sandbox, the formal graph was regenerated at
`build/M0-T271-S3/r003`.  Ninja compiled
`src/app/command_session_binding.c` into `lib/app.lib`, linked the final
`bin/ntvdm64.exe`, and linked both focused fixtures.

- `t231-s10-command-native-session-fixture.exe` exited zero and reported that
  the source `54:00` COMMAND session has no v1 fallback.
- `t234-s2-command-pif-direct-fixture.exe` exited zero and verified the
  original PIF parser's title, command, directory, target, close-on-exit and
  CONFIG/AUTOEXEC selection.

This move is ownership-only; it does not alter the original COMMAND dispatcher
or PIF parser contract.
