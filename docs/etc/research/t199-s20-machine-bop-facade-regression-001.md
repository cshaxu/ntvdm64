# M0 T199 S20 Machine-BOP Facade Regression

## Question

Can all currently admitted machine-composition BOP forms pass common ingress
and a single `bx-vdm` facade, while leaving Bochs and mantle selector-blind?

## Implementation

- Ingress now classifies the three-byte BIOS forms `12` and `15` as copied
  top-level machine facts rather than letting composition recognize raw bytes
  before ingress.
- `bx_ntvdm_machine_bop_facade_v1` owns `12`, `15`, `5F`, `5C`, `5D`, and
  `FD`. It delegates the bounded existing memory and SPCKBD paths; it returns
  typed stop for the unresolved `5C`, `5D`, and `FD` forms.
- The prior top-level facade no longer owns the `5F` machine handoff. This is
  a successor migration: S18's historical evidence remains true for its
  original routing state, while current ownership is the machine facade.

## Verification

The focused probe was built and run with MSVC x64 `/MT`:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T199S20MachineFacadeX64Probe.ps1 `
  -RepositoryRoot (Get-Location).Path `
  -BuildRoot (Join-Path (Get-Location).Path 'artifacts\build\t199-s20-machine-facade-r2')
```

It covers all six selectors, proves that `12`, `15`, and `5F` receive no
facade-fabricated result, and that `5C`, `5D`, and `FD` produce typed stop.
The bound boot-namespace composition probe was rebuilt at
`artifacts/build/t199-s20-machine-composition-r2`; it exits zero and covers
the existing `12`, `15:88`, `5F` action, and all three deferred stops.

## Limits

`12` and `15:88` remain fixed profile values, not live Bochs-memory queries.
`5F` remains only its source-derived checked RAM action. `5C`, `5D`, and
`FD` do not perform a transition, IRET hook, keyboard, video, PIC, or firmware
operation. No trace ran and no bx-core/bx-mantle source changed.

## Follow-Up

The next package starts C1, the selector-blind A20-state capability required
before any XMS/DPMI provider can return an A20 result.
