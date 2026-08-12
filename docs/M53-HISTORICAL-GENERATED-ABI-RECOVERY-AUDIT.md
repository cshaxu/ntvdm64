# M53 Historical Generated ABI Recovery Audit

Status: completed read-only local-source audit, 2026-08-08. This independently
rechecks the immediate `InitNtCpuInfo` block without inventing a layout or
modifying an execution path.

## Corrected Question

The prior search premise was that the selected NT4 x86 profile required a
CCPU-compatible `GDP_PTR`/`ConstraintR*` generated ABI. The original
`obj.vdm/CDEFINE.INC` disproves that premise: the `$(386)` branch selects
`MONITOR`, `C_VID`, and `X86GFX`; `CPU_40_STYLE` and optional CCPU selection
are in the non-x86 branch. The observed `InitNtCpuInfo` stop belongs to a
forced cross-profile fixture.

## Local Inventory Result

| Input tree | Result |
| --- | --- |
| Imported OpenNT NT4 (`src/opennt`) | Original `nt_cpu.c` and CCPU `gdpvar.h` are present. Full generated candidates occur only under MIPS/PPC host profiles; `base/cvidc/cpu_c.h` has a different generated C/video profile. This matches the original build selection: no x86 `GDP_PTR` declaration is expected. |
| Local OpenNT NT4 checkout | Same source/layout inventory as imported OpenNT; no additional x86 generated monitor input. |
| OpenNT 4.5 | Same `nt_cpu.c` call shape plus only MIPS/PPC generated host variants and the separate CVIDC declarations. |
| Windows 2000 `private/mvdm` | No `InitNtCpuInfo`, `GDP_PTR`, `GLOBAL_CurrentUniverse`, or `ConstraintRAL_LS8` source match. It is not an alternate source for this SoftPC generated profile. |
| XP SP1 `softpc.new` | Contains a later `nt_cpu.c` and `config.c` with the same non-x86 dependency. Its `ccpu386/gdpvar.h` defines `GLOBAL_CurrentUniverse` through CCPU `Gdp`; `base/cvidc/cpu_c.h` supplies `ConstraintRAL_LS8`. These forms belong to distinct generated profiles and are not composable. |
| Imported NTVDMx64 reference | No source match for the requested generated monitor ABI. Its modern execution adaptations cannot supply the missing historical layout. |

## Evidence Rule And Result

The apparently useful XP/CVIDC names are rejected. Joining CCPU `Gdp` with
CVIDC constraints or borrowing MIPS/PPC GDP offsets would define a new hybrid
monitor ABI. It would not recover the actual NT4 x86 V86-monitor profile.

No further search for a hypothetical NT4 x86 `GDP_PTR` input is warranted.
`InitNtCpuInfo` remains the required fail-closed stop only for cross-profile
instrumentation.

## Consequence

Historical x86 source remains a behavior/source oracle, while the actual x86
execution path depends on V86. Progress toward a runnable x64 software sample
therefore needs a separately governed execution backend with the neutral
capabilities identified in M48/M49; it cannot be unlocked by a generated-header
recovery exercise.
