# M0 T308 S1 — Monitor / NtVdmControl contract evidence

## Question

Can the directly reached user-mode monitor subset retain its original shapes
without importing NT4 kernel VDM service behavior or a second machine executor?

## Inputs and procedure

The checked source is the existing `VDMSERVICECLASS`, `VDMQUERYDIRINFO` and
thread-local `VDM_TIB` carrier in `adapter-mvdm-host-out/monitor`, together
with original MVDM call sites and the kernel-contract ledger. The dedicated
T308 Ninja graph compiles the monitor source, its session carrier and the
adapter-private OpenNT TEB binding with MSVC `/MT` under x86 and x64. Two
fixtures test the callback/status form and the bound-thread `VDM_TIB` carrier.

## Observations

- `NtVdmControl(VdmQueryDir, query)` rejects missing session/binding, then
  invokes only the explicitly bound callback and preserves its `NTSTATUS`.
- Every other service class returns `STATUS_NOT_IMPLEMENTED`; the companion
  disposition ledger gives it one future package owner rather than treating
  that result as success.
- `mvdm_monitor_bind_current_thread` places the thread-local carrier in the
  adapter-private `NtCurrentTeb()->Vdm` field. It neither exposes a native
  pointer across an ABI nor creates a kernel VDM object.
- The formal graph was executed outside the sandbox with MSVC `/MT`:
  `New-T308MonitorControlNinja.ps1 -Architecture x86` followed by
  `ninja -C build/M0-T308/S1/x86 test`, and the identical x64 pair. Both
  `vdm_control_fixture` and `vdm_tib_fixture` reported `PASS` for both
  architectures.

## Interpretation and limit

This closes the directly composable monitor-control form and DPMI-visible
thread carrier. It does not close CPU execution, interrupt delivery, LDT,
printer, audio, process broker or debugger behavior. Those forms remain
owned by the packages recorded in the service disposition ledger.
