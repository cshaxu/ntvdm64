# M0 T310 S15 P2 — selected x86 CCPU40 profile boundary

## Question

What is the valid selected x86 CCPU configuration, and must the historical
CPU30/MONITOR machine path remain a product requirement?

## Inputs

- `src/mvdm-host/softpc.new/host/inc/host_def.h`
- `src/mvdm-host/softpc.new/obj.vdm/cdefine.inc`
- `src/mvdm-host/softpc.new/base/ccpu386/ccpudefs.inc`
- `src/mvdm-host/softpc.new/base/ccpu386/ccpusas4.c`
- `tools/build/New-T287OriginalXmsStaticNinja.ps1`
- `tools/build/New-T310CcpuExecutionNinja.ps1`

## Procedure and observations

`host_def.h` selects `CPU_30_STYLE` for the original x86 host path.
`cdefine.inc` selects `CPU_40_STYLE`, `CCPU` and `C_VID` for the non-MONITOR
CCPU path. The CCPU-local `ccpudefs.inc` intentionally carries both CPU30 and
CPU40 definitions.

A forced CPU30-only compile of the CCPU source graph reaches `ccpusas4.c` and
fails because its CPU40 `SasVector`/`TYPE_sas_*` generated contract is absent.
This proves the CCPU graph is not a CPU30-only graph.

The original CCPU configuration, retaining its historical compatibility
defines, produces
`build/M0-T310/S15/ccpu-cpu40/ccpu-bounded-execution.exe` on Win32/x86. The
shared XMS static archive builds under the selected `CPU_40_STYLE`
configuration at `build/M0-T310/S15/xms-static/x86-CPU40/original-xms-common.lib`.

The build retains visible original non-width warnings. No warning suppression
or source repair was introduced for this profile distinction.

## Conclusion

The CPU30/MONITOR path is separate from the CCPU40 source graph. The product
selects only the CCPU40 row; CPU30/MONITOR is neither built nor accepted. The
selected CCPU40 configuration is the working x86 executor baseline. Its
retained `CPU_30_STYLE` definition is an original CCPU compatibility carrier,
not a second selected profile. x64 is deferred.

## Follow-up

Continue S15 with original XMS dispatcher/block/UMB/A20 behavior. T310 uses
one CCPU40 x86 lifecycle acceptance row only.
