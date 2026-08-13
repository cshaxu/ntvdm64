# T198 S14 MSVC x64 Finite HLT Witness 001

## Question

Does the closed MSVC x64 CPU5/Pentium-MMX `bx-core`/`bx-mantle` closure execute
the existing selector-blind neutral `HLT` fixture through the finite native
runner and controlled stop?

## Reproducible Procedure

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/Invoke-T198S3FiniteNativeRunProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t198-s14-msvc-x64-finite-hlt-r1 `
  -HostArchitecture x64
```

The recipe first creates the S13 complete CPU5 source closure with MSVC x64
`/MT`, then adds only `bx_ntvdm_finite_run.cc`, the generic default-decline
bridge, and a generated fixture containing one guest byte `f4` at physical
`0x1000`, real-mode `CS=0x0100:EIP=0`, and a 64-tick stop budget.

## Result

The fresh r1 run linked and exited zero. Its recorded fixture is `f4 (HLT)`;
the run log shows CPU reset, APIC allocation, Pentium-MMX CPUID and ordinary
RAM allocation before the original core diagnostic `WARNING: HLT instruction
with IF=0!`. The finite runner then reaches its controlled stop and cleanup,
as demonstrated by process exit code zero.

`headers.txt` identifies the finite-run executable as COFF `8664 (x64)` and
PE32+ (`20B`). `link.rsp` contains neither a Bochs product input nor
`bx-vdm`, OpenNT, adapter, MinGW, UCRT, GUI, or device archive input. The
machine and guest CPU identities remain distinct: the executable is x64 while
the emulated CPU profile remains CPU5/Pentium-MMX.

## Bound

The IF=0 HLT diagnostic is expected for this intentionally minimal bare
real-mode fixture; it is retained as observation, not suppressed. This proves
only native instruction-loop entry and finite controlled stop. It does not
prove `#UD`, BOP ingress, provider execution, DOS/WOW16 behavior, CLI
behavior, firmware boot, or device behavior.
