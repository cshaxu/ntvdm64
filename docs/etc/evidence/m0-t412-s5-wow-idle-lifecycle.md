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

### Real three-program DOS reuse observation

The current staged `O:\ntvdm64\run16.exe`/`basesrv.exe`/`ntvdm.exe` set was
observed without redirected standard handles. A direct package-root
`run16.exe MEM.EXE` returned zero while leaving its original `ntvdm.exe -f`
worker resident; `m0-t412-s5-run16-mem-cmdline-20260914-185905.status.log`
records that worker command line and launcher result. The companion trace
records initial `Check → Reserve → Prepare → worker Connect` followed by the
original PIF, DOS and next-command `GetNextVDMCommand` requests.

A single `cmd.exe` Console then launched the same positional `run16 MEM.EXE`
twice. In `m0-t412-s5-same-console-probe-20260914-190456.log`, the first
launcher has the expected `Check → Reserve → Prepare` sequence. The second
authenticated launcher has `Check` but **no** second `Reserve`, `Prepare`, or
worker `Connect`; the existing worker alone performs the subsequently logged
original GetNext requests, and that second launcher has disconnected by the
seven-second process snapshot. This proves same-Console resident-record
selection only. It does **not** prove that an already-interactive COMMAND
worker consumed the second record or completed MEM. The probe's exact broker
and worker PIDs were then terminated; no test process was retained.

### Correction: resident COMMAND is not automatically fetchable

The later real `run16 mem` hang established the omitted distinction. After a
parent has collected an earlier command's exit code, original
`BaseSrvGetVDMExitCode` changes the sole DOS record from
`VDM_HAS_RETURNED_ERROR_CODE` to `VDM_READY`. A later original
`BaseSrvCheckDOS` treats that state as reusable, changes it to
`VDM_TO_TAKE_A_COMMAND`, returns `VDM_PRESENT_AND_READY`, and signals the
worker event only when `hWaitForVDMDup` already exists. It does not establish
that the worker is currently in `GetNextVDMCommand`.

An interactive COMMAND prompt reaches that client call only through its
source-owned child/re-entry routes (`cmdExec32` or `cmdReturnExitCode`), not
merely because it is sitting at a prompt. Consequently the product cannot
interpret `VDM_PRESENT_AND_READY` as an unconditional right for `run16` to
wait forever. The apparent second-launcher success above is therefore not an
acceptance result. S5 must recover the source-shaped re-entry/delivery
contract, with trace evidence that a pending worker wait was signalled and
then consumed, before same-Console `run16 MEM.EXE` can be accepted.

### Follow-up repair: worker wait is the reuse boundary

The repair keeps the observation with the selected original owner, not in a
broker-local worker state machine. `srvvdm.c:BaseSrvDOSWorkerWaitPending`
holds `BaseSrvDOSCriticalSection`, distinguishes an absent `ConsoleRecord`
from an existing record, and queries the original server-side
`hWaitForVDMDup` event. A record is reusable only when that event exists and
is nonsignalled: this is the state installed by an actual
`GetNextVDMCommand` no-command return. `VDM_READY` alone is deliberately not
enough.

For an existing record with no pending worker wait,
`base_service.c:OpenNtBaseServiceCheck` presents `NULL` ConsoleHandle to the
unchanged `BaseSrvCheckDOS`. That selects its original no-console record and
returns a nonzero `DosSesId`; the binding allocates one finite service-local
Console identity for the reservation, so the new worker's original PIF
request binds that new record rather than the resident COMMAND record.
`run16` creates that `DosSesId` worker with `CREATE_NEW_CONSOLE`. The default-
off `MVDM_BASESRV_TRACE_PATH` trace now emits
`check-separate-dos-session`; it contains only state and status.

The focused x86 `basesrv-service-reservation-test.exe` passes after proving
both directions: an existing ready record without a wait is rejected as not
reusable; after the original worker completes its current command and enters
`GetNextVDMCommand` wait, Check signals that wait and the source-owned
second-time Get consumes the command. The full x86 `run16.exe`, `basesrv.exe`
and `ntvdm.exe` links also pass from `build/M0-T412/S5/product` (with only the
pre-existing `ntvdm32.exe` export-name link warning).

No package-root runtime claim is made here. The reported live `run16 mem`
launcher, resident worker and broker were deliberately left untouched; this
build has not replaced their files or reused their endpoint. A later clean
package run must observe either the separate-session trace/worker or the
actual wait signal and command consumption before this is accepted as real
runtime closure.

Earlier redirected-stdio observations timed out with empty output because
the launcher had no Console-standard handles. They are retained as a distinct
non-Console execution result, not treated as a failure of the Console product
route or a reason to alter original BaseSrv selection.

Real empty-broker observation used an isolated build `basesrv.exe`, with no
client or worker.  `O:\ntvdm64\logs\m0-t412-s5-empty-broker-r2.trace` contains:

```
BASESRV-S3 phase=empty-grace pid=0 status=00000000
BASESRV-S3 phase=empty-stop pid=0 status=00000000
```

