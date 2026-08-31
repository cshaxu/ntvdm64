# M0 T318 S2 P50 — Fixed-container console startup owner cohort

## Scope

This is the source-first prerequisite scan required before another observation
of the fixed `runtime-r33-config-system-media` container.  It follows the
selected original CPU40 entry path rather than treating a new runtime symptom
as a new single-function task:

```
obj.vdm/ntvdm.c::main
  -> CpuEnvInit / nls_init
  -> base/support/main.c::host_main
  -> host/src/nt_reset.c::host_applInit
  -> {host/src/nt_hosts.c::init_host_uis,
      host/src/nt_event.c::nt_start_event_thread}
  -> cpu_init / setup_vga_globals / host_init_screen
  -> host/src/nt_msscs.c::InitialiseDosEmulation
  -> host_start_cpu
```

## Original console/display cohort

The original host bodies themselves own the early console condition:

- `nt_reset.c::host_applInit` selects original `nt_video_funcs`, initializes
  the ICA lock, calls `init_host_uis`, and starts the original event thread.
- `nt_hosts.c::init_host_uis -> InitScreenDesc -> SetupConsoleMode` obtains
  the input/output standard handles, opens `CONIN$`/`CONOUT$` only when a
  stream is redirected, mirrors them into the original SCS standard-handle
  fields, and calls the original public console APIs.
- `nt_event.c::nt_start_event_thread` creates the original keyboard mutex,
  Ctrl-C handler, suspended event thread, and original console events.
- `nt_reset.c::host_reset -> ConsoleInit` retains original text-section,
  focus-event, video setup and console geometry ordering.  The working video
  vector is established by original `host_applInit` before the
  `host_init_screen` macro dispatch.

`InitScreenDesc` and `SetupConsoleMode` are the directly reached
`DisplayErrorTerm` sites: invalid standard handles, failure to open `CONIN$`
or `CONOUT$`, and console-mode/buffer queries.  `nt_event.c` contains the
same original terminal direction for failed event/thread creation.  They are
not private CSR/BaseSrv calls.

The selected `softpc.new/host/src` startup scope has no executable `CSR`
reference.  Its sole `BaseSrv` text occurrence is a historical comment in
`nt_reset.c`; no early host call crosses the BaseSrv/CSR hard boundary.

## Disposition

The fixed non-debug console-owning launcher supplies real `CONIN$` and
`CONOUT$`, precisely satisfying this original owner cohort.  P48's stage
result—an eight-second timeout rather than the former immediate terminal—is
consistent with those direct console prerequisites no longer failing.

No source alteration is justified: replacing `DisplayErrorTerm`, creating a
parallel console abstraction, or moving these calls into app/session would
change the original host ownership.  The current public Win32 console calls
are directly usable and the existing fixed container is their only allowed
runtime binding.

## Next boundary

After the console/display cohort, the remaining pre-NTIO original host work is
the `InitialiseDosEmulation`/SCS initialization chain: original Base VDM
command acquisition, DEM, XMS, DBG, VDD setup, selected NTIO media load and
the CPU entry transaction.  P44--P49 already close the media, XMS and
configuration portions; the next audit isolates the still-unobserved command
acquisition and debugger/VDD branches before a new product is linked and the
fixed container is used once.
