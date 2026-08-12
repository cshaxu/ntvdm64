# M57 Historical Profile Boundary Correction

Status: completed source-definition correction, 2026-08-08.

## Fact

The original NT4 build definition
`src/opennt/base/mvdm/softpc.new/obj.vdm/CDEFINE.INC` selects:

```text
386/x86:       MONITOR, C_VID, X86GFX
non-x86:       CPU_40_STYLE, optionally CCPU and C_VID
```

The `GDP_PTR`/`ConstraintR*` state consumed by `InitNtCpuInfo` belongs to the
second profile. A fixture that combines it with CCPU and x86 host compilation
is a deliberately useful source/ABI probe, but not a partially restored NT4
x86 execution route.

## Consequence

NT4 x86 historically relies on the V86 monitor. The research target is an x64
software-emulated process and explicitly excludes V86. Therefore it cannot
be unlocked by recovering or reconstructing a hypothetical NT4 x86 CCPU GDP
header: no such input is expected by the original x86 build selection.

OpenNT CCPU, BIOS, BOP, DEM, NTDOS, and guest artifacts remain valuable as
source/behavior oracles. A future executable research runner must instead use
a separately governed software execution backend and satisfy the neutral
transition/memory contracts in M55 before it may use those oracle-derived
adapter fixtures.

## Prohibitions

- Do not combine CCPU `Gdp` declarations with CVIDC constraints.
- Do not borrow MIPS/PPC GDP layouts.
- Do not call a CCPU lifecycle harness a recovered x86 monitor.
- Do not make the frozen BOP/DEM/IVT scaffolding into a substitute runtime.
