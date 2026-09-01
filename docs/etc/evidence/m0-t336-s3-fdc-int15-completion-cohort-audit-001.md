# M0 T336 S3 — FDC/INT15 completion-cohort audit

## Question

After S2 restored the CCPU40 recursive vector used by the exact
`floppy.c:wait_int` call site, is a further FDC, INT15, ICA, timer, or guest
loader implementation missing from the selected original product closure?

## Result

No new implementation edge is established by the available evidence.

The original completion chain is already selected as a coherent source cohort:

1. `floppy.c:fl_disk_reset` toggles the FDC reset line and calls `wait_int`.
2. `fla.c` implements the reset rising-edge result state, calls
   `fla_hw_interrupt`, and delivers the original `ica_hw_interrupt(0,
   CPU_DISKETTE_INT, 1)` request.
3. `wait_int` first runs the original NTIO-supplied INT15 device-busy entry,
   then runs the NTIO-supplied wait entry through the now-installed CCPU40
   recursive vector.
4. The corresponding original interrupt completion is
   `floppy_i.c:diskette_int`: it marks `SS_INT_OCCURRED`, acknowledges the
   ICA, runs the INT15 interrupt-complete entry, and restores its saved CPU
   state.
5. `ntvdm.c:main` calls `TimerInit`; normal BIOS reset reaches
   `timer.c:timer_init`, which creates the original heartbeat through
   `nt_timer.c:host_timer_init`; `kb_setup_vectors` resumes that heartbeat
   after NTIO has supplied the guest vector table.  Its `CPU_TIMER_TICK`
   delivery is the original CCPU40 `c_cpu_interrupt` path.

`nt_rflop.c` is selected for actual host floppy media operations, but is not
a prerequisite for the FDC reset interrupt above.  In the fixed profile,
`config.c` starts with zero admitted physical floppy drives; the historical
`stubs.c:host_floppy_init` is therefore not a correct target for a speculative
replacement.  It is an original source terminal for an unadmitted physical
floppy feature, not evidence that the default startup must emulate one.

## Guest-media boundary

The INT15 and wait entries are not host-generated firmware addresses.
Original `nt_bop.c:MS_bop_F` calls `keybd_io.c:kb_setup_vectors`, which takes
the table supplied by the already loaded `NTIO.SYS` image.  The product's
`nt_msscs.c:InitialiseDosEmulation`/`AddSystemFiles` path resolves the selected
media beneath `build/output/dos` and copies it into the SoftPC SAS span before
CPU start.  Thus the missing earlier vector was a host CCPU initialization
edge, not a missing DOS or Win16 guest loader.

## Verification and limit

- The S2 x86 map resolves the former native return address to
  `floppy.c:wait_int`, not to a guest loader or BOP provider.
- The S2 paired formal x86/x64 links include the original `fla.c`,
  `floppy.c`, `floppy_i.c`, `ica.c`, `timer.c`, `nt_timer.c`, `nt_rflop.c`,
  `keybd_io.c`, and CCPU40 body.
- The unchanged fixed x86 container remains live through its bounded timeout
  after the vector repair; it no longer reports the former null target.
- The external fixed observer now suspends the already launched process only
  at that timeout and records its native x86 contexts before the pre-existing
  watchdog termination.  With the current formal x86 product, the primary
  thread and six companion threads are all in system wait stubs; no exception
  is reported.  This is a host wait-state observation, not guest-memory
  inspection, a debugger attachment, or a product alteration.

The timeout alone does not prove which later guest instruction is executing.
It therefore does not authorize an FDC rewrite, a physical-floppy provider,
or a new BOP.  In particular, the current all-thread wait snapshot cannot
attribute a particular wait object to `wait_int`, the heartbeat, or another
original host worker.  A later source-defined wait-owner attribution is needed
before another source cohort is changed.
