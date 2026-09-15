# app

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
