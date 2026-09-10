# M0 T313 S3 P3 — Original DMA And IOS Witness

## Scope

This checkpoint extends the existing bounded x86 CCPU40 fixture.  It does not
add a test-only DMA implementation, a second executor, or a new device
backend.

## Selected original bodies

- `softpc.new/base/support/ios.c`: original I/O table initialization and
  `inb`/`outb` dispatcher.
- `softpc.new/base/system/at_dma.c`: original `dma_init`, `dma_post`,
  controller register and page-register handlers.

The old fixture-only definitions of the I/O table and dispatch globals were
removed.  The fixture now receives those globals only from original `ios.c`.

## Bounded result

The fresh external graph at `build/M0-T313/S3/ccpu-controller-r1` compiled,
linked and ran `ccpu-lifecycle.exe` on x86 CCPU40.

The live fixture performed this source-owned sequence:

1. `io_init(); dma_init(); dma_post();`
2. verified that the original DMA controller and page-register ranges are
   associated with `DMA_ADAPTOR` / `DMA_PAGE_ADAPTOR` in original I/O tables;
3. used original `outb`/`inb` dispatch to write/read the two-byte channel-1
   address register and a floppy page register;
4. continued through the pre-existing original CCPU/SAS/FPU and recursive
   `host_simulate` bounded proof.

The fixture exited zero.  It deliberately does not issue a device DMA request
or claim storage/floppy behavior; those belong to their original device
owners and the later S4 device cohort.

## Remaining S3 scope

Timer heartbeat suspension/resume, ICA lock and console-input binding are
already witnessed.  The remaining S3 positive endpoint is a bounded live
original timer delivery through the timer/PIC/CCPU chain.  It must retain the
existing `nt_timer → time_tick → TimerGenerateMultipleInterrupts →
ica_hw_interrupt` order and may not create a parallel timing controller.
