# M0 T355 S20 P1 — original XMS backing-selection build

## Scope

This delivery changes only the formal CPU40/x86 build selection.  It removes
`/DMVDM_XMS_SESSION_BACKEND` from
`tools/build/New-T310OriginalSoftpcNinja.ps1`; no original XMS/SoftPC function
body, lease contract, BOP provider, guest image, firmware, or runtime command
changed.

## Procedure

1. Generated `build/M0-T355/S20/formal-x86` with the formal Ninja generator,
   MSVC x86 and Node 22 GDP generator.
2. Inspected its generated `build.ninja`: the removed macro has no selected
   compile-command occurrence.
3. Re-read `xms.486/xms.c`: with historical `i386` undefined and the removed
   macro absent, its existing non-`i386` branch calls
   `VdmAllocateVirtualMemory(..., FALSE)` and supplies
   `VdmCommitVirtualMemory`/`VdmDeCommitVirtualMemory` to `SAInitialize`.
4. Re-read `softpc.new/host/src/nt_mem.c`: those original functions reserve
   the `intelMem` allocation and commit/decommit the requested original range.
5. Ran the formal parallel Ninja target
   `original-softpc-process.exe` outside the sandbox.  All 426 actions
   completed and `link.exe` produced a non-`/FORCE` executable.

## Link evidence

`original-softpc-process.exe.map` resolves the original lifecycle functions
to `original-softpc-host-roots:nt_mem.obj`:

```
_VdmAllocateVirtualMemory
_VdmDeCommitVirtualMemory
_VdmCommitVirtualMemory
```

The mapping-manager-bound `xmsMoveMemory` carrier remains in
`softpc-bindings:mvdm_xms_memory.obj`.  The same archive still exports
`xmsCommitBlock`, but the selected `XMSInit` source branch no longer passes it
to `SAInitialize`; symbol presence is not callback selection.

The linked output is
`build/M0-T355/S20/formal-x86/original-softpc-process.exe` with SHA-256
`555B4FF35C291BD24E802BE11F676134DC38E19D8AB6355D7335590980C913BC`.

## Result and limitation

P1 proves the original CPU40/x86 XMS backing lifecycle is selected and
formally linkable.  Historic compiler warnings remain visible and were not
suppressed.  No container or guest execution occurred, so this delivery makes
no claim about runtime continuity, the first XMS allocation, or DEM `50:42`.
A separately admitted observation is required for those facts.
