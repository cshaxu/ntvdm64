# M0 T288 S3 monitor-frame recovery evidence

## Question

Can the original OpenNT monitor `VDM_TIB`/event state be recovered as a
small, portable copied frame before the SoftPC/SAS package is connected?

## Inputs and procedure

- Exact `v86/monitor/i386/monitor.c`, `monitorp.h`, `vdmtib.h`,
  `vdm.h`, and IEUVDDEx VDM_TIB inspector sources.
- Current selector-blind `adapter-mvdm-host-in` copied #UD record, existing
  `adapter-mvdm-host-out/softpc` original-name register facades, and session
  thread binding.
- An x86 MSVC `/Zs` probe with the original monitor source and its complete
  selected include directories.  The first probe exposed omitted historical
  include directories; the corrected probe reached the original private-tree
  `vdmtib.h` include and the current abbreviated `vdm.h` shadow.

## Observations

- `monitor.c` owns one global `VDM_TIB`, uses its native `CONTEXT` fields to
  call `NtVdmControl(VdmStartExecution)` or CCPU `FastEnterPm`, increments
  the returned EIP, and dispatches every historical event class.
- The original `VDM_TIB` contains host-architecture `CONTEXT` and several
  pointer-bearing printer/DPMI/trace fields.  It is not a fixed-width x64
  component ABI and cannot be copied across the adapter boundary intact.
- The original `vdmtib.h` references the historical private-tree location
  `\\nt\\private\\inc\\vdm.h`; this is a build topology dependency, not an
  authorization to edit the byte-exact mirror or to redefine VDM_TIB.
- The current `adapter-mvdm-host-out/monitor/include/vdm.h` deliberately is a
  small `NtVdmControl` facade.  When selected before the exact platform ABI it
  does not provide monitor-internal `VIRTUALICA` declarations.  Future source
  composition must use a per-target original-platform include order, not make
  the facade pretend to be the complete private header.
- Existing generic #UD input is already the one fixed-width, selector-blind
  machine-to-host event boundary.  A second VDM_TIB-derived event record would
  duplicate that contract without making `monitor.c` executable.

## Interpretation

The smallest valid S3 outcome is the explicit non-duplication decision: retain
the exact original monitor and VDM_TIB declarations as source evidence, retain
the current 16-bit original-name SoftPC facade, and do not invent a second CPU
frame.  The remaining executable mechanics are a single S4 package: its
source-shaped SoftPC/SAS bindings must decide how Bochs supplies register,
mode, memory and event results without exposing a raw `VDM_TIB` or a native
guest pointer.

IEUVDDEx inspection is a separate debugger/VDD target-process contract and is
not a local monitor-state provider.  SIM32 serial/WOW transport likewise does
not become a substitute monitor path.

## Follow-up

S4 audits and recovers only source-proven SoftPC/SAS monitor mechanics.  It
must use a target-specific original platform-ABI include order, preserve
original names where a facade is needed, and leave BOP dispatch, VDD/debugger,
SIM/WOW and kernel VDM behavior with their named owner packages.
