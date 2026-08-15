# T198 S13 MSVC x64 Minimal Lifecycle Witness 001

## Question

Can the adopted CPU5/Pentium-MMX `bx-core` and finite `bx-mantle` form one
source-built, MSVC x64 `/MT` machine lifecycle without a Bochs product target,
adapter/provider object, OpenNT object, MinGW object, or foreign CRT?

## Reproducible Procedure

Run the following from the repository root with Visual Studio Build Tools 2022
installed:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T197S6MinimalMachineLinkProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t198-s13-msvc-x64-lifecycle-r3 `
  -WholeCpu5Core -HostArchitecture x64 -RunLifecycle
```

The probe projects the pinned adopted configuration from source hash
`A1FADC25502FB0A4644917B6F3A62987C125CC7335457F9C59E7CE7E62A45AC8`.
The x64 projection changes only host pointer-width configuration
`SIZEOF_INT_P=4` to `8`; its separate CPU profile remains
`BX_CPU_LEVEL=5` and `BX_SUPPORT_X86_64=0`.

## Result

The r3 recipe source-built the complete CPU5 source membership and finite
mantle using MSVC `cl.exe` and `/MT`, linked successfully, and exited zero
after `initialize(1 MiB, 1 MiB) -> CPU reset/APIC/CPUID -> cleanup()`.

The retained generated artifacts provide the exact closure:

- `artifacts/build/t198-s13-msvc-x64-lifecycle-r3/compile.log` contains no
  `warning C`, `C4311`, or `C4312` diagnostic.
- `t197-s6-minimal-machine-link-probe.json` records every source and object,
  `architecture: x64`, `/MT` compiler identity, zero compile/link/run status,
  and the explicit forbidden inputs.
- `link.rsp` and `link.map` retain the link closure. They contain no
  `main.cc`, product configuration, GUI/device archive, `bx-vdm`, OpenNT, or
  MinGW input.
- `headers.txt` reports COFF machine `8664 (x64)` and optional-header magic
  `20B (PE32+)`; the linked executable is therefore an x64 process image,
  not a guest x86 identity claim.

## Interpretation And Bound

The old x86 configuration incorrectly declared a four-byte host pointer
width. Under an x64 compiler that selected `bx_ptr_equiv_t` as 32-bit and
produced truncation warnings; it was a configuration projection defect, not
a CPU, memory, or device-semantic defect. Correcting the host width leaves
the emulated CPU profile at Pentium-MMX and does not modify adopted Bochs
source.

This closes the minimal lifecycle gate only. It proves neither an HLT run nor
generic `#UD`, BOP ingress, provider execution, DOS/WOW16 behavior, CLI
behavior, BIOS boot, or a full Bochs product executable. Those need separate
admission and evidence.
