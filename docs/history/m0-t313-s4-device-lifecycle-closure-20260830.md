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

- [P1 package assembly](../etc/evidence/m0-t313-s4-p1-device-package-assembly-001.md)
- [P2 reset order and mirror cleanup](../etc/evidence/m0-t313-s4-p2-device-reset-order-and-mirror-cleanup-001.md)
- [P3 endpoint reconciliation](../etc/evidence/m0-t313-s4-p3-device-endpoint-reconciliation-001.md)
- [T313 S3 controller closure](m0-t313-s3-controller-lifecycle-closure-20260830.md)

## Deliberate non-claims

This does not assert a live display window, media read/write, serial traffic,
printer shell, continuously ticking PIT, guest boot, DOS/WOW workload or a
second executor.  Those require the S5 bounded execution/teardown acceptance
path and later product integration; they must not be supplied by a local
fixture or a fabricated device shim.
