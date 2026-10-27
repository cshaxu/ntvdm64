# Host timer mirror: divergence and lifecycle reconciliation

`softpc.new/host/src/nt_timer.c` selected x86 worker build and map provide
`host_timer_init`, heartbeat and `VdmUnhandledExceptionFilter`. Normalized
original line match is 980/982 = 99.80% (matching original lines only, not
behavioral completeness). Full diff has three change groups: DIV-050 typed
remainder, DIV-040 accepting user APC at the alertable heartbeat wait, and the
DIV-159 include/comment discussed below.

## Actual timing and exception paths

Original `base/system/timer.c::timer_init` registers `TimerEoiHook` and
calls `host_timer_init`. The latter creates a suspended heartbeat thread;
`TimerInit` creates suspend/resume events and initializes critical sections.
`DelayHeartBeat` calls original `NtWaitForSingleObject` / multi-object wait
according to MONITOR configuration and handles timeout, suspension/resume,
alert/termination and original error reporting. `TerminateHeartBeat` calls
`NtAlertThread` and, off the heartbeat thread, has the original 10-second
alertable join; this join exists identically in OpenNT and was not introduced
by these changes. `TimerEoiHook` and the selected original PIC callback own
actual timer interrupt cancellation, not `nt_timer.c`'s exception filter.
The T473 S5 `pic-reset-eoi-test` exercises actual timer callback along with
PIC reset; it does not prove host heartbeat APC failure/lifecycle.

DIV-050 changes the `RtlExtendedLargeIntegerDivide` remainder destination in
`host_GetSysTime` from signed field pointer to `ULONG` local followed by
explicit assignment. The divisor is 1,000,000 and the positive remainder is
less than it. `tests/opennt-host/rtl_x86_fixture.c` exercises divide ABI, but
not this specific heartbeat branch and its timestamp/reset concurrency.
Retain independent current-image test for normal clock plus heartbeat/non-
heartbeat and exact pre/post return values and negative/cleanup boundaries.

## DIV-040 registration wording

The adapter `ntvdm-exe/win32/nt_thread_alert_compat.c` currently first looks
up and uses NTDLL `NtAlertThread` when exported; only if absent does it queue
a public APC. Worker map binds `_NtAlertThread@4` to this adapter. The added
`STATUS_USER_APC` terminal case exists in nt_timer.c; normal native NT alert
and fallback APC are not equivalent as a general queued/pending alert.
`docs/etc/evidence/m0-t443-s13-ntalertthread-disposition.md` records native
export on its historical supported host and explicitly keeps absent-export
fallback as a limitation. `tests/adapter-mvdm-host-out/nt_thread_alert_compat_fixture.c`
exercises native alertable wake and invalid handle, while
`tests/mvdm-host/softpc_delay_heartbeat_fixture.c` tests actual DelayHeartBeat
suspend/resume with a mocked surrounding environment; it does not itself
prove no-export fallback or error/termination cleanup. These are distinct
fixtures and must not be conflated with `pic-reset-eoi-test`.

**Registration correction suggestion:** `src/mvdm/README.md` DIV-040 and the
inline comment should describe current native-first + absent-export APC
fallback, not assert that the adapter always uses QueueUserAPC. Preserve the
actual two accepted statuses and the T443 limitation; do not alter the timer
algorithm based on this documentation mismatch. Verify exact source and
history in the relevant owner task before making any code/comment edit.

## DIV-159 stale declaration and register

Current `nt_timer.c` contains the DIV-159 comment/include of
`mvdm_softpc_termination.h`, but its `VdmUnhandledExceptionFilter` body has
no call to `mvdm_softpc_record_unhandled_exception` or equivalent reporter:
it still calls SuspendTimerThread, UnhandledExceptionFilter, original
terminate-if-handler and ResumeTimerThread. Repository source search finds
`MVDM_EXCEPTION_REPORT_PATH` in the observer, not a production writer.
`git show a2ec4d531 -- src/mvdm/softpc.new/host/src/nt_timer.c` proves that
T420 S13 removed the reporting call, and
`docs/etc/evidence/m0-t420-s13-dem-and-residual-diff-recovery.md` states that
the sweep intentionally removed default-off observations. The remaining
comment/include and `src/mvdm/README.md` row still say the reporter exists.
No product crash or actual missing necessary feature follows solely from
this stale register.

**Registration correction suggestion:** reconcile DIV-159 as retired by
T420 S13, with commit and source evidence; assess whether the now-unused
include/comment can be removed after checking actual preprocessor dependency.
Do not restore a deleted observer solely to satisfy stale wording. If a
current product requirement does require exception reports, admit and prove
that separate requirement, with exact filter/lifetime/error behavior.

The registry now records the retired report and the native-alert-first/APC
fallback behavior. The source's stale inline comment and include remain for
the separately required dependency review; no runtime edit was made here.

## Remaining finite proof

- In the current identified package, exercise default native alert and,
  separately with a controlled resolver, absent-export APC at before/during/
  after waits, invalid handles and alert failure; verify original termination
  ordering, joins, no deadlock or double close and service-loss ownership.
- Verify typed time remainder and wrap/frequency limits with both heartbeat
  states and actual producer; record no hardware timing claim beyond fixtures.
- Reconcile DIV-159 report retirement and leftover include/register as above;
  distinguish T473 PIC/device tests from heartbeat exception evidence.

No new runtime process, product code, guest or external device was changed
or exercised for this review.
