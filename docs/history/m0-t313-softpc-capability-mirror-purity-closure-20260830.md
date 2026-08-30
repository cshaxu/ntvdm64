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

- [S1 capability and purity closure](../etc/evidence/m0-t313-s1-softpc-capability-purity-closure-001.md)
- [S2 CPU, memory, firmware and reset closure](m0-t313-s2-cpu-memory-firmware-reset-closure-20260830.md)
- [S3 controller lifecycle closure](m0-t313-s3-controller-lifecycle-closure-20260830.md)
- [S4 device lifecycle closure](m0-t313-s4-device-lifecycle-closure-20260830.md)
- [S5 controlled terminal](../etc/evidence/m0-t313-s5-p1-source-shaped-controlled-termination-001.md)
- [S5 original debugger startup binding](../etc/evidence/m0-t313-s5-p3-debugger-startup-binding-001.md)
- [S5 original EMS writeback](../etc/evidence/m0-t313-s5-p4-original-ems-writeback-closure-001.md)
- [S5 final reread](../etc/evidence/m0-t313-s5-p5-bounded-execution-and-purity-reread-001.md)

## Explicit limits

This closure is deliberately not a DOS/WOW workload, BIOS boot, BOP,
BaseSrv/CSRSS, debugger-product, x64, graphics-presenter, or host-device
integration claim.  The original full debugger source is retained but its
private NT4 Debug Subsystem dependency remains owned by queue item 12.

## Next admission

Queue item 1, **Base VDM source-shaped broker recovery**, may be admitted
only under its own bounded Status packet.
