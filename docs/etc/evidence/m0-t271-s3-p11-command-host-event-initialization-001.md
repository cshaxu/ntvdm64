# M0 T271 S3 P11 — COMMAND host-event initialization ownership

`cmdmisc.c` calls the original host symbol `nt_init_event_thread`.  The
previous composition-local implementation only marked a COMMAND session
field, incorrectly making host-event state a BOP component concern.

The same-named implementation now belongs to the imported
`opennt-host/softpc.new/host/src/nt_event.c` subset.  It records the admitted
one-session initialization request in the private host event state record;
`runtime_command_misc_session` no longer carries this host-console flag.  The
COMMAND header retains only the source-shaped function declaration.

This is covered by the existing `HOST-DIV-030`: the historical VDD, display,
BIOS keyboard, console window, device, timer and process-exit portions of the
thread bootstrap remain deferred.  The new body does not represent any of
those actions as successful.

After an outside-sandbox formal Ninja relink in `build/M0-T271-S3/r003`,
`t231-s4-command-console-keyboard-direct-fixture.exe` exited zero.  It proves
the original COMMAND call observes `console_initialized` through the
opennt-host event snapshot and retains the console, keyboard fallback/success
and standard-handle-token checks.
