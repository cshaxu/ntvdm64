# M0 T313 S3 closure — Controller lifecycle

## Result

S3 is closed for the selected MSVC Win32/x86 CCPU40 controller cohort.  It
does not claim a full PIT period, complete device startup, a DOS boot, or a
WOW scheduler.

## Exit-criterion audit

- **Original controller owners:** `ios.c`, `at_dma.c`, `ica.c`, `nt_timer.c`,
  `nt_eoi.c`, reset/input forms and their original call ordering remain the
  selected source forms.  No controller implementation was moved into an
  adapter or test.
- **Timer/interrupt bounded behavior:** original `DelayHeartBeat` suspension
  and resume previously pass; original PIC master/slave initialization,
  IRQ0 request and `ica_intack` return `08h` now pass.  The reached
  `WOWIdle(FALSE)` is recorded as an original later-owner notification, not a
  test substitute.
- **DMA/I/O:** original `ios.c` and `at_dma.c` install and service the
  controller/page-register ranges through original `inb`/`outb`; obsolete
  fixture I/O-table definitions were removed.
- **Input:** the source-shaped public console input binding witness passes on
  x86.  It does not claim the NT4 Console Server product shell.
- **Formal closure:** fresh external `r2` completed all 403 selected CCPU40
  graph edges and includes both original timer translation units.
- **Mirror and cleanup review:** `timer.c` and `ica.c` are exact mirrors;
  the only `nt_timer.c` differences are registered MVDM-HOST-DIV-040 and
  MVDM-HOST-DIV-050.  No removable project-owned production logic was found
  in the reached MVDM-host paths.

## Deliberate transfer

The controlled link probe proves `timer.c` is a complete PIT/EOI/device
cohort, not a standalone tick helper: waveform, BDA initialization, floppy
DOR, EOI delay/registration and PIF policy are required.  S4 therefore owns
the whole original device package for any live full-period endpoint.  It must
not reintroduce a fixture-local timer, controller shim or parallel executor.

## Evidence

- [S3 P1](../etc/evidence/m0-t313-s3-p1-controller-source-assembly-001.md)
- [S3 P2](../etc/evidence/m0-t313-s3-p2-controller-chain-and-input-binding-001.md)
- [S3 P3](../etc/evidence/m0-t313-s3-p3-original-dma-ios-witness-001.md)
- [S3 P4](../etc/evidence/m0-t313-s3-p4-original-pic-irq-witness-001.md)
- [S3 P5](../etc/evidence/m0-t313-s3-p5-original-timer-package-formal-closure-001.md)
- [S3 P6](../etc/evidence/m0-t313-s3-p6-original-timer-device-boundary-001.md)
