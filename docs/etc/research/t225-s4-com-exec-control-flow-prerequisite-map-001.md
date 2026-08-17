# T225 S4 P1 COM EXEC Control-Flow and Prerequisite Map

## Original Owner

OpenNT `msproc.asm:$Exec` owns COM loading. After `get_binary_type` returns COM,
the guest COM branch allocates the largest available DOS arena block, reserves
the PSP and stack, seeks the selected handle to zero, calls `ExecRead`, and
sets `CS:IP=Exec_DMA-10h:100h`, `SS`, `SP`, and a zero return word before
continuing to `Exec_Set_PDB`. `get_binary_type` itself owns guest open, ioctl,
header read/classification, and source-shaped close/failure paths.

The adapter may therefore supply only the existing DEM file service boundary.
It may not create a PSP, allocate a DOS arena, copy COM bytes directly to an
entry address, set guest registers, or select the return destination.

## Existing Evidence

- `NTIO.SYS`, `NTDOS.SYS`, and `COMMAND.COM` have source-built artifact
  evidence in the DOS/WOW16 closure ledger.
- S3 closes Direct/Readonly DEM file services needed by guest `$Exec`.
- Current source-built fixtures exercise package-session BOP routes only;
  they do not execute the NTDOS `$Exec` code path or prove its arena/PSP
  preconditions.
- The current minimal Bochs runtime remains a separate machine/firmware
  dependency gate. A BOP fixture is not a native COM execution claim.

## Prerequisite Disposition

| Prerequisite | Owner | Current disposition |
| --- | --- | --- |
| COM classification/open/read/close | guest NTDOS plus S3 DEM services | source/ABI closed; native reach unproven |
| DOS arena and CurrentPDB | guest NTDOS arena/process code | source present; runtime reach unproven |
| PSP creation/JFN inheritance | guest NTDOS | source present; runtime reach unproven |
| COM destination, CS:IP, SS:SP | guest NTDOS `$Exec` | source present; runtime reach unproven |
| machine RAM, real-mode execution | bx-core/bx-mantle | separate native-machine gate; not admitted here |
| DEM 50:36/3C notifications | S2 lifecycle package | source-built closure complete |
| parent termination and COMMAND return | guest NTDOS/COMMAND | later T225 package |

## Fixture Plan

1. Build a source-derived guest fixture that invokes the original guest `$Exec`
   COM path in a bounded DOS image, with a declared COM target and no host-child
   launch.
2. Observe only guest-owned outcome records: file token sequence, arena/PSP
   allocation, COM bytes at the guest-selected DMA address, and candidate entry
   registers. The observer must not mutate those values.
3. Admit a native Bochs trace only after the fixture proves all preconditions
   and the machine package declares its required CPU/firmware/RAM state.

## Recovery Ledger

| Rung | Decision |
| --- | --- |
| Original source | Use NTDOS `$Exec` COM branch unchanged. |
| Smallest seam | Retain S3's typed file I/O only. |
| External intrusion | Rejected. |
| New behavior | Rejected: no adapter loader or synthetic DOS process. |

## P2 Native Fixture Admission Result

The retained source-built artifacts are present: NTIO.SYS, NTDOS.SYS, and
COMMAND.COM have the DOS/WOW16 closure evidence, and S3 supplies their DEM
file I/O. However, current native startup observation scripts explicitly
place only NTIO.SYS. NTDOS.SYS is recorded as an identity-only input and
COMMAND.COM is only a profile/namespace input. Existing tests that use three
byte files or synthetic command images validate composition contracts, not
real guest execution.

Therefore a real `$Exec` COM fixture is not presently admissible. The first
missing owner package is machine/startup continuity: original NTIO must reach
original NTDOS, and NTDOS must reach its normal COMMAND open/EXEC sequence on
a minimal machine with the required firmware, RAM, CPU, and interrupt state.
This is not a request to put those semantics in bx-vdm. It is a prerequisite
for a later guest-owned COM fixture.

S4 remains an admission/audit package. Its output is the concrete handoff to
the machine/startup owner; no COM execution claim, native trace, or adapter
loader implementation is made.
