# app

Application composition is deliberately split by process role:

- `run16_entry.c` is the public CreateProcess-shaped CLI. It performs original
  Base classification/configuration, starts the broker if required, launches a
  suspended worker, registers it, then waits through the original parent
  completion route.
- `basesrv_entry.c` hosts the standalone BaseSrv transport. Original `srvvdm.c`
  remains the DOS/WOW record owner; this entry supplies only authenticated local
  RPC, resource attachment and modern Console membership observation.
- `worker_entry.c` is the pure `ntvdm.exe` worker. It starts one original SoftPC
  machine, connects to BaseClient transport and has no launcher, classifier,
  local command queue or private argv shadow.
- `package_layout.{c,h}` validates immutable worker package media.  Worker
  backend selection and entry dispatch have no separate app-shell abstraction:
  `worker_entry.c` makes the one selected SoftPC/session call directly.

`console_probe.c` and `console_query.{c,h}` form the bounded, private modern
Console membership observation used by BaseSrv. They do not assign command
records or redefine BaseSrv readiness.

The former single-process entry, launch declaration and local BaseVDM queue
were retired in T412 S6. The current product has one worker ABI (`ntvdm.exe`)
and no alternate local queue/provider.
