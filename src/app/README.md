# app

`version.h` is the sole shared application identity for all three executables.
`APP_VERSION` is `0.0.<admitted T number>` (currently `0.0.412`); S/P changes
do not independently change that version. `APP_PROTOCOL_VERSION` identifies
the incompatible RPC service contract (currently 2, matching service.idl 2.0).
Incompatible changes within a T must advance the protocol. Launcher and worker
link the same metadata-consuming RPC client; BaseSrv uses the same header.
Connect requires exact protocol and zero-padded application-version agreement
in both directions. Mismatch returns 1306 with a diagnostic, before any task
request; run16 must not retry it as an absent broker. Replace all three EXEs
together and end old resident processes before testing a new protocol.
If a created worker rejects the version before connecting, run16 observes its
process exit alongside the parent event and invokes original creation rollback;
it must not hang waiting for a task that never started. Ordinary guest results
still follow the original completion path.

Application composition is deliberately split by process role:

- `run16_entry.c` is the public CreateProcess-shaped CLI. It performs original
  Base classification/configuration, starts the broker if required, launches a
  suspended worker, registers it, then waits through the original parent
  completion route.
- `basesrv_entry.c` hosts the standalone BaseSrv transport. Original `srvvdm.c`
  remains the DOS/WOW record owner; this entry supplies only authenticated local
  RPC, resource attachment and modern Console membership observation.
- `ntvdm.c` is the pure `ntvdm.exe` worker entry. Its explicit finite
  standalone binding supplies worker-local BaseClient/session state before the
  unchanged original initialization sequence; it has no launcher, classifier,
  local command queue or private argv shadow.
- `package_layout.{c,h}` validates immutable worker package media.  Worker
  backend selection and entry dispatch have no separate app-shell abstraction:
  the finite binding is compiled with the worker and hands directly to
  `ntvdm.c`.

`console_probe.c` and `console_query.{c,h}` form the bounded, private modern
Console membership observation used by BaseSrv. They do not assign command
records or redefine BaseSrv readiness.

The former single-process entry, launch declaration and local BaseVDM queue
were retired in T412 S6. The current product has one worker ABI (`ntvdm.exe`)
and no alternate local queue/provider.
