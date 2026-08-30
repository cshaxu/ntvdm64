# M0 T310 S36 — MVDM SIM32 owner-package closure

## Scope and source identity

The selected original CCPU40 host manifest
`src/mvdm-host/softpc.new/host/src/sources` directly selects
`sim32.c`.  The reviewed source surface is:

- `Sim32pGetVDMPointer` and its VDD-compatible export
  `ExpSim32GetVDMPointer`;
- `Sim32FlushVDMPointer`, `Sim32FreeVDMPointer`,
  `Sim32GetVDMMemory` and `Sim32SetVDMMemory`;
- `sim32_effective_addr`, `sas_mem_map`, `sas_clear_map`, and the selected
  CPU40 `host_sas_init` / `host_sas_term` lifecycle;
- direct callers in original host roots (`nt_mem`, `nt_emm`, `nt_bop`), XMS
  and DEM; and
- `demsrch.c:FileFindReset`'s optional `NtVdmControl(VdmQueryDir)` fast path.

The original SIM32 body remains directly selected.  Its pointer result is a
private same-owner SoftPC/SAS implementation value in the selected x86
CCPU40 process; S36 does not publish it through a component ABI, a session
mapping identity, a BOP result, or asynchronous state.  Existing callers
that cross a component boundary retain their separately registered bounded
lease dispositions.

## Recovered boundaries

`sas_overwrite_memory` was the sole selected SIM32/SAS forced-link miss.
Original `ccpu386/ntstubs.c` contains its exact two-line facade to original
`c_sas_overwrite_memory`, but its complete translation unit also declares
alternate CPU/FPU-facing state that conflicts with the selected CCPU40
owners.  `mvdm-host-overlay/softpc.new/base/ccpu386/sas_overwrite_memory.c`
therefore retains only that exact source subset under `MVDM-HOST-DIV-138`.
It has fixed-width physical address/length arguments, creates no mapping and
does not alter the original invalidation order.

The reached DEM directory-reset acceleration now formally links the existing
same-shaped monitor binding `NtVdmControl(VdmQueryDir)`.  It is deliberately
separate from the BaseSrv command route.  With no bound monitor capability it
returns `STATUS_NOT_IMPLEMENTED`; original `FileFindReset` then executes its
already-present slow `NtQueryDirectoryFile` scan.  This preserves the source
failure/fallback direction without importing kernel VDM or MONITOR.

## Verification

1. Generated a fresh formal graph:

   ```text
   tools/build/New-T310OriginalSoftpcNinja.ps1 -Architecture x86 \
     -BuildRoot build/M0-T310/S36/formal-sim32-x86-r2
   ```

   The selected CCPU40 graph completed **384/384** Ninja edges.  Its forced
   link has no `Sim32*`, `sas_overwrite_memory` or `NtVdmControl` unresolved
   form.  Remaining forced-link names are separately owned: LIM writes (EMS),
   debugger module/prompt calls, DPMI/DBG dispatch and initialization,
   `ShowStartGlass`, NT virtual-memory/hard-error forms.

2. Ran the x86 monitor-control fixture through the repaired original-declaration
   include boundary.  It passed:

   ```text
   PASS: VdmQueryDir adapter contract
   ```

   It proves no-session refusal, unbound-session refusal, rejection of a
   different service class, bound typed callback dispatch, explicit unbind,
   and session teardown.

## Disposition

S36 is source-package closed for the selected Win32/x86 CCPU40 profile.
`NtVdmControl` services other than `VdmQueryDir`, kernel VDM/MONITOR,
protected DPMI pointer state, LIM behavior and Debugger calls remain their
named later owner packages.  This is not an integrated guest-execution claim;
that remains S49.
