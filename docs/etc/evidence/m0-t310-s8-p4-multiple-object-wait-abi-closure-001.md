# M0 T310 S8 P4: Multiple-Object Wait ABI Closure

## Original shared contract

The selected SoftPC host code has one NT4 wait contract, reached by:

- `nt_event.c`: console input versus suspend event, `WaitAny`, alertable;
- `nt_timer.c`: heartbeat suspend/resume, timeout and APC termination flow;
- `nt_ntfun.c`: communications event versus CPU event, `WaitAny`,
  non-alertable.

Its source shape is `NtWaitForMultipleObjects(count, handles, WaitAny/WaitAll,
alertable, timeout) -> NTSTATUS`. It must not be replaced by separate
controller-local waits.

## Modern binding

`adapter-mvdm-host-out/win32` now provides the same-shaped entrypoint with
public `WaitForMultipleObjectsEx` behind it. It preserves count/handle order,
`WaitAll`/`WaitAny`, alertable completion, relative or absolute NT 100-ns
timeout direction, and the observable wait result values. `WAIT_IO_COMPLETION`
maps to the already-admitted `STATUS_USER_APC` result used by the original
heartbeat termination branch.

This is an ABI/static closure only; P5 remains owner of whole-machine event,
timer and communications execution evidence.

## Verification

After regenerating both formal graphs with the required Node 22 provenance
hash path, these targets succeeded:

- x64: `obj/adapter-win32/nt_wait_compat.obj`, `obj/host/nt_event.obj`,
  `obj/host/nt_timer.obj`, `obj/host/nt_ntfun.obj`.
- x86: the same four targets.

Neither architecture emitted C4013/C4057/C4133 for the restored
`NtWaitForMultipleObjects` callers.
