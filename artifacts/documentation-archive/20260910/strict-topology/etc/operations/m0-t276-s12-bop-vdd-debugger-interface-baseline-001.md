# M0 T276 S12 P9: BOP, VDD and debugger interface baseline

## Question

How can the original BOP dispatcher, VDD service bridge and Yoda debugger
retain their source ownership without moving selector/service meaning into a
mechanical adapter or Bochs?

## Findings

- `nt_bop.c` is one source file containing several distinct product owners:
  DEM/XMS/COMMAND/DPMI routing, WOW startup, Redirector loading, VDD, debugger,
  ISV extension loading and IRET notification. It is therefore an imported
  dispatcher source, not an adapter implementation unit.
- The mechanical adapter may copy the BOP instruction/frame and return typed
  completion only. It cannot decide `52`, `53`, `54`, `57`, VDD or WOW meaning.
- `nt_vdd.c` retains the original callback, I/O-port, IRQ and DMA API forms.
  Those services require `adapter-vdd`, bounded session callback/resource
  identity and typed `adapter-bochs` operations. They cannot add VDD names or
  policy to Bochs.
- `nt_yoda.c` decodes guest CPU/SAS state for a debugger product. It may be
  recovered only as part of a complete debugger owner package with checked
  synchronous guest access; neither raw SAS pointers nor CCPU internals are
  admissible today.

## Decision

No dispatcher, VDD or debugger source is enabled. Existing specialist adapters
are sufficient named boundaries; no generic BOP/debug/device compatibility
root is created. Every dynamic DLL path is profile-gated until app/session can
provide source-derived admission, ownership and teardown.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S12BopVddDebuggerInterfaces.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```

Expected result: `T276 S12 BOP/VDD/debugger interface verification passed: 12 rows.`
