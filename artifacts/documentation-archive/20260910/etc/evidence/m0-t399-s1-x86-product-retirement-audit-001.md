# M0 T399 S1 — x86 product retirement audit

## Question

Does the current product path enforce the owner's x86-only `ntvdm32.exe`
contract, without retaining native-x64 build, stage, runtime, or acceptance as
a closure requirement?

## Procedure and results

1. Invoking `New-T310OriginalSoftpcNinja.ps1 -Architecture x64` is rejected
   at its mandatory `ValidateSet('x86')` entry gate. It creates no x64 product
   graph.
2. The same generator successfully created
   `build/M0-T399/S1/formal-x86/build.ninja` from the x86 argument and its
   current source manifest reports 131 CCPU units. The generator has no
   `$Architecture -eq 'x64'` branch.
3. The current formal x86 product PE,
   `build/M0-T398/S2/error-interaction-x86/original-softpc-process.exe`, has
   COFF machine `0x014C` (`IMAGE_FILE_MACHINE_I386`).
4. `StageProductExecutable.mjs --architecture x64` now rejects before any
   publication with `only the x86 ntvdm32.exe product is supported` (exit 2).
   An x86 stage succeeds and writes only `ntvdm32.exe` to both destinations;
   each output is 3,583,488 bytes with SHA-256
   `f1c3f81d793198d32ed685429e4509547a581fc0b3f87240b97296022f0a66a0`.
5. `Stage-OriginalSoftpcRuntime.mjs --product-name ntvdm64.exe` now rejects
   before staging. Its default and sole accepted product name is
   `ntvdm32.exe`. The Console observer likewise rejects a runtime manifest
   whose product destination is not `ntvdm32.exe`.
6. A repository sweep finds 46 older, x64-capable focused-helper generators.
   They are named historical T279–T394 fixture/audit helpers, not called by
   the formal product generator; their retained x64 parameters are historical
   evidence only. They are not current product build, stage, runtime or
   acceptance inputs and are deliberately neither repaired nor mass-edited.

## Interpretation

The formal source graph, executable identity, publication tool, fixed-runtime
stager and observer now agree on exactly one product: Win32/x86
`ntvdm32.exe`. The retained 46 historical helper scripts do not reopen x64 as
a product target. This audit selects no x64 source repair.

The sole permitted future exception remains a demonstrated
architecture-neutral mapping-manager correctness defect. Such a defect must
be admitted and evidenced as a shared contract issue; an x64 compiler/linker
diagnostic alone is not one.
