# T419 S1 DTASKMGR Worker Contract Audit

## Question

Can a native Console manager list and end this product's active work without a
second task registry, a client-supplied PID/HANDLE, arbitrary process control,
or a change to original DOS/WOW task policy? If so, what is the exact selected
unit of termination?

## Inputs and procedure

The audit read the active three-program BaseSrv transport and service:

- `src/basesrv/transport/service.idl`;
- `src/basesrv/main.c`;
- `src/basesrv/opennt/source/base_service.c`, `base_process.c` and
  `base_reservation.c`;
- the selected original `src/opennt-host/base/win32/server/srvvdm.{h,c}`; and
- the current x86 build graph in `tools/build/New-T310OriginalSoftpcNinja.ps1`.

It traced connection registration, launch reservation, worker registration,
original DOS/WOW record ownership, process-exit watch, original cleanup and
the existing authenticated RPC version check. No source, build or runtime
behavior changed during this audit.

`New-T310OriginalSoftpcNinja.ps1` generated the formal x86 graph at
`build/M0-T419/S1/formal-x86-001`. Its native Ninja runner repeated the known
host startup-lock condition: after 30 seconds it consumed negligible CPU,
produced no product artifact and held `.ninja_lock`. The audit terminated only
that self-started runner and deleted its disposable partial build root after
validating the exact path. A 443-command serial fallback was then started and
confirmed to compile original x86 objects, but progressed at an impractical
two commands per minute on this host. It was stopped before any source change
and its exact build root was deleted. This is an unavailable build result, not
a pass; S2 must repair/use the serial runner or another recorded full-build
route before it can claim its required regression matrix.

## Observations

### The existing authoritative state

`OpenNtBaseRegisterProcess` derives a process ID from an authenticated process
attachment, duplicates its handle with the existing rights and assigns a
monotonic `CSR_PROCESS.SequenceNumber`. It never accepts a client-provided PID
as authority. `OpenNtBaseServiceConnect` stores that `CSR_PROCESS` in an
`OPENNT_BASE_CONNECTION`.

When a worker claims its launch reservation, `base_service.c` registers an
`OPENNT_BASE_WORKER_WATCH`. That object contains the existing retained
`CSR_PROCESS.ProcessHandle`, its `SequenceNumber`, console identity and WOW
flag. `RegisterWaitForSingleObject` already invokes
`service_worker_terminated` on that exact process handle. The callback calls
the original `BaseSrvCleanupVDMResources`, drains the authenticated connection
and leaves parent wait/result behavior to the selected original ownership.

The original `srvvdm.c` owns the guest-side records:

- `DOSHead` → `CONSOLERECORD` → `DOSRECORD` exposes a VDM's DOS record chain,
  `VDMState` and `VDMINFO` application/title data; its synchronization owner is
  `BaseSrvDOSCriticalSection`.
- `WOWHead` → `WOWRECORD` exposes original WOW task IDs, dispatch state and
  `VDMINFO`; its synchronization owner is `BaseSrvWOWCriticalSection`.
- `CONSOLERECORD.SequenceNumber` and `WOWHEAD.SequenceNumber` associate those
  original records with the registered worker's `CSR_PROCESS.SequenceNumber`.

Thus the selected source already knows which original DOS/WOW records belong
to a registered worker. It does **not** supply a distinct stable ID or start
time for each DOS child record. In an interactive DOS VDM, COMMAND, MEM and
EDIT can be distinct `DOSRECORD` activity while sharing one worker process.

`BaseSrvExitVDMWorker` is cleanup after a VDM has exited: it signals parent
waits and frees original records. It is not a task-manager API and does not
terminate a live child program or worker. Calling it as a kill shortcut would
break the original ordering.

## Selected contract

DTASKMGR's selectable unit is one **VDM worker**, not an individual DOS/WOW
child. The manager displays the worker's current original record summary and
may list hosted child names/states as informational rows, but selection and
termination always target the enclosing worker. The UI confirms that all
hosted child work ends with it.

