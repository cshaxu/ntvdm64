# T200 S16 — direct-composition startup-plan closure

## Question

Can the direct, copied-input composition supply the existing pointer-free NTIO
startup plan without returning to an environment-installed startup session, the
legacy adapter runtime, or the finite-run fixture?

## Source and ABI basis

`bx_ntvdm_startup_session_v1_prepare_ntio_plan` establishes retained NTIO v0
geometry: payload at physical `0x700`, real-mode `CS=0x70`, a four-byte
preserve range at `0x714`, and one-megabyte preflight. It remains reference
evidence only, because it owns a different startup session.

S16 extends direct composition to load the profile-selected `NTIO.SYS` and
adds `bx_ntvdm_composition_runtime_v1_prepare_startup_plan`. The function
returns the existing pointer-free `bx_ntvdm_startup_plan_v1` and a borrowed
payload view. The payload belongs to the installed composition and becomes
invalid on `bx_ntvdm_composition_runtime_v1_reset`.

The source-built profile does not declare a machine-startup recipe; this proves
only retained NTIO geometry and ownership. It does not promote fixture
preentry bytes to a product default.

## Boundary

| Concern | Owner |
| --- | --- |
| NTIO image and payload lifetime | bx-vdm direct composition |
| Plan geometry and validation | existing bx-vdm startup-plan ABI |
| Guest RAM publication and CS:IP mutation | later bx-mantle mechanical step |
| Machine initialization, CPU loop, BOP routing | outside S16 |

The current composition manifest now links the existing startup-plan ABI. This
is link membership only, not machine activation or BOP-routing change.

## Verification

`Invoke-T200S16DirectStartupPlanProbe.ps1` passed at
`artifacts/build/t200-s16-direct-startup-plan-r6`. It reruns the retained S14
source-built direct-composition closure, rebuilds the current composition as
x64 `/MT`, then proves the expected NTIO plan/payload and rejection after
reset. No machine was initialized, CPU loop entered, or guest instruction run.

## Follow-up

Product preentry recipe and mechanical mantle publication remain source-map
work. Fixture-only IVT/BDA setup stays excluded until independently proven.
