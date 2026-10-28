# LPT selected-provider failure and legacy printer-emulator boundary

Original `softpc.new/base/inc/lq2500.h` (current/OpenNT SHA-256 `42758F3A5E16744FAC7A14A04FA60D4B77467D8D92D8C1ECAADFC3283B02EB5D`) describes an LQ-2500 text-stream printer emulator and declares `Emulate_LQ2500`/`Reset_LQ2500`. A bounded literal search in current MVDM sources and the primary OpenNT MVDM tree found those names only in the header; no selected implementation or exact source consumer was identified. This does not prove no alternate host profile or missing original capability, and the file remains pending. It is not the same owner as passing raw LPT bytes to a device.

Current original `base/comms/printer.c` routes printer-port work to the selected host `nt_lpt.c` provider. Historical T420 S30 ran a real DOS INT17 LPT1 byte `5A` to an observer-owned temporary named pipe, then removed that mapping (`m0-t420-s30-dos-profile-host-medium-closure.md`). That is bounded byte-output evidence, not a physical printer, printer-control status, direct-access VDM IOCTL or ESC/P rendering witness. `shard-3/lpt-status-audit.md` separately records current status-port diagnostic divergence DIV-325 and exact provider/map limitations. Native WOW HDC/raw spool routes are additional owners, not equivalent to this DOS port or Epson emulation.

## Selected-object negative observation

An x86 executable linked current cached `nt_lpt.obj` SHA-256 `209CFD395378F01705EB6D58B5555E5E07AD32C7D8BF9FC92BEB2C1B17FBCECF` and `lpt-flush-selected-object-fixture.c` SHA-256 `D41D781616EEF34543014380E2F6082619392F7571953C94A23BE0DC302D9DE1`. The fixture checks x86 40-byte `HOST_LPT` stride and relevant offsets against actual `flushBuffer` disassembly, sets up a test-owned Windows event handle, and calls the real selected object; Win32 `DeviceIoControl` and `WriteFile` are real, and the event is deliberately not a printer. MinGW i686 `-Wl,--gc-sections` emitted `Warning: corrupt .drectve at end of def file` but built successfully. Result, process exit 0:

```text
direct return=1 remaining=0 last_error=6
buffered return=0 remaining=0 last_error=6
```

The direct case demonstrates that, under an intentionally invalid IOCTL-handle state, original `flushBuffer` discards buffered bytes and returns success after the native call fails; the normal file-write branch reports failure but also clears the buffer. Source comparison shows this ordering is inherited from the original, not a new diff. The probe forcibly sets direct mode and does not establish that the real worker can reach this exact handle/mode combination, that a correctly opened parallel provider fails this way, or that an actual DOS guest observed lost print data. Error 6 is a real Win32 call result for this test handle, not a physical-device failure test. A successful short `WriteFile` is a separate untested boundary.

## Finite follow-up

1. Trace original `host_set_lpt_direct_access`, open/close and guest status/control dispatch to prove whether a direct-mode handle may fail the write IOCTL after it was validly opened, including hot unplug, cancellation and task/worker close. Preserve real printer and native host status as authorities. Validate using actual provider code and controlled faults plus a real test-owned endpoint if available.
2. Test failed IOCTL, short write, failure after buffering, direct/non-direct switch, flush-on-close and repeated shutdown with exact byte counts and error/status observation. Determine guest-visible INT17/port contract before adding retry, error or buffer retention; it is not safe to call historical success semantics corrected merely by changing a return value.
3. If original LQ-2500 emulation is actually required by a supported guest, locate its authentic implementation and full host print/font/lifecycle dependencies, identify an observable guest API path, and compare original output with a disposable test spool. Do not call a raw LPT pipe, the unrelated Win16 HDC witness or a synthetic printer provider an LQ-2500 emulation pass.

No guest, user device, shared ledger or product was modified. Further classification of `lq2500.h` and any original-path deviation remains evidence-bound.