The observed process exited after the configured grace.  The three current
product EXEs were rebuilt and staged at `O:\ntvdm64`.

## Remaining S5 gate

The owner superseded the former worker-retirement hypothesis: every VDM worker
now follows the selected original OpenNT COMMAND/Console/`ExitVDM` lifecycle.
S5 must recover and prove that path, including completion, disconnect and
record cleanup, rather than inventing an eligible-idle notification, lease,
timer or broker-directed exit. The existing 60-second delay is broker-only:
it runs only after the service has no worker, reservation, queue or client and
allows the otherwise empty `basesrv.exe` instance to stop. Required acceptance:
active COMMAND/EDIT residency is preserved; original completion/disconnect
cleans records exactly once; broker drain versus a new arrival is safe; and a
new broker startup has no stale record. Do not poll an empty command queue or
kill an otherwise connected worker.

## Original worker exit restoration

### Question

Does the standalone `ntvdm.exe` still suppress the selected OpenNT worker
completion path after the three-program split?

### Source comparison and change

The selected OpenNT `softpc.new/host/src/nt_reset.c` owns the actual worker
terminal path. `host_terminate` calls `ExitVDM(VDMForWOW, (ULONG)-1)` for WOW
or `ExitVDM(FALSE, 0)` for DOS, then calls `ExitProcess(VdmExitCode)`.
`host_applInit` likewise rejects a non-`-f` direct worker invocation with
`ExitProcess(0)`. The original BaseClient `ExitVDM` dispatches `BasepExitVDM`;
the selected `srvvdm.c:BaseSrvExitDOSTask` signals pending parents, closes the
worker wait and frees the Console record.

The project mirror had replaced both `nt_reset.c` process exits with
`mvdm_softpc_terminate_current_session`. That was a single-process
`ntvdm32.exe` precaution. It is invalid for the now separate `ntvdm.exe`
worker: the escape bypasses the original `ExitVDM` service notification and
can leave broker state to disconnect cleanup rather than source-owned terminal
cleanup.

The mirror now restores the selected original `nt_reset.c` bodies and removes
its private termination include. `MVDM-HOST-DIV-147` remains only for the
separate `sim32.c` in-process allocation-failure boundary; it no longer covers
worker lifecycle.

### Verification and limit

The x86 translation unit was rebuilt using the formal Ninja command shape,
then its `original-softpc-host-roots.lib` and `ntvdm.exe` product link were
rebuilt. The focused
`basesrv-service-reservation-test.exe` passed the original
`Check/Update/Get/ExitVDM` lifecycle through the authenticated binding. The
staged `O:\ntvdm64\ntvdm.exe` is SHA-256
`D1A4F0D9E7FA5FF2BA8EB75510DC4E061798BF7C2AA995267B2F0FA872409E4D`.

This proves source selection, compilation, link and service-side terminal
contract.

### Real standalone worker observation

An unredirected package-root `run16.exe MEM.EXE` smoke used the staged worker
above and the new log
`O:\ntvdm64\logs\m0-t412-s5-original-worker-exit-smoke-20260914-193001.log`.
The authenticated launcher PID 14592 reached `Check → Reserve → Prepare`; its
worker PID 19752 connected and then produced the decisive source-owned
sequence:

```
BASESRV-S3 phase=exit pid=19752 status=00000000
BASESRV-S3 phase=exit-code pid=14592 status=00000000
BASESRV-S3 phase=disconnect pid=14592 status=00000000
BASESRV-S3 phase=empty-grace pid=0 status=00000000
BASESRV-S3 phase=empty-stop pid=0 status=00000000
```

Thus the real worker reached the broker's original `BasepExitVDM` path; the
parent received its original exit-code result; the worker record disconnected;
and the now-empty broker exited under its own empty-grace. The test did not
kill the worker. The attempted exact-root cleanup found PID 14592 already
gone; the broker PID 48324 was then observed to emit `empty-stop` and exit.
This is a noninteractive startup/exit observation, not acceptance of an
interactive COMMAND/EDIT session or full WOW/WRITE behavior.

## Package-local launcher discovery

The owner-directed `run16_entry.c` change keeps image classification with the
selected original `OpenNtBaseGetBinaryTypeW` caller.  It adds only product
file discovery: for a bare target name, `run16.exe` first searches the
directory containing the installed three-program package, then falls back to
the ordinary process search path.  The selected classifier and the eventual
`CreateProcessW` receive the resolved canonical path; this does not add a
second header parser or program-type policy.

The formal x86 S5 product rebuilt `run16.exe` from the changed translation
unit and linked it against the unchanged formal BaseClient/broker libraries.
Its SHA-256 is
`2C24A5AEC75BDDE08686E46B58591CE2529AEE3743F8FFC6A2F2E68AF8101C06`.
The companion formal products remain `basesrv.exe`
`5EEC4226F6D98C8C4ABB20C408688BA93108B67EA315C8E1DC09D819B2110710`
and `ntvdm.exe`
`E0DEA3FDAC51BCABD47A01A979C53607BB2261ECE8A83F5BCB159D5F63D7F389`.

