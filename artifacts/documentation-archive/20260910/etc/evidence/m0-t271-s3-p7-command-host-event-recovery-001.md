# M0 T271 S3 P7 — COMMAND reached host-event recovery

## Source and ownership

The reached COMMAND provider calls four historical host functions:

- `nt_std_handle_notification` from OpenNT
  `base/mvdm/softpc.new/host/src/nt_msscs.c:1067`;
- `nt_block_event_thread` from `nt_event.c:1364`;
- `nt_resume_event_thread` from `nt_event.c:1501`; and
- `cmdPushExitInConsoleBuffer` from `nt_event.c:1879`.

They are OpenNT host APIs, not COMMAND dispatcher/provider algorithms. P7
therefore moves their same-named reached source subsets and the original
`CntrlHandlerState` ownership to `opennt-host/softpc.new/host/`.  The imported
COMMAND callers retain their original names and call order.

## Disposition

The active subset preserves redirection notification, event block reason,
`CNTRL_VDMBLOCKED`, `CNTRL_PUSHEXIT`, and resume ordering. `opennt-host` owns
the explicit state observation used by lifecycle tests.

The rest of the source bodies are intentionally not represented as success:
the historical console event thread, VDD hooks, graphics/mouse buffers, BIOS
keyboard state, timers, device closure, console-window messaging,
process-group closing and `ExitThread` each require a separate display/VDD or
console/process owner. They are registered as `HOST-DIV-029` and
`HOST-DIV-030` with local `DIVERGENCE:` markers.

## Verification

Formal Ninja graph `build/M0-T271-S3/r002` rebuilt `opennt-host` and linked
the reached COMMAND route. The following focused executables exited zero
outside the sandbox after the final private-overlay convergence:

- `t236-s1-command-local-child-fixture.exe`, including explicit
  `CNTRL_PUSHEXIT` / recorded-exit coverage, redirection notification and
  block/resume state;
- `t234-s2-command-dynamic-environment-fixture.exe`; and
- `t231-s10-command-native-session-fixture.exe`.

The independent `t231-s7-command-get-next-direct-fixture.exe` relinked with
the final graph, but its direct invocation retained a pre-existing fixture
stall and one exact process was terminated. It is not used as P7 acceptance
evidence; this P changes no get-next provider logic. The local-child fixture
exercises all four newly recovered host entries on the active COMMAND path.
