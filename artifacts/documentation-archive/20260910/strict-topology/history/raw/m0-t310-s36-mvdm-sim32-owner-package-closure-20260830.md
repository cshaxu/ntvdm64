# M0 T310 S36 closure — MVDM SIM32 owner package

S36 directly composed the selected original `softpc.new/host/src/sim32.c`
under the sole x86 CCPU40 profile.  It formally added the pre-existing
same-shaped `NtVdmControl(VdmQueryDir)` monitor binding to the original
SoftPC graph and selected the exact original `sas_overwrite_memory` facade as
a narrow mirror overlay because full `ntstubs.c` duplicates alternate CPU/FPU
state.  The fresh 384-edge formal graph contains no SIM32, SAS-overwrite or
`NtVdmControl` unresolved symbol.  The focused x86 monitor fixture passes.

No kernel VDM, MONITOR process route, Bochs fallback, generic memory shim or
new mapping manager was introduced.  DPMI32 is the next owner package; global
machine behavior remains S49.
