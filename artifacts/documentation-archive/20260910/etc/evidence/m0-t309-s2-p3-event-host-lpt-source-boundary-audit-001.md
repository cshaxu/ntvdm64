# M0 T309 S2 P3 — original event, host-UI and LPT source-boundary audit

## Question

Can the original `nt_event.c`, `nt_hosts.c`, and `nt_lpt.c` group be composed
as one direct completion of the DEM/COMMAND provider cohort?

## Inputs and procedure

The physical residual ledger was traced back to all original caller bodies and
their complete translation-unit include/call boundaries.  The audit considers
the three source files as one historical host endpoint group rather than
selecting only `nt_std_handle_notification`, `nt_block_event_thread`, or
`host_lpt_flush_initialize`.

## Observations

- `nt_event.c` owns a full Console input/event worker: keyboard and mouse
  queues, control-C policy, console wait/suspend events, full-screen/display
  calls, event-thread lifecycle, and callbacks into original SoftPC CPU/BIOS
  mechanics.  It includes private Console, keyboard, USER and VDM headers as
  well as `CpuH`; it is not a small standard-stream provider.
- `nt_hosts.c` obtains and changes the process standard handles, recreates
  `CONIN$`/`CONOUT$`, records `SCS_hStd*`, changes console modes, and initializes
  display/palette state.  The historical `SetStdHandle` process-global behavior
  conflicts with the product rule that the app's own stdin/stdout/stderr remain
  independent of the single MVDM session.
- `nt_lpt.c` has two distinct original paths.  Buffered ordinary printer/file
  output uses public file APIs, while direct port access uses
  `IOCTL_VDM_PAR_*`, `Monitor*Printer*`, SAS storage and original timer/config
  state.  A modern public printer endpoint cannot silently stand in for the
  direct monitor/port semantics.
- The only reached COMMAND callers are lifecycle forms: standard-handle
  notification/block/resume/push-exit and LPT flush initialization.  Their
  presence does not make the rest of the historical event/UI/port executor
  safe to activate.

## Disposition

No source body from this group enters the current T309 Bochs-only provider
graph.  The exact later owners are:

- a session-owned Console/standard-stream lifecycle adapter under
  `adapter-mvdm-host-out/win32`, preserving the reached source call shapes
  without changing global app standard handles;
- the selectable original-SoftPC backend for `CpuH`, SAS and original callback
  mechanics; and
- a bounded LPT endpoint capability plus an explicit unavailable/direct-port
  decision under the same SoftPC/Bochs machine contract.

The original files remain full mirror evidence.  T309 neither crops them nor
turns their reached functions into isolated COMMAND shims.

## Confidence and follow-up

High confidence for ownership and the direct call relationships: all are
visible in the original translation units and confirmed by the physical
provider-residual ledger.  This is a source-boundary disposition, not a claim
that console, keyboard, display or printer behavior is currently runnable.
The remaining P3 source group is `nt_msscs.c` cross-owner startup/VDD
composition.