The formal graph also required one source-neutral archive-order correction.
`nt_inthk.c` is an original host translation unit which defines the CCPU40
hardware/software/fault hook installation entry points.  Its archive had been
scanned before the CCPU40 and DPMI archives first referenced those symbols.
The `ntvdm.exe` link row now repeats the same original
`original-softpc-host-roots.lib` at the tail.  No new provider, hook or
runtime policy was added; the final formal graph reports `ninja: no work to
do` after the x86 link completes.

For a behavioral check, the staged `O:\ntvdm64\run16.exe` was launched
with bare `MEM.EXE` while its working directory was
`build/M0-T412/S5/package-search-cwd`, which contains no `MEM.EXE`.  The
package directory does contain the immutable `O:\ntvdm64\MEM.EXE`.
The final-runtime log
`O:\ntvdm64\logs\m0-t412-s5-run16-final-sibling-search-20260914-195000.log`
contains the authenticated worker connection marker:

```
NTVDM-S3 phase=connect status=00000000
```

The exact launcher PID 40248 and any child had naturally exited by the
five-second observation.  This proves the final staged `run16.exe` reached
the existing three-program chain through package-local resolution, without
relying on the current directory.  It is not a replacement for the separate
interactive COMMAND/EDIT or WOW acceptance gates.

## Launch rollback recovery

`BaseCheckVDM` publishes the original DOS/WOW record before the launcher
creates, registers and resumes its worker.  The standalone `run16` caller had
omitted the selected original failure leg: a failed pre-resume creation or
registration could leave that published record in the broker.  A later launch
could then mistake it for a reusable worker record.

The launcher now records the three source-shaped milestones—published,
registered and resumed—and, only when a failure occurs before `ResumeThread`,
calls `BaseUpdateVDMEntry(UPDATE_VDM_UNDO_CREATION, ...)` with the original
`VDM_PARTIALLY_CREATED` or `VDM_FULLY_CREATED` state.  It preserves the first
launch error and treats cleanup as best effort, matching the server's record
owner.  It does not run after a resumed worker: normal worker completion stays
on the existing original `ExitVDM` route.

The formal x86 `run16.exe` relink and the focused
`basesrv-service-reservation-test.exe` pass after this recovery.  A new
real-package observation from `O:\winnt` ran two same-Console, unredirected
`run16.exe MEM.EXE` launches.  Both launcher processes returned `0` inside
the 30-second bound and
`O:\winnt\logs\m0-t412-s5-reuse-routing-20260914.trace` records the complete
`Check → Reserve → Prepare → worker connect → ExitVDM → parent exit` sequence
for each.  The first MEM worker exited normally, so the live package did not
leave an interactive COMMAND worker available to exercise reuse.  The focused
reservation fixture separately and exactly covers the source predicate for
that case: an existing DOS record without a pending original wait is not
reused; after `GetNextVDMCommand` installs its wait, it is reusable.  The
prior `O:\ntvdm64` paths above are retained as historical evidence only.

### Native-Console resident-worker re-entry

After deployment at the new package root, an independent native `cmd.exe`
Console ran two positional `run16.exe MEM.EXE` launches in sequence.  Both
launchers returned `0`; the first left `O:\winnt\ntvdm.exe -f` resident, and
the second did not create another worker.  The bounded observation was then
cleaned up by exact executable path.

`O:\winnt\logs\m0-t412-s5-native-console-reuse-20260914.trace` records the
first worker's PIF/DOS/next-command sequence, then the second launch's
`CheckVDM` state `00000004` (`VDM_PRESENT_AND_READY`) and the same worker's
second-time command requests.  Its `0000040C` request is the selected original
COMMAND re-entry shape: `RETURN_ON_NO_COMMAND`, `ASKING_FOR_SECOND_TIME` and
the DOS request bit.  The one `C000000D` response is the original size-query
round trip before the successful retry, not an emulator fault.

This is real package evidence that an actually resident worker receives the
subsequent launch without the former infinite parent wait.  It does not claim
interactive keyboard/EDIT semantics, full redirected COMMAND behavior or
WOW/WRITE acceptance; those remain their separately stated boundaries.

### Original first-VDM scope

The selected `BaseSrvIsFirstVDM` owns one process-global `fIsFirstVDM` bit:
the first request returns true and clears it.  It is neither a worker-local
flag nor a persistent machine-wide setting.  The standalone server now has a
default-off `first-yes`/`first-no` observation at that existing RPC boundary;
it reports the already-selected original result and creates no new state.

With a newly started `O:\winnt\basesrv.exe`, two independent native-Console
`run16.exe MEM.EXE` workers returned zero.  The first trace at
`O:\winnt\logs\m0-t412-s5-first-vdm-20260914.trace` records `first-yes`; the
second records `first-no`.  After the exact test-owned worker/broker processes
were stopped, a fresh broker and worker returned zero and
`O:\winnt\logs\m0-t412-s5-first-vdm-restart-20260914.trace` records
`first-yes` again.  Thus the bit is consumed exactly once per broker process
and resets only with the fresh process, as the selected source specifies.
