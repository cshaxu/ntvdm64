# M0 T412 S5 — ExitVDM and empty-broker lifecycle checkpoint

## Scope and result

This checkpoint restores the omitted `BasepExitVDM` client crossing and
implements only the safe product-level **empty broker** grace policy.  It does
not claim S5 or T412 closure: no source-proven worker-ready/completed state is
yet available for retiring a resident worker, so a quiet interactive
`COMMAND.COM` or `EDIT.COM` is deliberately never reaped.

## Original-owner recovery

OpenNT's `base/win32/client/vdm.c:ExitVDM` selects its original server branch
using the Console sentinel (`(HANDLE)-1` for WOW) and calls `BasepExitVDM`.
`base/win32/server/srvvdm.c:BaseSrvExitVDM` then invokes either
`BaseSrvExitDOSTask` or `BaseSrvExitWOWTask`.

Before this checkpoint the standalone service dispatch table included
`BasepExitVDM`, but `base_rpc_client.c` did not forward it.  Consequently the
worker could not report original completion across the RPC boundary.

The selected binding is deliberately narrow:

- the copied RPC request carries only `is_wow` and the original WOW task id;
- the authenticated connection supplies the service-local Console identity;
- the worker wait remains an OS-managed typed attachment.  The broker revokes
  its receipt only after the original server has removed the record; the
  client receives the already-held worker-local event solely to close it, as
  original `ExitVDM` does.

This restores the original source's branch and record cleanup without sending
native handles or making a second task state machine.

## ConsoleRecord identity recovery

Original `BaseSrvCheckDOS` selects and mutates records by its Console handle;
that owner is unchanged. The former standalone binding instead assigned a
fresh local key to every authenticated connection, so a later `run16` from
the same modern Console could never reach the original `VDM_READY`/`VDM_BUSY`
selection path.

The selected S1 finite Console probe is now connected to the service. Before
its first `CheckVDM(EXISTING_CONSOLE)`, a new connection snapshots only live,
already authenticated registered candidates, asks an owned detached
`run16 --internal-console-probe` helper for membership, and revalidates each
process generation before publishing one existing local key. A failed,
ambiguous, stale, or unconfigured query fails closed; it never guesses a
Console relation. The broker itself never attaches to a user Console. This is
only the unavailable modern identity transport: it does not select a VDM,
queue a command, or create a pool.

## Empty-broker policy

`basesrv.exe` is still singleton-by-exclusive ncalrpc endpoint.  The new
product policy starts a 60-second grace only when both conditions are true:

1. the authenticated process registry has no entries or pins; and
2. the finite launcher-to-worker reservation list is empty.

An arriving `Connect` cancels the timer before registration.  The timer uses
an epoch under an SRW lock, so a cancelled/stale callback cannot stop a broker
that has accepted a new connection.  This is a broker retention policy, not
an interpretation of original BaseSrv task state.

In particular, an active worker connection makes the service nonempty even if
no command is queued.  Therefore this checkpoint does not reap interactive
COMMAND/EDIT and does not claim eligible-idle worker retirement.

## Verification

The focused x86 lifecycle test now executes actual retained OpenNT dispatch:

```
CheckVDM → UpdateVDMEntry → GetNextVDMCommand(PIF) →
GetNextVDMCommand(DOS) → GetNextVDMCommand(wait) → ExitVDM
```

It verifies the original ConsoleRecord/pair-wait route, broker receipt
revocation after original `BaseSrvExitDOSTask`, and the worker-side event
close obligation.  It also proves the empty predicate is false while an
authenticated launcher/worker or reservation exists and true only after both
disconnect/release.

The same test now configures a finite membership observer, retains an
authenticated first launcher, and submits a second launcher `CheckVDM`.
The observer is called exactly once and the unchanged original owner returns
success. A separate fresh x86 fixture launches the current `run16.exe` probe
and passes same/different Console separation, attached-controller refusal,
unchanged failure output, detached return, and owned-child cleanup. The formal
BaseSrv map contains `app_console_query`,
`OpenNtBaseServiceConfigureConsoleQuery`, and
`service_bind_existing_console`.

Commands passing from `build/M0-T412/S5/product`:

```
basesrv-reservation-test.exe
basesrv-service-reservation-test.exe
```

The second reports:

```
PASS: original Check/Update/Get/ExitVDM lifecycle completes through authenticated worker binding
```

### Resident-DOS reuse recovery

The original `BaseSrvCheckDOS` ready/busy branches create a pair of parent
completion events, retain the broker-side event in the DOS record, return its
caller duplicate through `CheckVDM.WaitObjectForParent`, and later use that
identity in `BaseSrvGetVDMExitCode`. The prior copied `Check` RPC preserved
only task and state scalars. Consequently a same-Console launcher could
identify a resident record but had no completion object and `run16` refused
the original `VDM_PRESENT_AND_READY` outcome.

The S5 binding now transfers this one unavailable OS resource as an
authenticated typed event attachment plus a broker-local receipt. No native
handle is copied into the scalar envelope. `run16` waits on the attachment
and invokes the existing `BaseCheckForVDM`; its copied client binding returns
the retained receipt only to original `BasepGetVDMExitCode`. `srvvdm.c`
continues to decide ready/busy, queue the DOS record, duplicate streams, wake
the worker and calculate the exit code.

The focused lifecycle test now also proves this sequence: a second same-
Console launcher gets `VDM_PRESENT_AND_READY`, an event attachment and a
nonzero receipt; the original worker's next `GetNextVDMCommand` consumes that
queued command; `ExitVDM` signals the attachment; and original
`BaseSrvGetVDMExitCode` resolves the receipt to exit code zero. This is a
service-level original-owner proof, not yet a real persistent COMMAND runtime
acceptance. Therefore `profiles/pure-dos/pure-dos.pif` remains until default
three-program COMMAND/MEM/EDIT runs prove the non-PIF DOSX path.

Real empty-broker observation used an isolated build `basesrv.exe`, with no
client or worker.  `O:\ntvdm64\logs\m0-t412-s5-empty-broker-r2.trace` contains:

```
BASESRV-S3 phase=empty-grace pid=0 status=00000000
BASESRV-S3 phase=empty-stop pid=0 status=00000000
```

The observed process exited after the configured grace.  The three current
product EXEs were rebuilt and staged at `O:\ntvdm64`.

## Remaining S5 gate

Recover and prove the original task-completion/ready notification needed to
classify a worker as eligible for reuse or retirement.  Required acceptance:
active COMMAND/EDIT survives beyond the grace; a completed eligible worker is
retired exactly once; concurrent arrival cancels retirement; disconnect and
new broker startup leave no stale reservation/record.  Do not implement this
by polling an empty command queue or by killing an otherwise connected worker.
