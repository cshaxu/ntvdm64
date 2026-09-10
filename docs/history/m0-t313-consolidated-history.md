# m0-t313 historical record

Consolidated task record. Raw contemporaneous records are retained in the external documentation archive.

## m0-t313-s2-cpu-memory-firmware-reset-closure-20260830.md

# M0 T313 S2 closure — CPU, memory, firmware, and reset lifecycle

## Result

S2 is closed for the selected MSVC Win32/x86 CCPU40 source-shaped lifecycle.
It does not claim a completed DOS boot, controller endpoint, or guest service
run; those belong to S3--S5.

## Exit-criterion audit

- **Original lifecycle order:** `obj.vdm/ntvdm.c → host_main → cmos_pickup →
  c_cpu_init → InitialiseDosEmulation → reset → host_start_cpu` is retained
  and recorded in P7. The executor is original CCPU40; there is no Bochs,
  CPU30, MONITOR, V86, or second executor.
- **CPU/SAS bounded behavior:** the current x86 witness executes original SAS
  vectors, CCPU decode, RAM write, FPU result, `host_start_cpu`, and recursive
  `host_simulate`, returning through the original `D6 FE` path with a typed
  session result.
- **Memory and A20 boundary:** P8 proves the XMS-shaped A20 facade and the
  mapping-manager physical-page/DOS-alias positive and negative cases.
  Original EMS/XMS/UMB bodies are selected directly; no fixture allocator is
  introduced.
- **Firmware/resource/reset:** original BIOS/system/resource/reset bodies are
  selected. `nt_rez.c → nt_unix.c::host_find_file` retains the original lookup
  shape over the session firmware root. `reset()` retains its original NT
  `host_reset → q_event_init` tail; S3 owns its controller effects.
- **Fresh formal evidence:** external r3 completed the 403-edge selected
  candidate and forced-link audit. Genuine later-owner unresolved symbols
  remain visible rather than being replaced with product shims.
- **Immediate cleanup:** P9 removed two non-original warning-suppression
  assignments in reached `nt_rez.c`; its original warnings are visible again.
  The reached S2 scan found no other removable project-owned production form.

## Evidence

- [P7](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t313-s2-p7-original-lifecycle-witness-001.md)
- [P8](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t313-s2-p8-memory-a20-and-umb-boundary-001.md)
- [P9](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t313-s2-p9-firmware-resource-mirror-cleanup-001.md)
- [S1 ledger](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t313-s1-softpc-capability-purity-ledger-001.tsv)

## Transfer

S3 takes `base/system` and `host/src/{nt_timer,nt_eoi,nt_event}` controller
lifecycle, including the reset-tail effects. It must retain original owner
bodies and same-shaped bindings; it may not replace reset with an adapter
lifecycle.


## m0-t313-s3-controller-lifecycle-closure-20260830.md

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

- [S3 P1](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t313-s3-p1-controller-source-assembly-001.md)
- [S3 P2](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t313-s3-p2-controller-chain-and-input-binding-001.md)
- [S3 P3](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t313-s3-p3-original-dma-ios-witness-001.md)
- [S3 P4](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t313-s3-p4-original-pic-irq-witness-001.md)
- [S3 P5](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t313-s3-p5-original-timer-package-formal-closure-001.md)
- [S3 P6](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t313-s3-p6-original-timer-device-boundary-001.md)


## m0-t313-s4-device-lifecycle-closure-20260830.md

# M0 T313 S4 — Device Lifecycle Closure

## Result

Closed for the selected original Win32/x86 CCPU40 SoftPC profile.

## What is closed

- Original BIOS `reset()` is the sole reached device startup owner and retains
  the original ICA, timer, video, serial, DMA, floppy and conditional printer
  ordering.
- The complete original `system`, `video`, `cvidc`, `disks` and `comms`
  package source manifests are present in the formal candidate graph.
- Video/C-video, storage and communications each retain a bounded original
  endpoint or an exact condition-unselected/unavailable result.  No device
  controller, media backend, serial backend or printer backend was rewritten
  in an adapter.
- Reached `mvdm-host` reset mirror drift was immediately removed: four
  obsolete `ULONG_PTR` scalar casts and the extra CMOS declaration are no
  longer in the mirror or its exception register.
- The fresh selected formal candidate
  `build/M0-T313/S4/reset-mirror-r1` completed 403/403 edges.

## Evidence

- [P1 package assembly](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t313-s4-p1-device-package-assembly-001.md)
- [P2 reset order and mirror cleanup](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t313-s4-p2-device-reset-order-and-mirror-cleanup-001.md)
- [P3 endpoint reconciliation](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t313-s4-p3-device-endpoint-reconciliation-001.md)
- [T313 S3 controller closure](../../artifacts/documentation-archive/20260910/strict-topology/history/raw/m0-t313-s3-controller-lifecycle-closure-20260830.md)

## Deliberate non-claims

This does not assert a live display window, media read/write, serial traffic,
printer shell, continuously ticking PIT, guest boot, DOS/WOW workload or a
second executor.  Those require the S5 bounded execution/teardown acceptance
path and later product integration; they must not be supplied by a local
fixture or a fabricated device shim.


## m0-t313-softpc-capability-mirror-purity-closure-20260830.md

# M0 T313 closure — SoftPC capability and mirror-purity acceptance

## Result

`M0 T313 S1-S5` is closed for the selected MSVC Win32/x86 `/MT` CCPU40
original SoftPC profile.

- One original SoftPC/CCPU40 executor is selected.  There is no Bochs,
  CPU30/MONITOR, kernel-VDM or `src.old` production fallback.
- Original CPU/SAS/FPU, EMS/UMB, firmware/reset, controller, video/storage
  and communications owner packages are formally assembled.  Each selected
  machine cohort has bounded positive or explicit unavailable evidence.
- Original `host_start_cpu()` performs a finite CCPU execution and returns;
  the source-shaped `TerminateVDM` chain completes the current session rather
  than terminating the host process.
- Reached mirror differences were either removed immediately or remain
  registered source-contract divergences.  The final EMS writeback recovery
  selects the original bodies, not a replacement adapter.

## Evidence

- [S1 capability and purity closure](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t313-s1-softpc-capability-purity-closure-001.md)
- [S2 CPU, memory, firmware and reset closure](../../artifacts/documentation-archive/20260910/strict-topology/history/raw/m0-t313-s2-cpu-memory-firmware-reset-closure-20260830.md)
- [S3 controller lifecycle closure](../../artifacts/documentation-archive/20260910/strict-topology/history/raw/m0-t313-s3-controller-lifecycle-closure-20260830.md)
- [S4 device lifecycle closure](../../artifacts/documentation-archive/20260910/strict-topology/history/raw/m0-t313-s4-device-lifecycle-closure-20260830.md)
- [S5 controlled terminal](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t313-s5-p1-source-shaped-controlled-termination-001.md)
- [S5 original debugger startup binding](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t313-s5-p3-debugger-startup-binding-001.md)
- [S5 original EMS writeback](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t313-s5-p4-original-ems-writeback-closure-001.md)
- [S5 final reread](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t313-s5-p5-bounded-execution-and-purity-reread-001.md)

## Explicit limits

This closure is deliberately not a DOS/WOW workload, BIOS boot, BOP,
BaseSrv/CSRSS, debugger-product, x64, graphics-presenter, or host-device
integration claim.  The original full debugger source is retained but its
private NT4 Debug Subsystem dependency remains owned by queue item 12.

## Next admission

Queue item 1, **Base VDM source-shaped broker recovery**, may be admitted
only under its own bounded Status packet.

