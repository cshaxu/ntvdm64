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

- [P7](../etc/evidence/m0-t313-s2-p7-original-lifecycle-witness-001.md)
- [P8](../etc/evidence/m0-t313-s2-p8-memory-a20-and-umb-boundary-001.md)
- [P9](../etc/evidence/m0-t313-s2-p9-firmware-resource-mirror-cleanup-001.md)
- [S1 ledger](../etc/evidence/m0-t313-s1-softpc-capability-purity-ledger-001.tsv)

## Transfer

S3 takes `base/system` and `host/src/{nt_timer,nt_eoi,nt_event}` controller
lifecycle, including the reset-tail effects. It must retain original owner
bodies and same-shaped bindings; it may not replace reset with an adapter
lifecycle.
