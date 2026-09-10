# M0 T318 S2 P63 — Pre-NTIO host UI and timer owner audit

## Purpose

Classify the source-owned execution that can occur before the original
`InitialiseDosEmulation()` call, after the fixed r33 container established that
the staged product is adjacent to its verified media and still terminates with
`0xC0000005`.

This is a static owner-contract audit.  It neither changes the runtime
container nor attributes the exception to a function without a crash frame.

## Selected source order

The selected CPU40 product compiles the original entry in
`src/mvdm-host/softpc.new/obj.vdm/ntvdm.c` with only its C entry symbol
renamed.  Its retained order is:

1. `TimerInit()`;
2. `CpuEnvInit()`;
3. `nls_init()`;
4. `host_main(argc, argv)`.

Within original `softpc.new/base/support/main.c`, the relevant pre-NTIO path
is:

```
host_main
  -> host_applInit
     -> init_host_uis
        -> InitScreenDesc
        -> SetupConsoleMode
     -> nt_start_event_thread
  -> config / cpu_init / host_init_screen
  -> InitialiseDosEmulation
```

The original call to `host_start_cpu()` is after `InitialiseDosEmulation()`.
Consequently neither a first guest instruction nor a BOP/provider route is
evidence for the current escaped exception.

## Timer classification

`TimerInit()` in `softpc.new/host/src/nt_timer.c` creates only the heartbeat
resume/suspend events, its two critical sections, and the performance-counter
baseline.  It does **not** create or resume the heartbeat thread.

The original `host_timer_init()` creates the heartbeat thread suspended from
the base timer initialization.  `host_reset()` in
`softpc.new/host/src/nt_reset.c` subsequently resumes it.  Thus timer code is
not an arbitrary concurrent source before the UI/console startup contract;
its first execution belongs to the later reset/device phase.  It remains an
owner to inspect if the static UI/device cluster proves clean, but it is not a
justified first repair target solely because the process has an access
violation.

## Explicit access-violation classification

The only selected direct `RaiseException(STATUS_ACCESS_VIOLATION, ...)` body
is `nt_msscs.c::nt_memory_fault`.  Its documented condition is an unhooked
page fault after VDD memory-hook lookup.  That path needs a VDD memory fault;
it cannot explain the pre-NTIO sequence above.

The original `ntvdm.c::DbgBreakPoint` checked-build branch also raises an
access violation when no debugger is attached.  The selected product passes
`/DPROD`, so that branch is compiled out.  No current observation therefore
permits treating the r33 status as a deliberate debugger-break result.

`VdmUnhandledExceptionFilter()` in original `nt_timer.c` calls the system
unhandled-exception policy and terminates with the received exception code.
It explains why the process result carries `0xC0000005`; it deliberately does
not reveal the originating instruction.

## Result and next owner unit

The current static result is a bounded, source-shaped next unit:

`host_applInit → init_host_uis → InitScreenDesc/SetupConsoleMode →
nt_start_event_thread`, including the original error/termination contracts and
the selected public-console facade bindings.

No source change follows from P63.  The next repair is permitted only if this
whole UI/console owner cluster yields a source-proven missing or divergent
modern binding.  The fixed r33 container and product observation remain
unchanged until then.
