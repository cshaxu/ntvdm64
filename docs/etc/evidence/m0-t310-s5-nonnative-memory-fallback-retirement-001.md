# M0 T310 S5 — non-native DPMI memory fallback retirement

## Decision

The local `adapter-mvdm-host-out/softpc/mvdm_dpmi_memory.c` provider and the
parallel `mvdm_xms_memory.c` provider are retired.  They independently
implemented the
historical `VdmAllocateVirtualMemory`, `VdmCommitVirtualMemory`,
`VdmDeCommitVirtualMemory`, `VdmFreeVirtualMemory`,
`VdmReallocateVirtualMemory`, `VdmAddVirtualMemory`, and
`VdmRemoveVirtualMemory` spellings by maintaining a private allocation list
over Bochs memory.

That was a non-native fallback, not an adapter binding to the selected
original SoftPC implementation.  It is therefore incompatible with the
active original-SoftPC S5 profile and with the single session mapping-manager
rule.  The original `xms.486/xmsblock.c` mirror is restored byte-for-byte to
the selected OpenNT body; no XMS move overlay remains.

## Removed live inputs

- `src/adapter-mvdm-host-out/softpc/mvdm_dpmi_memory.c`
- `src/adapter-mvdm-host-out/softpc/include/mvdm_xms_memory.h`
- `src/adapter-mvdm-host-out/softpc/mvdm_xms_memory.c`
- `src/mvdm-host-overlay/xms.486/xms_block_move.[ch]`
- `tests/adapter-bochs/t289_s4_dpmi_composition_fixture.cc`
- `tests/adapter-bochs/t287_s4_xms_block_fixture.cc`
- `tools/build/New-T289S4DpmiCompositionNinja.ps1`
- `tools/build/New-T287S4XmsBlockNinja.ps1`
- `tools/build/New-T287S6XmsDispatchNinja.ps1`
- the deleted graph's invocation from
  `tools/build/Invoke-T289S6DpmiFamilyMatrix.ps1`
- its active adapter divergence-register row.

Historic T287/T289 evidence remains history: it records what the retired
experiments proved, not a current source-link or runtime claim.  The former
T287 S6 fixture is retained only as an unreferenced diagnostic source because
it also covers command-register, SAS and INT15 behaviors; it has no live
build graph and cannot represent current XMS acceptance.

## Replacement direction

`mvdm-host/softpc.new/host/src/nt_mem.c` remains the only selected MVDM
algorithm owner for the `Vdm*VirtualMemory` family.  Its x64-safe recovery is
not a cast-only edit: the session's single guest-memory mapping-manager
instance must bind original Intel-linear allocation and
`PhysicalPageREC.translation` operations to a checked SoftPC backing range.
`VdmAddVirtualMemory` and `VdmRemoveVirtualMemory` need the same physical-page
transaction before their original WOW/DPMI callers can be enabled.

Until that source-shaped binding is implemented and tested, all DPMI and XMS
memory callers remain deferred.  There is no fallback to Bochs, no second
allocation list and no separate handle/address mapper.

## Verification

- Repository search finds no live production or build reference to either
  removed provider, its removed overlay, or the retired graphs.
- The active T310/S5 x86 and x64 original CCPU formal graphs do not select any
  deleted input and remain the applicable execution proof.
