# T473 S10: selected device and video base headers

This checkpoint extends the [five-header CCPU observation](base-inc-role-review.md)
to 13 actual original BIOS, RTC, keyboard, mouse, PIC, timer, floppy, video,
host and serial production translation units. The retained
[preprocessing runner](base-inc-multi-preprocess.ps1) used their actual Ninja
commands and left cached objects and the worker image unchanged. The
[observer](base-inc-multi-observer.py) binds VALID dependency records, exact
original/frozen hashes, both audit catalogs, preprocessing output hashes and
each emitted declaration context in [observations](base-inc-multi-observations.json).

Thirty byte-identical original headers emit declarations in at least one of
these contexts and are **partial**. Their individual context lists and finite
remaining obligations are in [dispositions](base-inc-multi-role-dispositions.csv):

| Area | Individual original files | Remaining behavior |
| --- | --- | --- |
| BIOS, control, CPU and RTC | `cmos.h`, `cntlbop.h`, `cpu.h`, `rtc_bios.h`, `virtual.h` | BIOS/RTC state and CPU/BOP contracts, mode changes, errors and guest behavior. |
| Video and graphics | `cga.h`, `egagraph.h`, `egamode.h`, `egaports.h`, `egaread.h`, `ga_defs.h`, `ga_mark.h`, `gvi.h`, `vgaports.h`, `video.h` | Register/plane layout, selected providers, dirty updates, rendering, mode transitions and teardown. |
| Floppy and disk | `fla.h`, `gfi.h`, `gfitest.h` | Drive and media identity, commands, errors and real/virtual-media boundaries; declarations alone do not activate test-mode behavior. |
| Keyboard and mouse | `host_nls.h`, `keyba.h`, `keyboard.h`, `mouse.h`, `mouse_io.h`, `ppi.h` | Input/IRQ ordering, scan and callback state, errors, reset and cleanup. |
| Scheduler and timing | `idetect.h`, `quick_ev.h`, `timestmp.h`, `tmstrobe.h` | Idle transitions, queue/timer ordering, reentry, cancellation and timing behavior. |
| COM and diagnostics | `rs232.h`, `trace.h` | Real serial peer and lifecycle; diagnostic guards and actual selected consumers. |

Source text retention is 100% for these exact originals; selected declarations
alone do not provide a defensible restored-behavior percentage. The other 111
pending headers are not excluded: macros and other translation units remain to
be audited. No product or guest image was changed by this checkpoint.
