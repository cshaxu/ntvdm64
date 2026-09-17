# M0 T420 S4 — SoftPC system package recovery

## Scope

The selected original package is the eleven-unit manifest in
`src/mvdm/softpc.new/base/system/sources`:

`at_dma.c`, `cmosnt.c`, `ica.c`, `idetect.c`, `illegalp.c`, `qevnt.c`,
`rom.c`, `timer.c`, `timestrb.c`, `dummy_nt.c`, and `unexp_nt.c`.

It owns original timer/time-strobe scheduling, quick events, 8259 PIC/ICA,
CMOS, DMA, ROM initialisation and device/reset-side lifecycle.  It does not
own the public Console input binding (S5) or the worker host-thread provider
(S12).

## Source-first disposition

Nine units are now byte-identical to the pinned OpenNT source: `at_dma`,
`cmosnt`, `ica`, `illegalp`, `qevnt`, `timer`, `timestrb`, `dummy_nt`, and
`unexp_nt`.

`idetect.c` differs only in its `vdm.h` include spelling.  The original
relative include was valid before the source-tree recomposition; `<vdm.h>`
resolves the identical public OpenNT header through the formal include path.

`rom.c` retains `MVDM-HOST-DIV-165`, a compile-profile selection of the
unchanged original generic `rom_init()` body.  Historical NTVDM+X86GFX
expected a kernel-VDM product shell to pre-reside the ROMs.  The standalone
CPU40 worker has no such shell; selecting the original generic body retains
the original resource lookup, read, SAS RAM copy and ROM-protection ordering
without adding a replacement loader.  The divergence is registered in
`src/mvdm/README.md`.

This S removes 41 mirror-side autonomous/diagnostic lines: the default-off
PIC/ICA event witnesses and illegal-instruction observer no longer alter the
selected original bodies.  No overlay, adapter policy, or mirror file was
added.

## Lifecycle proof

Original BIOS reset calls the system initialisers and post functions for ICA,
CMOS and timer.  `timer_init()` enters the selected host timer provider;
timer events enter the original time-strobe and quick-event chain, then the
original ICA delivery path.  CPU40 observes `CPU_SIGALRM_EXCEPTION_MASK` and
invokes the original host timer event path.  This preserves package ownership:
the system package supplies policy, while the existing finite host provider
supplies the unavailable Win32 thread/wait mechanism.

## Verification

`build/M0-T420/S4/formal-x86-001` is the fresh x86 formal graph.

- `original-softpc-system.lib`: passed after source restoration.
- `product-programs`: produced and linked x86 `run16.exe`, `basesrv.exe`,
  `ntvdm.exe`, and `dtmgr.exe`.
- `Verify-VdmTibStorage.mjs`: passed for the newly linked worker map.
- `ccpu-halt-reset-test.exe`: passed; the original CCPU HALT/RESET fixture
  reported `AX=beef`, producer `0`, and exit `0`.
- Deployed `O:\winnt` regression: direct MEM, repeated MEM, nested
  COMMAND→MEM, `COMMAND /c ver`, and EDIT all passed.  The nested test also
  corrected a test-only Console prompt detector: it now scans the visible
  buffer for the already-painted drive-qualified prompt rather than assuming
  the public cursor has settled on that same row.  No product or guest state
  is involved in that observation correction.
- Six Console geometry cases and five short-window EDIT→MEM cases passed;
  logs use `m0-t420-s4-video-v2-*` under `O:\winnt\logs`.

## Next owner

S5 owns the complete `softpc.new/base/keymouse` manifest.  S12 owns the
worker host thread/timer binding implementation; neither may reintroduce
PIC/timer policy into an adapter.
