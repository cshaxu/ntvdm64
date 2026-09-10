# M0 T313 S3 P1 — Original Controller Source Assembly

## Scope

This checkpoint establishes the selected x86 CCPU40 source baseline for the
S3 timer, interrupt, DMA and input cohort.  It does not claim full guest
device delivery, DOS boot, or a replacement for the historical NT4 monitor.

## Original control flow

- `softpc.new/base/system/timer.c` calls original `host_timer_init()`.
- Original `nt_timer.c` creates the suspended heartbeat worker in
  `host_timer_init()`, establishes its event/critical-section prerequisites in
  `TimerInit()`, and reaches `cpu_interrupt(CPU_TIMER_TICK, 0)` from
  `Win32_host_timer()` after its RTC/tick work.
- Original `nt_eoi.c` owns `InitializeIcaLock()`, `host_ica_lock()` and
  `host_ica_unlock()`; delayed IRQ/EOI continuation remains source-owned.
- `nt_mouse.c` and `nt_event.c` use that same original ICA lock before they
  issue original `ica_hw_interrupt()` calls.  S3 must therefore recover the
  controller cohort together, rather than install independent input shims.

## Current composition result

- Fresh external formal candidate: `build/M0-T313/S3/r1` completed
  `original-softpc-candidate` at **403/403** x86 CCPU40 edges.
- The corresponding forced-link audit completed and intentionally retained
  later-owner unresolved symbols: EMS `LIM_*`, debugger module/prompt forms,
  `ShowStartGlass`, virtual-memory section forms, debugger dispatch/hard-error
  forms and `DBGInit`.  None is replaced by this checkpoint.
- The existing original `DelayHeartBeat` witness passed on x86 after its build
  graph was made CCPU40/GDP-aware.  It proves the original suspend/pending and
  resume-to-`STATUS_SUCCESS` event contract only.
- The existing original ICA-lock witness passed on x86 after its graph was
  made CCPU40/GDP-aware and linked the already-registered
  `mvdm_ica_eoi_bridge` overlay required by `nt_eoi.c`'s original include.
  It proves the original lock excludes a second worker until release; it does
  not simulate a PIC interrupt.

## Immediate cleanup and divergence disposition

- Removed `CPU_30_STYLE` from the reached ICA verification graph.  The graph
  now uses the selected CCPU40 profile only.
- Added the generated GDP header prerequisite and the registered CCPU/GDP and
  ICA overlay include roots to the two existing verification graphs.  This is
  build binding, not a modification to `mvdm-host` source.
- No non-original implementation was found in the reached `mvdm-host`
  controller files to remove in this pass.  `mvdm_ica_eoi_bridge` remains a
  registered overlay because it preserves the original `int *` public wrapper
  while calling the selected `LONG *` PIC declaration.

## Remaining S3 work

The next pass must bind and exercise original timer-to-PIC delivery,
source-owned DMA initialization, and the original input initialization paths,
or record a typed unavailable result where a private NT4 product-shell API is
required.  It may not create a parallel controller implementation.
