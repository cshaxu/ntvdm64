# M0 T377 S3 — Timer, IRQ and video owner recovery

## Source/binding result

The formal CPU40/x86 product already selects the whole original immediate
timer/IRQ/video cohorts needed by the T377 matrix:

- host heartbeat: `softpc.new/host/src/nt_timer.c`, including
  `host_timer_init`, `TimerInit`, `Win32_host_timer`, suspend/resume and the
  original timer-thread failure path;
- controller and timer mechanics: original `softpc.new/base/system/timer.c`,
  `ica.c`, `qevnt.c`, `rom.c`, `cmosnt.c` and the selected original host EOI
  owner;
- display mechanics: the complete original `base/video` source group and the
  generated `base/cvidc` group, rather than a substitute video memory
  controller;
- host presentation: original `nt_graph.c` with the existing narrow public
  Console/session DIB binding.  This replaces only the private NT4
  `CONSOLE_GRAPHICS_BUFFER` allocation protocol; original VGA/CGA/EGA writes,
  palette and close order remain the owners.

The source diffs are limited to registered binding points: public-APC
termination equivalence in `nt_timer.c` (`MVDM-HOST-DIV-040`), its bounded
timer remainder typing (`MVDM-HOST-DIV-050`), required C-video declaration and
host-word contracts, and the named presentation adapter
(`MVDM-HOST-DIV-176`).  No timer, PIC, IRQ, C-video or video controller was
reimplemented in an adapter.  S2's formal rebuild linked all selected libraries
and `original-softpc-process.exe` successfully.

## Runtime discipline

Three exploratory fixed-root executions were made while establishing the
matrix:

- `MEM.EXE` was opened by the original DEM file path and the process returned
  zero;
- `KB16.COM US` was opened and the process returned zero, but this particular
  no-input invocation did not reach a `KEYBOARD.SYS` open; it is not keyboard
  installation proof;
- `GRAPHICS.COM` opened both itself and `GRAPHICS.PRO`, then produced product
  result `0x48`.

These are **not S5 acceptance observations**.  They used different optional
diagnostic environment variables and one graphics run timed out after the
diagnostic mode changed.  They cannot be compared as one fixed runtime
container.  The reports remain in `O:\\ntvdm64` as non-closure evidence;
they are not used to infer a controller defect or to authorize a leaf repair.

## Disposition

S3 is source/binding closed.  S4 owns the one formal immutable-media contract
and a no-diagnostic frozen observation contract.  Only then may S5 make the
bounded workload observations and classify the graphics `0x48` result against
original source.
