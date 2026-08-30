# M0 T313 S3 P6 — Original Timer Device Boundary

## Question

Can `timer.c:TimerGenerateMultipleInterrupts` be isolated as a small timer
test endpoint between the already proven heartbeat and PIC bodies?

## Controlled source-graph probe

The existing bounded CCPU40 controller fixture was temporarily configured to
compile original `softpc.new/base/system/timer.c`, reference only
`TimerGenerateMultipleInterrupts(1)`, and retain unreferenced functions as
separate COMDATs.  No source body was changed and no test/provider shim was
added for the timer package.

The x86 link reached the original selected `timer.obj` and stopped on these
real package interfaces:

- `host_timer2_waveform`
- `host_timer_init`
- `host_GetSysTime`
- `fla_outb`
- `RegisterEOIHook`
- `host_DelayHwInterrupt`
- `dwWNTPifFlags`

The result demonstrates that the original timer source translation unit is a
single PIT/EOI/device cohort, not a leaf wrapper around `ica_hw_interrupt`.
In particular it carries waveform output, BDA time initialization, floppy DOR
output, EOI registration/delay and PIF timing policy in the same original
package.

## Disposition

The temporary fixture/build edits were completely reverted after the probe;
there is no incomplete timer source selection, synthetic symbol, warning
suppression, or product implementation left in the worktree.

S3 retains its bounded original evidence for heartbeat suspend/resume, PIC
IRQ0 request/acknowledgement, DMA/I/O and console input.  A live complete
timer-period endpoint must be admitted only with the whole original PIT/EOI
device cohort, rather than through a fixture-local implementation or a chain
of artificial link seams.
