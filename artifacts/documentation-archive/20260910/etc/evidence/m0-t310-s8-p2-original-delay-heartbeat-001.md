# M0 T310 S8 P2 — original DelayHeartBeat source binding

## Question

Can the selected original SoftPC `DelayHeartBeat` suspend/resume contract run
unchanged on both supported host architectures without selecting MONITOR,
Bochs, kernel VDM, or a replacement timer controller?

## Original path under test

The exact mirror
`mvdm-host/softpc.new/host/src/nt_timer.c` retains the original
non-`MONITOR` source form:

1. alertably wait on `hHBSuspendEvent` for the requested relative interval;
2. return `STATUS_TIMEOUT` when the interval expires;
3. on suspend, wait alertably for `hHBResumeEvent`;
4. return `STATUS_SUCCESS` after resume;
5. preserve the source's terminal alert handling.

This is the original source-selected simple event contract.  It is **not**
called an x64 branch: the historical comment calls it the RISC/simple path,
and its suitability for the selected non-MONITOR profile is established here
only by direct behavior, not by an `i386` inference.

## Focused closure

`tests/mvdm-host/softpc_delay_heartbeat_fixture.c` links the unchanged
original translation unit and drives only `DelayHeartBeat`.  `nt_timer.c`
also contains initialization, BDA, RTC and device calls outside this narrow
function.  The fixture supplies guarded, test-only unresolved forms for those
unselected outbound edges; every such form terminates the test if called.
`Start_of_M_area` is likewise a link-only anchor: this evidence does not
claim a guest-memory BDA backing route.

The test starts the original wait in a native harness worker, proves it is
pending, signals the original suspend event, proves it remains pending, then
signals the original resume event and requires `STATUS_SUCCESS`.  The original
translation unit itself still sees its historical `CreateThread` expression
through the registered cdecl-to-WINAPI adapter; only the fixture undefines that
macro for its own harness worker.

## Result

The following formal MSVC `/MT` Ninja results passed outside the sandbox:

```powershell
ninja -C build/M0-T310/S8/p2-heartbeat-source/x86 verify
ninja -C build/M0-T310/S8/p2-heartbeat-source/x64 verify
```

Both builds compile the exact original `nt_timer.c` and execute the same
event-sequence fixture.  The only diagnostics are fixture-local unreachable
code warnings after guarded failure stubs.

## Disposition

This closes direct source evidence for the selected original simple
suspend/resume wait contract.  It does **not** close:

- `host_timer_init` / `TimerInit` end-to-end lifecycle;
- BDA writes in `host_init_bda_timer` (must use the selected SoftPC
  guest-memory mapping boundary, not a raw host pointer or Bochs route);
- PIT/RTC tick production, ICA/PIC delivery, delayed interrupts, or teardown;
- MONITOR, X86GFX, WOW wake, or kernel-VDM paths.

Those remain P2 source-composition work under their original owners.
