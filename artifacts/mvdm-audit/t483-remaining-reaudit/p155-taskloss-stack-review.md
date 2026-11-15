# P155 TaskLoss callback boundary

This is failed-run evidence and source review, not repair acceptance.

## Captured failure

The build-only, pass-through vectored exception recorder captured a real
access violation without attaching a debugger. It always returned
`EXCEPTION_CONTINUE_SEARCH`; no exception was suppressed. Independent diagnostic
WOW32 was linked from the current product objects plus the recorder and staged
with its own package identity. Original guest media was unchanged.

Source/run artifacts: `build/t483-s5-p155/exception-veh.c`, `veh.ps1`,
`veh-trace-1.txt`, `WOW32-veh.map`, and `async-veh-1`.

Observed PID34252, TID30336, read address0, EDI0:

| Module/RVA | Mapped frame |
| --- | --- |
| WOW32+5712 | wow_original_CallBack16+52, dereference current PTD |
| WOW32+71b4b | CallBack16 wrapper |
| WOW32+18e0 | GetPModeVDMPointerAssert+50 |
| WS2_32+30615 | Native provider caller; no private symbol claim |

The optimized stack does not retain every converter frame. The source path
from Winsock post dispatch through the async result converter calls GETVDMPTR;
GetPModeVDMPointerAssert can invoke RET_FORCESEGMENTFAULT when the selector is
not present. Such a callback requires a current Win16 task, absent on this
provider thread. The trace establishes this failing boundary, not the precise
instant or cause of the selector becoming nonpresent.

## Exit and synchronization review

`mvdm-guest/wow16/kernel31/i21task.asm` sends SG_EXIT at its normal task exit
before GlobalFreeAll. Existing project USER cleanup reaches
wow_user_task_prepare_exit at this signal. W32FreeTask's original Winsock cleanup
is later. Early async retirement therefore belongs at an existing pre-resource
teardown boundary if this ordering is confirmed as the reproduced cause.

The last-task branch is different: ExitKernelThunk goes to WK32ExitKernel,
NtvdmAuthorizeExit, ExitVDM, ExitProcess. That branch does not pass through the
following GlobalFreeAll block. Do not claim every termination sends SG_EXIT.
The probe initializes USER and owns a window, but the captured stack alone does
not identify which guest exit branch it took.

WWS32FindAndRemoveAsyncContext removes a live context under WWS32CriticalSection
and releases that lock before the result converter writes guest memory.
WWS32CancelAsyncRequestsForTask only retires contexts still on that list.
Consequently, merely calling retirement earlier does not wait for an already
detached converter. A full-conversion serialization proposal must cover both
this interval and the pre-GlobalFreeAll boundary. No new mirror synchronization
has been implemented: the proposed five added lines require owner approval.

## Limits and next discriminating evidence

- Full19 passed, but does not supersede the TaskLoss worker failure.
- One successful P154 comparison does not establish that P155 caused the race.
- Debugger runs changed timing and did not reproduce; they do not disprove it.
- Record request/selector and pre-release phase in the same isolated diagnostic
  run before treating the selector-lifetime explanation as fully observed.
- Check lock order and native cancellation reentry before adopting serialization.
- A NULL-PTD guard would hide the failed guest-memory delivery and is not a fix.
- Run16 completion and guest phase3/errors0 are insufficient worker-lifetime
  acceptance; verify worker termination or absence of the captured exception.

Current source reduction remains wsdata231->174 and winsockp24->17, total
mirror difference -64. This investigation changes neither that count nor the
1798 reviewed-point count. P155 remains uncommitted pending resolution.

## Same-process timing witness

The original phase diagnostic repeat reached its next iteration before failing:
`phase-trace-2.txt` is the preceding healthy PID22832; the failing iteration is
`phase-trace-3.txt`, PID36268. They were not evidence of one file overwriting
another. A subsequent per-PID recorder attempt failed at package staging before
running a guest; it provides no new runtime result.

The failing trace records, in order:

1. Guest thread10896 enters pre-resource-exit, task17BF, tick346302406.
2. Provider thread7192 converts guest buffer1797:00AE, request owner serial1,
   tick346302421.
3. The same provider thread finds that selector not present, with CURRENTPTD0.
4. It faults in wow_original_CallBack16 at WOW32+5742 (diagnostic layout).
5. Guest thread10896 enters W32FreeTask and the repeated preparation hook.
6. Only then does it retire Winsock owner serial1, tick346302437.

This establishes the late-retirement ordering in the reproduced ordinary task
exit, rather than merely inferring it from source. The segment-free instruction
itself is not traced. The original guest ordering places GlobalFreeAll between
SG_EXIT and this later task cleanup. Serialization must also cover callbacks
already removed from the list; moving retirement alone cannot cover that case.
The five-line dispatcher lock proposal remains unimplemented pending approval.

## Test verdict correction

The maintained async probe previously wrote success before its finally cleanup.
It now initializes result.exit to1 and writes success only after cleanup finishes.
A build-owned AST fixture executes the actual runner finally/postlude with
controlled cleanup success and failure: both assertions passed (PID16432).
This verifies verdict publication, not normal worker exit or the product repair.
A real abnormal worker return still requires lifecycle evidence; cleanup success
alone is not a general proof of healthy worker termination.

## Owner-approved repair

Owner approved the five-line mirror increment and requested a retained revisit.
The existing early task-exit hook now retires asynchronous requests before guest
resource release. Original completion dispatch is enclosed by the existing lock
and a finally unlock. No second registry or lifecycle state was added. R1797
retains the coupled revisit; implementation approval does not close that item.
The final-source focused fixture passes353 assertions including actual dispatcher
serialization and exception propagation/unlock. Actual MSVC preprocessing finds
447 retained wsext statements with matching original logical lines. Runtime
acceptance and final delivery remain pending.
