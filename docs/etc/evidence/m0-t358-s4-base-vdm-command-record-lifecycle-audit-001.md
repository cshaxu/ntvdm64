# M0 T358 S4 — Base VDM command-record lifecycle audit

## Question

Did the declared `--command EXIT` record reach and get consumed by the
original COMMAND `54:01` acquisition path, or is the observed second `54:01`
wait a Base VDM delivery defect?

## Original chain and current binding

The console-owning observer starts the product with `-f -o --command EXIT`.
`app_launch_declaration_consume_options` retains that explicit app input;
`app_launch_declaration_publish` constructs the source-shaped record:

```text
application = <SystemRoot>\system32\COMMAND.COM
command     = /C EXIT\r\n\0
environment = COMSPEC=<application>\0PATH=<SystemRoot>\0
directory   = <SystemRoot>
```

It publishes the record to `base_vdm_broker` and immediately delivers it to
the bound `base_vdm_local` destination before original SoftPC startup.

The reached original configuration PIF probe in
`mvdm-host/softpc.new/host/src/config.c` uses `ASKING_FOR_PIF`.  The selected
local Base VDM slice explicitly returns its original-shaped unavailable PIF
result before it inspects or clears the DOS command record.  It cannot consume
the declaration.

The first original `cmdGetNextCmd` in
`mvdm-host/dos/command/cmdmisc.c` sets
`ASKING_FOR_FIRST_COMMAND | ASKING_FOR_DOS_BINARY` and calls the original
BaseClient spelling `GetNextVDMCommand`.  `base_vdm_local.c::get_next_command`
then copies the record into the caller buffers, clears `record->available`,
and returns success.  This preserves the original source ownership: COMMAND
forms `VDMINFO`; BaseClient performs its capture/wait/retry boundary; BaseSrv
selects/copies the record.

## Interpretation of the second request

After the first record is copied, `record->available == 0`.  A later DOS
`GetNextVDMCommand` therefore follows the original empty-queue branch:
`STATUS_PENDING` plus the session-local wait event, and the BaseClient waits
without retaining a guest or caller pointer.  The observed
`base_vdm_local_wait_for_command` frame is consequently proof that the first
record was already consumed, not proof that it was never delivered.

The current single declaration is intentionally one-shot.  No second command
producer is admitted, so a second normal command request must wait.

## Disposition

Base VDM `/C` record delivery is complete for the bounded one-session profile.
No BaseSrv/CSR reconstruction, BOP repair, guest rewrite, or command producer
change is selected.  The remaining question is instead the original guest
COMMAND.COM `/C EXIT` interpretation and exit/return path between the first
successful `54:01` return and the later command acquisition.  That is the
next whole owner cohort inside T358; it must begin with a guest/host control
flow audit before any new run or code change.
