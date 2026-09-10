# InitNtCpuInfo Recovery Disposition

Status: corrected profile-boundary disposition, 2026-08-08. This record
governs only a forced cross-profile CCPU probe. It does not authorize a
replacement implementation or a runnable DOS engine.

## What The Original Function Is

The sole recovered implementation is OpenNT
`base/mvdm/softpc.new/host/src/nt_cpu.c:265`. `InitNtCpuInfo` initializes the
host-side `nt_cpu_info` register-view structure. It does not load a guest
image, provide a BIOS device, dispatch a BOP/SVC, or perform DOS filesystem
work. Its inputs are generated monitor state, including:

- the GDP base (`GDP_PTR`) and two hard-coded production-layout offsets;
- generated global state such as `GLOBAL_CurrentUniverse`, segment selectors,
  cleaned EIP, EFLAGS, CR0, saved GPRs, and nano GPRs; and
- generated register-constraint bit positions (`ConstraintR*`).

It therefore binds a non-x86 `CPU_40_STYLE` debugger/monitor representation to
a specific generated profile. It is not a narrow Win32, console, file, timer,
or process dependency that a modern host shim may replace.

## Cross-Tree Recheck

The fixed OpenNT tree, local OpenNT 4.5 tree, imported NTVMDx64 tree, local
Windows 2000 tree, and local XP tree were rechecked for `InitNtCpuInfo`,
`GDP_PTR`, `GLOBAL_CurrentUniverse`, and `ConstraintRAL_LS8`. The original
`obj.vdm/CDEFINE.INC` is decisive context: its x86 branch selects `MONITOR`,
`C_VID`, and `X86GFX`; its non-x86 branch selects `CPU_40_STYLE` and optional
CCPU.

| Input | Result |
| --- | --- |
| OpenNT NT4 source | `nt_cpu.c` implementation and CCPU-local generated declarations; no x86 `GDP_PTR` host header is expected by the original build branch |
| OpenNT 4.5 source | same profile split |
| NTVMDx64 imported input | no alternate historical profile definition |
| Windows 2000 / XP local inputs | later callers and a same-shape `nt_cpu.c`; no reason to expect an x86 `CPU_40_STYLE` profile |
| MIPS/PPC host variants | generated GDP/constraint variants for non-x86 profiles only |

This explains the compiler evidence: forcing `nt_cpu.c` into an x86 CCPU
probe stops on `GDP_PTR` and `ConstraintR*` because it combines incompatible
build profiles. It is not an ordinary missing declaration that can be repaired
with a typedef.

## Admissibility Decision

The cross-profile probe may use `InitNtCpuInfo` only with a matching non-x86
profile input or a complete legal trace. Neither outcome restores the NT4 x86
historical execution path, which used a V86 monitor.

Until then, all of the following are prohibited:

- defining `GDP_PTR` as an alias for CCPU-local `Gdp`;
- importing offsets or constraint numbers from MIPS, PPC, CVIDC, or a later
  build profile;
- supplying a no-op `InitNtCpuInfo`; and
- reordering original `config()` to skip the call while claiming the historical
  startup chain has been restored.

The existing config/SAS fixture's non-returning stop at this call remains
valid bounded instrumentation only.

## Consequence For The Roadmap

Plan A source recovery has reached an architectural evidence boundary here.
Further work can improve source/artifact reconstruction, but it cannot turn
the forced CCPU fixture into the original x86 `config()` path.

The x64 research route must be explicitly governed software emulation. It may
use historical source to derive neutral requirements, but it must not claim to
recover the x86 V86 monitor or the cross-profile CCPU/`InitNtCpuInfo` fixture.
