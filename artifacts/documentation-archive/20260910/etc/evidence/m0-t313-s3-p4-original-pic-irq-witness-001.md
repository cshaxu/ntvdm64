# M0 T313 S3 P4 — Original PIC IRQ Witness

## Scope

This checkpoint extends the existing bounded x86 CCPU40 controller fixture
with the selected original `softpc.new/base/system/ica.c` body.  It proves the
8259 initialization, IRQ request and acknowledgement contract only.  It does
not claim timer-thread, EOI-delay, WOW scheduler or guest BIOS completion.

## Selected original sequence

The fixture uses the same order as the original BIOS/reset callers:

1. `ica0_init(); ica0_post();`
2. `ica1_init(); ica1_post();`
3. `ica_hw_interrupt(ICA_MASTER, CPU_TIMER_INT, 1);`
4. `ica_intack(&hook_address);`

The original `ica.c` body installs PIC port handlers into original `ios.c`,
unmasks the NTVDM master/slave PICs through its ICW/OCW sequence, records IRQ0,
and returns vector `08h` from the original acknowledge routine.

## Result

Fresh external x86 CCPU40 build and execution:

`build/M0-T313/S3/ccpu-controller-r2/ccpu-lifecycle.exe`

completed with exit code zero.  The asserted original PIC result is vector
`08h` for `CPU_TIMER_INT`.

`ica.c` also calls the historical `WOWIdle(FALSE)` notification once while
raising the interrupt.  The bounded test records that boundary; it supplies
no WOW scheduling implementation and asserts that no EOI hook is entered.
The test-only link forms are hit-counting, asserted boundaries, not product
providers.  The real source owner remains `nt_eoi.c`/the later WOW scheduler
package.

## Disposition

DMA/I/O and PIC initialization/delivery now have positive original-body
evidence.  The remaining S3 timer endpoint is the original
`nt_timer -> time_tick -> TimerGenerateMultipleInterrupts -> PIC` chain; its
thread/lifecycle source must be connected without turning the reached WOW/EOI
boundary into a local substitute.
