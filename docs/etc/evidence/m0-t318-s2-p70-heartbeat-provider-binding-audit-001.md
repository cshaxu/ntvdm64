# M0 T318 S2 P70 — heartbeat provider binding audit

## Scope

P69 proved that the first NTIO near jump (`0070:0000` to `0070:0370`) takes
the original CPU40 inter-instruction path before the next guest fetch. This
record audits the complete original heartbeat cohort that may run at that
check. It answers only whether a missing or uninitialised selected provider is
a source-supported repair candidate.

## Source order and binding

`softpc.new/obj.vdm/ntvdm.c` calls `TimerInit()` before `host_main()`. The
latter calls `host_applInit()` before `gfi_init()`, `config()`,
`InitialiseDosEmulation()`, and `host_start_cpu()`.

`host_applInit()` in `softpc.new/host/src/nt_reset.c` sets
`working_video_funcs = &nt_video_funcs` before the CPU can run. The original
`nt_video_funcs` object in `softpc.new/host/src/nt_graph.c` supplies
`nt_graphics_tick` in its `graphics_tick` slot. Therefore the
`host_graphics_tick()` macro used by `host_timer_event()` has a selected,
original non-null provider before the heartbeat worker is resumed.

The worker is only created suspended: `base/system/timer.c` calls the
original `host_timer_init()`, whose `CreateThread` call has
`CREATE_SUSPENDED`. The original NT reset sequence calls `host_reset()` only
after disk POST, and `host_reset()` then resumes that worker. This preserves
the original ordering; there is no early-worker race caused by the current
composition.

The P69 cohort calls are all selected original translation units in
`build/M0-T318/S2/formal-x86/build.ninja`:

- `host_graphics_tick` -> `host/src/nt_graph.c`;
- `GetScaleEvent` -> `host/src/nt_event.c`;
- `IDLE_tick` / `PrioWaitIfIdle` -> `host/src/nt_unix.c`;
- COM/LPT/floppy heartbeat -> `host/src/nt_com.c`, `nt_lpt.c`, `nt_rflop.c`;
- `DemHeartBeat` -> `dos/dem/demsrch.c`;
- time/tone -> `base/system/timestrb.c`, `host/src/nt_sound.c`.

## Disposition

No missing heartbeat function-vector binding, replacement provider, or
initialisation-order inversion was found. In particular, P65's C-VID timing
vector repair is distinct from the host `VIDEOFUNCS` vector audited here.
Changing this cohort, suppressing the first inter-instruction event check, or
adding a replacement scheduler would invent a recovery without source evidence
and is not admitted.

The fixed runtime container remains the only valid dynamic observer. P68
already established that adding console writes to the execution hot path
changes the observed trace; P70 consequently makes no dynamic claim and adds
no instrumentation.

## Result and next boundary

P69's heartbeat cohort is eliminated as a **missing-binding** explanation for
the post-`E9` non-observation. It is not eliminated as a future integration
path: timer/PIC/host-device behavior still needs ordinary child execution
evidence. The next investigation must stay source-shaped and inspect the
remaining original CPU40 control-transfer/post-instruction obligations without
adding a per-instruction or console probe.
