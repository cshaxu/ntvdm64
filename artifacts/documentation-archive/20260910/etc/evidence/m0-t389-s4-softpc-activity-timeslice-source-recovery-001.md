# M0 T389 S4 — SoftPC activity/timeslice source recovery

## Scope

This packet classifies the complete reached `ActivityCheckAfterTimeSlice`
cohort before changing its binding.  It is not a COMMAND/BOP implementation:
the callback is reached after the original host idle interval and was the
first reason the fixed `command.com` observation ended before the child shell
could reach its original `Do16BitPrompt`/DOS-CON path.

## Original caller cohort

The selected OpenNT CPU40 declaration carrier is
`mvdm-host/softpc.new/base/ccpu386/cpu4gen.h`.  It retains the exact no-argument
`VOID` entrypoint and CPU vector form.  The selected original callers are:

* `softpc.new/host/src/nt_unix.c:WaitIfIdle` — after a source-owned auto-reset
  `IdleEvent` wait of 10 or 1 ms;
* `nt_unix.c:host_release_timeslice` — after its PIF/interrupt-conditioned
  0 or 25 ms wait;
* `nt_unix.c:PrioWaitIfIdle` — after the original percentage-derived wait;
* `softpc.new/host/src/nt_eoi.c:BlockWOWIdle` — only after the original
  heartbeat interval detects an unblocked WOW task.

All four callers retain their existing original `IdleNoActivity`, `IdleEvent`,
`NowWaiting`, `ienabled`, PIF, heartbeat, ICA-lock and IRQ ordering.  No
caller passes a pointer, handle, guest address, register frame, BOP selector
or command record to this callback.  Therefore neither the mapping manager
nor an app/session command route belongs at this ABI.

## Provider provenance and disposition

The OpenNT/OpenNT-4.5/OpenNT-src-2 source union supplies the declaration and
all selected callers but no provider definition.  The registered NTVDMx64
derivative patch `mvdm-softpc-patch/patches/common/fmstubs.c` supplies the
same-spelled original patch body:

```c
VOID ActivityCheckAfterTimeSlice(VOID)
{
}
```

The prior product provider was
`adapter-mvdm-host-out/softpc/mvdm_softpc_activity_unavailable.c`.  It was an
autonomous substitute: it converted this normal void callback into a session
`ERROR_CALL_NOT_IMPLEMENTED` controlled stop.  The fixed T389/S3 rerun proved
that exact behavior (`origin=ccpu:ActivityCheckAfterTimeSlice`, `0x78`) and
that the execution bridge took its controlled-termination branch.

The selected recovery is the smallest source-shaped form: compile only that
imported patch body with `MVDM_SOFTPC_PATCH_ACTIVITY_CHECK_ONLY` and normal-link
its one symbol.  The full `fmstubs.c` is still not linked: it contains unrelated
empty/debug-break placeholders whose unresolved contracts must remain visible.
This recovery does not invent a scheduler, yield, timer, BOP, Console policy,
guest memory action or CCPU modification.

## Result and remaining boundary

`ActivityCheckAfterTimeSlice` now returns to the original caller, exactly as
the selected patch body does.  It removes the known premature 0x78 stop but
does not claim full NT4 activity accounting or multi-WOW scheduling.  Any
later reached, non-source-provided activity mechanism must be admitted by its
own original owner cohort; it cannot be hidden behind this callback.

## Required verification

Generate the formal x86 CPU40 graph, build the selected activity object and
normal product, then rerun the *unchanged* T389/S3 fixed-container
`command.com` observation.  Acceptance requires that the termination report
does not name `ccpu:ActivityCheckAfterTimeSlice`/`0x78`; it does not itself
prove banner, prompt, DOS-CON or interactive completion.

## Verification result

The formal graph at `build/M0-T389/S4/activity-source-x86` completed its
normal `original-softpc-process.exe` link with the selected
`ntvdmx64-softpc-activity-check.lib`. Its staged product hash was
`F02025B786B13F966C56853204F2F6D41E961E36359175784C30B33F5DB86BDF`.

The same non-debug, Console-owning, 30-second observation used by S3 was then
rerun unchanged except for the standard default-off termination report. It
timed out at the pre-existing keyboard-BOP readiness marker, but produced no
termination report. In particular, the previous
`ccpu:ActivityCheckAfterTimeSlice` / `0x00000078` controlled stop did not
recur. This closes S4's source/ABI/provider recovery criterion while leaving
the original child-shell banner/prompt/DOS-CON proof open.
