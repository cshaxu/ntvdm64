# M0 T313 S3 P2 — Original Controller Chain And Input Binding

## Result

The selected x86 CCPU40 graph retains one original SoftPC controller chain.
No project controller, PIC, DMA, keyboard, mouse or console-event replacement
was introduced.

## Source-shaped chain

- Original `bios/reset.c` initializes the two original ICAs before the
  controller set, then calls `dma_init(); dma_post();` before `mouse_init()`
  and `hda_init()`.
- Original `system/timer.c` owns the PIT side and raises the timer IRQ through
  original `ica_hw_interrupt(ICA_MASTER, CPU_TIMER_INT, ...)`.
- Original `host/src/nt_timer.c` owns the host heartbeat and reaches
  `cpu_interrupt(CPU_TIMER_TICK, 0)` only after its original tick/RTC work.
  In the selected CCPU40 form that is the original `c_cpu_interrupt` entry,
  not an adapter-owned interrupt executor.
- Original `host/src/nt_event.c` owns the console event loop, key-history
  update, keyboard lock use, mouse/key forwarding and teardown alert.  Its
  `nt_start_event_thread()` creates the worker suspended; original
  `nt_resume_event_thread()` remains the source-owned resume operation used
  by COMMAND and fullscreen/control paths.

## Modern public input boundary

The existing x86 `console_input_contract_fixture` passed from
`build/M0-T313/S3/host-input-r1`.  It verifies the public console/wait
bindings required by the original event source: invalid input rejection,
input wait-handle acquisition, and session-local video-event publication.
It intentionally does **not** start a long-lived original event worker or
claim the old Console Server registration/menu product shell.

The following historical product-shell operations remain explicit later-owner
or unavailable behavior, rather than a replacement event controller:

- Console Server VDM registration and menu/shortcut controls;
- kernel VDM/CSRSS event brokering;
- WOW-specific console/product-shell integration.

## Build and mirror result

- A clean external Ninja rebuild of `build/M0-T313/S3/r1` completed
  `original-softpc-candidate` at **403/403** selected x86 CCPU40 edges.
- Reached `nt_reset.c` had one necessary host-local width correction that was
  previously unregistered.  It is now source-commented and registered as
  `MVDM-HOST-DIV-145`; it retains `strlen`'s `size_t` result through the
  original bounded comparison and changes no SoftPC/guest ABI.
- No reached `mvdm-host` controller source contains a removable project-owned
  controller implementation.  The only selected external bindings are named
  public Win32 or registered same-shaped interfaces.

## Remaining S3 work

This checkpoint does not yet prove a timer tick delivered through the live
original PIC into a bounded CCPU execution slice, nor a bounded original DMA
port-table delivery.  Those are the remaining positive-controller endpoints;
they must reuse the source chain above and may not introduce a parallel
controller or second executor.
