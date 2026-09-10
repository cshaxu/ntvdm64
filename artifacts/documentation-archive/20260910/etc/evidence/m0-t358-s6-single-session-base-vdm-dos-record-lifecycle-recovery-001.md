# M0 T358 S6 — single-session Base VDM DOS-record lifecycle recovery

## Change

`adapter-mvdm-host-out/basesrv/base_vdm_local` now retains the reached
original DOS record states:

```text
EMPTY -> TO_TAKE_A_COMMAND -> BUSY -> HAS_RETURNED_ERROR_CODE
```

Initial delivery still follows the existing original-shaped BaseClient capture
and BaseSrv copy boundary.  A normal later command request with no record still
publishes the session wait event and retries after a producer signal.

The new bounded branch applies only when the original COMMAND child-return
call asks `RETURN_ON_NO_COMMAND` and the one local DOS record is `BUSY` or has
already returned.  The original BaseSrv would mark that child returned, wake
an external parent record, and let its BaseClient retry see no next command.
This profile has no external parent process or duplicated handle.  It records
the same return state and returns the same caller-visible empty/no-command
result directly; it does not fabricate a guest command, retain a pointer, or
introduce a scheduler.

## Source authority

- `opennt-host/base/win32/server/srvvdm.c`: DOS record selection, busy/return
  transition, parent wake and `RETURN_ON_NO_COMMAND` terminal disposition.
- `opennt-host/base/win32/client/vdm.c`: wait-object retry with
  `ASKING_FOR_SECOND_TIME`.
- `mvdm-host/dos/command/cmdexec.c::cmdReturnExitCode`: source caller that
  consumes the no-command result.

The original MVDM and guest files remain unmodified.  The one source-derived
divergence is documented inline where the absent NT4 parent-record product
shell is replaced by the identical bounded local outcome.

## Verification

1. Fresh x86 local Ninja fixture:

   ```text
   ninja -C build/M0-T302/S2/x86 test
   PASS: local Base VDM broker contract
   ```

   The fixture covers initial command copying, `BUSY` child return,
   `RETURN_ON_NO_COMMAND` empty result, subsequent producer wake/retry,
   re-entry accounting, DOS/WOW queue separation and teardown.

2. Formal CPU40/x86 Ninja increment:

   ```text
   ninja -C build/M0-T359/S2/formal-x86 original-softpc-process.exe
   ```

   completed eight affected compile/library/link actions, including
   `base_vdm_client`, `base_vdm_local`, `base_vdm_broker`, app declaration and
   final `original-softpc-process.exe` link.

## Limit

This is a one-session local DOS record lifecycle only.  CSRSS/CSR transport,
external parent process coordination, cross-process/multi-session records,
WOW notification and a general command broker remain out of scope.  The next
S may perform one fixed runtime observation of this formally linked product.
