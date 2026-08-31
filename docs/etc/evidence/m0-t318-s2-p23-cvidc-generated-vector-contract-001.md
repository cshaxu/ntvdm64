# M0 T318 S2 P23 — C-VID generated vector contract

## Scope

The fixed non-debug, console-owning startup container remains unchanged. This
record recovers only the missing historical CCPU-to-C-VID generated vector
assembly from original C-VID metadata; it adds no BOP, guest lifecycle, or
machine behavior.

## Source evidence

- Original `softpc.new/base/cvidc/c2cpusad.h` names 154 public and 55 private
  C-VID CPU-vector slots.
- Original `cvidc/accessfn.c` forwards each non-CCPU call through those slots.
- The selected CCPU40 manifests contain the provider bodies but omit the
  product-generated binding unit.  The CCPU and C-VID `cpu4gen.h` layouts are
  not interchangeable after their initial shared members.

## Recovery and disposition

`GenerateCvidcCpuBinding.mjs` expands the original slot list, and the overlay
binds it at C-VID's original `setup_vga_globals` assembly point.  `localfm`
uses the C-VID carrier layout so the `accessfn.c` view and the global carrier
agree.  The following source-absent or selected-profile-absent slots remain
explicit null/unavailable: `ClearHwInt`, `InitIOS`, `DefineInb`, `DefineInw`,
`DefineInd`, `DefineOutb`, `DefineOutw`, `DefineOutd`, `SetSTATUS`, `SetAC`,
`GetAC`, `GetET`, `GetNE`, `GetWP`, `GetSadInfoTable`, `GetCpuState`,
`SetCpuState`, `InitNanoCpu`, `PrepareBlocksToCompile`, `SetRegConstraint`,
`TrashIntelRegisters`, `GrowRecPool`, `BpiCompileBPI`,
`FmDeleteAllStructures`, `ConstraintsFromUnivEpcPtr`, and
`ConstraintsFromUnivHandle`. They are an owner-contract recovery cohort, not
fallback behavior.

## Verification

- Generated x86 CPU40 graph and linked `original-softpc-process.exe`: pass.
- Generated x64 CPU40 graph and linked `original-softpc-process.exe`: pass.
- The unchanged formal startup container still exits `0xC0000005`; this does
  not attribute the fault to any one vector slot and does not claim NTIO,
  NTDOS, EXEC, or parent-return reachability.
