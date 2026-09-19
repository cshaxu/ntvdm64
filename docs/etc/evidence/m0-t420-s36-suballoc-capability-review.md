# T420 S36 suballocator capability review

## Inputs and current disposition

S36 follows S35 delivery 23d12c9f5 under standing sequential admission.
It remains open. Product profile is MSVC Win32/x86 CCPU40; original guest
media remain immutable. Build/test products belong below build/M0-T420/S36,
runtime probes below the approved tests directory and observations below logs.

The initial read-only comparison finds both suballoc/suballoc.c and its
private suballcp.h byte-identical to pinned OpenNT. Original inc/suballoc.h
retains the already documented _M_IX86 commitment-granularity selection.
No allocator algorithm correction is currently justified.

## Original owners and required evidence

| Surface | Original selected owner | Verification still required |
| --- | --- | --- |
| Initialize/query/allocate/free | suballoc.c, XMS xms.c/xmsblock.c | Real caller capacity, allocation granularity, exhaustion, release/coalescing and reuse; preserve the S35 passing baseline. |
| Same-rounded-size/shrink/tail growth | SAReallocate | Distinguish each original branch with sizes/addresses and retained data, rather than one generic successful realloc. |
| Growth into preceding free extent | SAReallocate/AllocateChunkAt/MoveMemRoutine | Controlled fragmentation, changed base, overlapping move data and complete release. |
| Relocation elsewhere | SAReallocate/SAAllocate/MoveMemRoutine/SAFree | Block adjacent growth, prove actual address change and byte preservation, then restore capacity. |
| DPMI shared pool | dpmi32/dpmimemr.c CPU40 branches | Real DOSX/DPMI guest allocation/reallocation/free; source inclusion is not execution proof. |
| Commit/decommit/move failure | Original callbacks and existing worker memory leases | Original-source failure fixture plus admitted real guest negative cases and teardown; no invented allocator or fake success. |

Original SAReallocate tries tail growth, then a preceding extent, then a new
allocation elsewhere; its documented failure contract preserves the original
allocation. Original DpmiAllocateVirtualMemory's selected CPU40 branch calls
SAAllocate(ExtMemSA) directly. This source graph does not by itself prove that
the current guest reaches that branch.

The retained S16 fixture only increments commit/decommit/move counters. In
particular fixture_move does not copy bytes, so a passing counter check cannot
establish data preservation or overlapping-copy correctness. S35 verifies real
XMS growth/shrink and failure preservation but does not force an address change.
S36 must close these explicit gaps, then run formal x86 and the complete
transcript-gated product regression. No runtime acceptance or S36 closure is
claimed by this initial source audit.
