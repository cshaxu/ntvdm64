# M0 T198 S62: `BOP 66` EMS-Unavailable Contract Audit

## Observed input

The clean source-built witness at
`artifacts/build/t198-s62-emm-probe-observation-r1` reaches `C4 C4 66` at
`8dc8:79c5` with:

| Field | Value |
| --- | --- |
| AX | `0809` |
| BX | `0000` |
| DS:DX | `0809:0085` |
| FLAGS | `00000046` |

The following byte `83` is ordinary guest code, not a BOP service.

## OpenNT source map

`dos/v86/doskrnl/bios/spcemm.asm` defines `InitSpcEmm`: it clears BX, passes
`EmmRet` in `DS:DX`, then issues `bop 66h`.  Its immediate continuation treats
`BX=0` as no expanded memory and takes `fail`, returning `AX=FFFF` without
installing INT 67h.

## Existing finite provider

`bx_ntvdm_emm_unavailable_service_v1_dispatch` exactly validates a real-mode
`C4 C4 66` event, resumes after three bytes, and overwrites only `BX=0`.
It is explicitly an unavailable completion, not EMS/LIM emulation: it adds no
page mapping, memory manager, device, firmware, host integration, or Bochs
behavior.  S63 may compose it at the top-level boundary.

