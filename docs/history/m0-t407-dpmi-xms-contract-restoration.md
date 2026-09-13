# M0 T407 DPMI/XMS contract restoration

## Closure

M0 T407 is closed after S1–S14. It restored the admitted original XMS/DPMI
contracts, split CCPU descriptor domains, retired unsupported kernel Fast
carriers, recovered final IDT publication and corrected the immediate DOSX
startup regression caused by removing the `0040h` fixed-selector carrier.

The final result is recorded in the [S14 recovery evidence](../etc/evidence/m0-t407-s14-fsti-fixed-selector-recovery.md).
This is a bounded worker-host closure, not an assertion that `WRITE.EXE` runs.

## Delivered boundary

- Original DPMI/XMS owner algorithms remain in `mvdm-host`; standalone-only
  mechanics remain finite registered bindings.
- The original `AllocateExceptionStack → FSTI → SwitchToRealMode` route is
  now observed through the BOP `FDh` provider. No early `SEL_VDMTIB`
  publication was invented.
- MEM and COMMAND regression rows complete with exit `0`; WRITE reaches its
  later exit-255 WOW frontier without diagnostic code.

## Explicit receivers

| Residual | Receiver |
| --- | --- |
| CCPU table/TR and worker event lifecycle | [CCPU40 C-VID/event restoration](../proposals/proposal-ccpu40-cvid-event-path-restoration-001.md) |
| WOW callbacks, TEB/TD and debugger/resource boundaries | [WOW/debugger restoration](../proposals/proposal-wow-debugger-original-owner-restoration-001.md) |
| Integrated WRITE execution | [single-process WOW16 lifecycle recovery](../proposals/proposal-wow16-single-process-lifecycle-recovery-001.md) |

These are assigned, unadmitted candidates. They may not use T407 closure as
evidence that their unproved kernel or WOW behavior already works.
