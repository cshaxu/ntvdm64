# M0 T313 S4 P2 — Original Device Reset Order And Mirror Cleanup

## Original owner sequence

The selected original BIOS reset body is
`mvdm-host/softpc.new/base/bios/reset.c`.  Its `reset()` sequence is the S4
device-package root; no project device startup dispatcher is introduced.

In source order it performs:

1. `ica0_init`/`ica0_post`, then `ica1_init`/`ica1_post`;
2. CMOS/PPI/configuration and BIOS-data-area setup;
3. `gvi_init`, `timer_init`/`timer_post`, keyboard initialization, then
   `video_init`;
4. serial `com_init`/`com_post` for every configured adapter;
5. `dma_init`/`dma_post`, `fla_init`, mouse and hard-disk initialization;
6. `printer_init`/`printer_post` only under the original `PRINTER` condition.

The corresponding original definitions are retained in the selected packages:

- `base/video/{video.c,gvi.c,cga.c,ega_prts.c,vga_prts.c}`;
- `base/system/timer.c` with `host/src/nt_timer.c`;
- `base/disks/fla.c`;
- `base/comms/{com.c,printer.c}`;
- `base/system/{ica.c,at_dma.c}`.

`host/src/stubs.c` also contains empty video forms only inside its original
`MONITOR` conditional block.  The selected x86 CCPU40 profile does not define
`MONITOR`; these inactive original declarations neither replace nor compete
with `base/video` and are retained as mirror material.

## Reached mirror cleanup

The reached reset body was compared with
`O:/repos.external/opennt-src-2/nt/private/mvdm/softpc.new/base/bios/reset.c`.
The selected x86 profile does not need two prior x64-only carrier changes:

- four scalar `config_inquire` conversions had been widened through
  `ULONG_PTR`;
- an extra declaration of `cmos_clear_shutdown_byte` had been inserted.

Both changes were removed.  `reset.c` is again byte-shaped as its source
baseline for these locations, and `MVDM-HOST-DIV-086` is removed from the
mirror register.  The remaining `MVDM-HOST-DIV-141` register entry continues
to cover only other reached files where pointer-width treatment is still an
explicitly registered concern; it no longer names `reset.c`.

## Formal verification

A fresh external selected Win32/x86 CCPU40 Ninja graph completed all 403
candidate edges after the cleanup:

```text
build/M0-T313/S4/reset-mirror-r1
target: original-softpc-candidate
result: 403/403, exit 0
```

The build deliberately leaves original diagnostics visible.  It is compile
and source-order evidence, not a claim that BIOS reset or all device endpoints
have run in a live guest.
