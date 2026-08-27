# app

CLI and final composition component. Existing app code is recovery evidence
until its owner/dependency review; provider algorithms do not belong here.

## M0 T273 S3 minimal machine shell

`machine_shell.{c,h}` is the admitted minimum app assembly: a caller-owned
`session` owns teardown, while the shell opens and closes the opaque
`adapter-bochs` machine lifecycle. It does not load a guest, select a BOP,
provide an OpenNT service, expose a Bochs type, or create a worker/scheduler.

## M0 T282 S6 local command source

`command_source.{c,h}` owns the copied, local single-session producer record
used by `GetNextVDMCommand`. It retains no caller `VDMINFO` pointer, Windows
handle, CSR record or guest address. The record is intentionally consumed only
through the same-shaped `adapter-vdm-monitor` entry while a session is bound.

### Registered divergences

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| APP-002 | BaseSrv owned command records, console discovery and a wait/retry transport for `GetNextVDMCommand`. | Public modern Windows has no composable BaseSrv/CSR command service. | App owns one copied command record and its source scalars; `session` only binds a synchronous dispatch. The source's terminal `RETURN_ON_NO_COMMAND` + second-call state retains `STATUS_NO_MEMORY`; first-VDM, wait, WOW and PIF remain unavailable. | `command_source.h`, `command_source.c` |
