# M0 T314 S2 — local Base VDM wake/retry closure

## Recovered source contract

The selected original control path is the DOS subset of BaseClient
`GetNextVDMCommand` in `opennt-host/base/win32/client/vdm.c` and BaseSrv
`BaseSrvGetNextVDMCommand` in `opennt-host/base/win32/server/srvvdm.c`.
When no DOS command record is available, the server publishes a wait object.
The client waits, adds `ASKING_FOR_SECOND_TIME`, and reissues its captured
request.  A capacity failure reports required sizes without consuming the
record; an environment request copies only the environment and leaves the
record available.

## Local source-shaped recovery

`adapter-mvdm-host-out/basesrv` now retains that admitted protocol with one
session-owned copied record:

- `base_vdm_local_dispatch` returns a typed local pending result only when no
  DOS record exists, records one pending request and resets its local event.
- A producer copies command bytes into the record and signals that event. It
  receives neither a `VDMINFO` pointer nor a guest/native resource from the
  waiting caller.
- `GetNextVDMCommand` keeps the original public ABI. It preserves a private
  caller snapshot, waits, marks only the second request with
  `ASKING_FOR_SECOND_TIME`, reconstructs the private request capacities and
  pointers, then retries the source-shaped dispatcher.
- Consumption clears the copied record's availability and pending state. The
  existing session teardown signals and releases the event, clears local
  ownership and destroys the record lock after the admitted request has
  completed.

This is intentionally not CSR transport, a generic application queue, a
cross-process broker, a child/parent wait-pair implementation, or a WOW/PIF
provider. Those original branches require the NT4 BaseSrv product shell and
remain explicit later owners.

## Verification

The focused Base VDM fixture now covers first-VDM, capacity retry,
environment-only copy, command consumption, a separate producer wake,
second-request retry, terminal no-command, reentry, directories, `ExitVDM`
and unbind/disposal. It passed after a fresh external x86 Ninja run:

```text
build/M0-T307/S1/x86
3/3 edges; PASS: local Base VDM broker contract
```

The test's producer passes only the session-local record and copied command;
it deliberately does not establish a multi-session or cross-process contract.
