# M0 T381 S2 P1 — Native-child payload boundary

## Scope

This P records the first recovery action for the active original COMMAND
`cmdExec32` cohort.  It does not close S2, S3, S4, or T381.

The recovery adds `ADAPTER-SOFTPC-044`: a default-off observation at the
existing bounded session snapshot in `mvdm_command_native_child.c`.  The app
captures `MVDM_NATIVE_CHILD_REPORT_PATH` before original `cmdenv.c` reads the
inherited environment, removes it immediately, and the adapter can then copy
only a bounded command text tagged as `guest-tail`, `comspec`, or `worker`.
No guest pointer, environment block, handle, router, process launcher, or
original COMMAND control flow changes.

## Static and local verification

- The formal CPU40/x86 Ninja target
  `build/M0-T379/S2/formal-x86/original-softpc-process.exe` rebuilt and linked
  after the new adapter observer was included in `softpc-bindings.lib`.
- The regenerated focused x86 native-child lifecycle fixture at
  `build/M0-T381/S2/command-native-child-x86/command_native_child_fixture.exe`
  compiled, linked, and exited zero.  It continues to verify checked command,
  environment and stream snapshots, CR-to-NUL guest mutation, Base VDM
  re-entry accounting, cancellation and cleanup.
- This focused fixture is not an original guest `54:08` completion proof and
  is not presented as one.

## One fixed-container observation

The one permitted T381 fixed non-debug observation was made in `O:\ntvdm64`
with the declared DOS request `C:\Windows\System32\cmd.exe /c exit 37` and the
new default-off payload report selector.  It timed out at the existing
controlled timeout result `0x53504354` before `54:07`/`54:08`; the durable
BOP trace ends after a successful `54:0F` (`cmdGetInitEnvironment`) return.
No payload report file was created.

Therefore this run proves neither parameter loss nor a `cmdExec32` worker
failure.  Its exact predecessor is earlier original COMMAND initialisation
after `cmdGetInitEnvironment`, outside the `54:08` worker interval.  The
result is retained as a negative reachability observation, not used to justify
a BOP leaf patch or a second run.

## Current disposition

S2 remains active.  The source-owned COMMAND initial-environment-to-first
command continuation must be statically reconciled before the original
`cmdExec32` interval can be reached again.  A later run may only be admitted
through a revised fixed-observation contract; this P does not authorize a
retry.
