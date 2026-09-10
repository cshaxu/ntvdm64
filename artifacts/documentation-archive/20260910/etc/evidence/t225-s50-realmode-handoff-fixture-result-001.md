# M0 T225 S50 P1 — Source-shaped real-mode handoff fixture result

## Scope

This fixture proves generic bx-core/bx-mantle mechanics corresponding to the
post-`50:36` NTDOS `$Exec` transfer. It is not a native guest run and contains
no BOP selector, provider, adapter route, guest artifact or device behavior.

## Input and expected state

The entry byte sequence establishes the original register roles and executes:
`CLI; MOV SS,AX; MOV SP,DI; STI; PUSH DS; PUSH SI; MOV ES,DX; MOV DS,DX;
MOV AX,BX; RETF`.

The synthetic child entry records its resulting AX, DS, ES, SS and SP in
ordinary fixture RAM, then halts. The fixture separately captures the
retained far-return words at `SS:FFFA`.

Expected results are `CS:IP=2222:0118`, retained frame bytes
`00 01 22 22`, and post-transfer values `AX=BEEF`, `DS=ES=4000`, `SS=3000`,
`SP=FFFE`. A separate invalid entry range is rejected before machine setup.

## Verification

Fresh formal graph root: `build/M0-T225-S50/realmode-handoff-r4`.

- Generator: `tools/build/New-T225S7FullNinjaGraph.ps1`
- Target: `bin/t225-s50-realmode-handoff-fixture.exe`
- Formal graph: 312 declared outputs compiled/linked under MSVC x64 `/MT`.
- Fixture exit: `0`.
- `module-manifest.json` SHA-256:
  `77796989e8e64f6378827739c70f41c494db3c6564afce4dfaed56aed93efd15`.

The initial focused run exposed only fixture address arithmetic: the far
return `2222:0100` maps to physical `0x22320`, not the segment base `0x22220`.
After correcting that fixture input, the focused and fresh formal runs pass.
No bx-core, bx-mantle, bx-vdm or OpenNT production behavior changed.

## Disposition

S50 reduces S49's generic-mechanics limitation: the whole source-shaped
instruction sequence is now executed by the adopted minimal machine.
It does not prove an immutable NTIO/NTDOS/COMMAND guest has reached a child,
and it does not select a BOP, provider, device or Bochs repair.
