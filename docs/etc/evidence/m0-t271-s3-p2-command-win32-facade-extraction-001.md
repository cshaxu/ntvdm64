# M0 T271 S3 P2 — COMMAND same-shaped Win32 facade extraction

## Owner correction

The following source-shaped modern Win32 surfaces were project code inside
`opennt-bop/command/opennt_command_composition.c`, despite being neither BOP
dispatch nor COMMAND provider semantics:

- `GetDriveTypeOem`;
- `GetEnvironmentVariableOem` and `SetEnvironmentVariableOem`;
- the historical `GetSystemDirectory` compatibility entry and its narrow
  test directory override.

They now reside in `adapter-win32/facade/opennt_command_oem_facade.{c,h}`.
Their names and caller-visible contracts remain unchanged. The source mirror
continues to call those names through its declared compatibility header, while
the adapter alone uses public modern Win32 APIs.

## Verification

On 2026-08-25, outside the sandbox, refreshed formal graph
`build/M0-T271-S3/r001` compiled both changed libraries and the focused
fixtures. Each exited zero:

- `t231-s2-command-misc-direct-import-fixture.exe`;
- `t231-s3-command-misc-registration-fixture.exe`;
- `t231-s4-command-console-keyboard-direct-fixture.exe`;
- `t231-s10-command-native-session-fixture.exe`.

This P does not claim full COMMAND provider/lifecycle closure. In particular,
the remaining source-derived `GetNextVDMCommand`, child-worker and session
completion composition stays explicitly in S3 for owner-by-owner recovery.