The public identifier is a copied pair:

```text
broker_epoch (u64) + worker_sequence (u32)
```

`worker_sequence` is the existing `CSR_PROCESS.SequenceNumber`; it is stable
only while that BaseSrv instance lives. `broker_epoch` is one new BaseSrv
startup value, generated once and returned in every management snapshot. A
terminate request must echo both. This rejects a stale selection after broker
restart or sequence reuse without exposing a PID, HANDLE, pointer or guest
address.

The worker start time is captured in the existing worker-watch object at
worker registration. It is the truthful start time available without an
independent child-task ledger. A child program row therefore carries no
fabricated separate start time.

### Read-only query

The new management query is a BaseSrv-owned, versioned RPC operation. Each
call carries the existing application/protocol identity and an authenticated
system process attachment. It does **not** create an `OPENNT_BASE_CONNECTION`,
enter the original process registry or persist a management client record.
Consequently a running DTASKMGR does not keep an otherwise empty broker alive.

After `broker_rpc_peer_process` and the existing version comparison, BaseSrv
acquires its service lock and then the original WOW and DOS locks in the same
order used by reached dispatch paths. A new `src/basesrv/` read-only adapter
copies bounded, sanitized display data from the original records into a
fixed-width reply, then releases all locks before RPC marshaling. It is a
reader of original state, not a second registry or scheduler. The global RPC
authorization callback currently cancels the empty timer for every accepted
call; the management operation must re-evaluate/re-arm that timer after its
reply when the service is empty.

### Termination request

The new request receives only `{broker_epoch, worker_sequence}`. BaseSrv:

1. authenticates and version-checks the caller;
2. under its service lock, resolves the watch whose existing
   `CSR_PROCESS.SequenceNumber` equals `worker_sequence`, checks the epoch and
   confirms the retained process is still live;
3. marks the existing watch as termination-requested and duplicates its
   process-local handle with unchanged rights; and
4. releases all locks and calls `TerminateProcess` only on that retained local
   duplicate, with `ERROR_CANCELLED` as the exit status.

DTASKMGR cannot issue a raw PID or handle and never calls `TerminateProcess`.
The server's existing process-exit watch, not the request handler, then runs
the original cleanup path. A duplicate request, disappeared worker or race
with natural exit returns a copied `already-exited`/`not-found` result; it is
not interpreted as success against a subsequently reused worker ID. Siblings
are not enumerated or touched.

## Lifecycle matrix

| Event | Query result | Terminate result |
| --- | --- | --- |
| Broker absent | RPC unavailable; DTASKMGR displays empty/unavailable state. | Disabled locally. |
| Broker live, no worker | Empty list; empty timer remains eligible. | No selection. |
| Worker live | One worker row with current DOS/WOW summary. | Confirmed request may be accepted. |
| Worker exits naturally during refresh | Row disappears on next snapshot. | `already-exited` or `not-found`; no retry against a new worker. |
| Broker restarts | New epoch; old selection is rejected. | Epoch mismatch; refresh required. |
| Worker hosts COMMAND plus child DOS/WOW work | One selectable worker with hosted activity disclosed. | Confirmation states all hosted work ends. |

## Interpretation and confidence

The worker-level design is high confidence: the existing authenticated
worker-watch handle and process-exit callback provide an exact finite control
capability and preserve the original post-exit cleanup order. The original
record topology proves why a child-level kill is not available without a new
task metadata/termination policy. The approved worker scope avoids that
divergence.

The remaining implementation risks are bounded: lock-order verification in a
new read-only snapshot adapter, reply-size/string truncation, epoch creation,
and negative race tests. They are S2/S3 implementation requirements, not
unknown product ownership.

## Follow-up

S2 implements the read-only BaseSrv management query and `src/dtaskmgr/`
Console TUI. S3 implements the confirmed server-owned worker termination path
and its race/lifecycle matrix. Both retain the mandatory direct and interactive
COMMAND/MEM/EDIT regressions.
