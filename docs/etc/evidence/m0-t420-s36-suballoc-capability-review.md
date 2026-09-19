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

## Real XMS forced-relocation witness

The independent DOS fixture reuses xms_capability.asm with NASM
`-f bin -DFORCED_RELOCATION`, producing
`build/M0-T420/S36/guest-relocation-r1/X36.COM` (SHA-256
`8c113a855040a05b72747be627d8e66ac94eed264b6b64a3546aad21b3982782`).
It allocates a 64KB block and a second 64KB guard. Public XMS lock/unlock
queries assert that the guard starts exactly 65536 bytes after the first.
It writes a known eight-byte pattern, grows the first block to 128KB and
asserts a different base address. It unlocks, frees the guard, reads back the
pattern and executes the existing shrink/error/free/reuse checks, ending
with equality to the initial total free XMS capacity. Successful reallocation
without an address change is explicitly rejected by the fixture.

Both `s36-relocation-direct-r1` (launcher 52380) and
`s36-relocation-nested-r1` (launcher 5740, two COMMAND /c levels) report exit
zero and the actual Console marker
`S36_XMS_FORCED_RELOCATION_DATA_RELEASE_OK`. Only verified test-owned process
children were terminated afterward; that isolation cleanup is not natural
worker-exit evidence. Original guest media and product code remain unchanged.

This proves real guest-visible relocation and data preservation, not which
of the allocator's preceding-extent versus elsewhere branches ran. Those
separate branch witnesses, fragmentation/failure callbacks and the real DPMI
caller remain open. Building the same source without FORCED_RELOCATION gives
the byte-identical original S35 COM, SHA-256
`f300bbee7aac0080492e3fbd8d5b7fb199a307a017ae9418f2e6b0ab599b308e`;
the existing S35 mode has no binary change.

## Original-source branch and injected-failure review

The retained S16 lifecycle fixture now performs real memmove over a bounded
test backing array rather than counting a move without copying. The unchanged
original translation unit is compiled by Generate-T420S16SuballocNinja.mjs
into `build/M0-T420/S36/suballoc-branches-r1`, MSVC x86 /MT. This is a
host-side allocator contract fixture for the CPU40 numeric-address binding,
not execution through a guest CPU. The normal invocation exits zero with:

- SUBALLOC_ROUND_TAIL_GROW_SHRINK_OK: rounded equality, unchanged-base growth
  and shrink preserve the retained byte pattern without invoking the mover.
- SUBALLOC_PRECEDING_OVERLAP_DATA_OK: a 1KB hole before a 4KB block and a
  following guard force growth to 5KB into the preceding extent; exact mover
  arguments and every original byte are checked, including overlapping copy.
- SUBALLOC_ELSEWHERE_DATA_OK: a following guard and no preceding room force
  relocation to the known later extent; exact mover arguments and bytes match.
- SUBALLOC_FRAGMENT_EXHAUST_FAILGROW_COALESCE_OK: fill 32 pages, free alternate
  pages, reject allocation/growth larger than any hole while preserving data
  and capacity, release the rest, and reuse the complete coalesced pool.
- SUBALLOC_FIRST_COMMIT_FAILURE_REUSE_OK: reject the first commit callback,
  retain all free capacity, then allocate and release successfully.

The separate `--failure-audit` invocation reproduces two original limitations.
Its exit zero means the reproduction assertions matched, NOT that the failure
contract passed or that S36 is accepted:

1. Rejecting the second page commit leaves the first successful callback
   uncompensated: two commit calls, zero decommit calls, entire bitmap free.
   Original CommitChunk explicitly marks the missing rollback with BUGBUG.
   The fixture models callback outcomes; it does not allocate real OS pages.
2. Allocate the complete pool, then fail the first decommit while freeing
   the page at base + 65536. SAFree returns false, but SAQueryFree reports
   4096 newly free bytes. FreeChunk saves/restores a fixed-size structure;
   its trailing variable-length allocation bitmap is not fully restored.

Original and mirrored suballoc.c both hash to SHA-256
`b06496e16d551f2bbac37d896b61857c6569eda8c98fb3d26dbf574e594148e0`.
These are upstream failure-path findings, not new mirror/overlay defects.
No allocator repair or waived acceptance is implied. The current CPU40
xmsCommitBlock/xmsDecommitBlock use bounded guest-memory zeroing leases rather
than real OS commit/decommit; product reachability and consequences of their
failure remain to be established before selecting a repair or limitation.
The mover callback returns VOID, so there is no allocator-level move-failure
status to inject; its worker cancellation boundary requires separate evidence.

Production footprint delta remains zero. Real DPMI allocation/reallocation,
the remaining worker-boundary failure checks, formal build and complete product
regression remain pending. S36 is not closed.

## Real DPMI entry probe: failed, not accepted

Independent tests/observation/dpmi_suballoc.asm follows the 16-bit client
contract in original dpmi/dxfunc.asm (also present in 486/dxfunc.asm): query
INT 2F/1687, allocate the requested host-data paragraphs, and far-call the
returned entry with AX=0. Its subsequent workload uses INT 31/0501, 0503 and
0502, with a data selector and two sentinel DWORDs to check preservation.
It does not replace, rebuild or patch DOSX or any original guest component.

NASM builds it as `build/M0-T420/S36/dpmi-guest-r1/D36.COM`, copied only to
the runtime tests directory. Initial 512-byte-stack execution
`s36-dpmi-direct-r1.txt` (launcher 40108) emits S36_DPMI_ENTERING, then garbled
text, and the launcher exits 0x42b. A 4096-byte-stack control has the same
result in `s36-dpmi-direct-r2.txt` (launcher 50880). The latter COM hashes to
`08e42ee9e363294d3b621d2b2b27b970b8f9f85117a1fa0d8112a06697e400ac`.
Both runs end without remaining package processes; no timeout or forced kill
is being counted as successful guest completion.

Neither S36_DPMI_ENTERED nor S36_DPMI_ALLOCATED nor the final success marker
appears. This locates the unverified boundary between the real-mode entry
call and its first protected-mode DOS text output; it does NOT yet establish
whether mode entry failed or that first DOS translation failed. There is no
evidence that SAAllocate was reached by this workload. A larger probe stack
does not cure it. Root cause remains open, and the source-shaped entry/frame
and return path need read-only observation before any production repair.

The selected lease implementation additionally confirms that commit callbacks
can return failure without OS VirtualAlloc: guest_memory_lease_acquire calls
malloc and the bound reader, and release can fail in the bound writer.
Therefore the original rollback findings cannot be dismissed merely because
the CPU40 backing is pre-reserved. End-to-end reachability and consequences
still require focused failure evidence; no new allocator policy is admitted.

The unchanged deployed product subsequently passes all 17 transcript-gated
Verify-CommandExitStatus routes, recorded in
`s36-dpmi-baseline-r1-summary.json`: direct/nested COMMAND and MEM, repeated
MEM, EDIT return, guest/native exit values and native stream/EOF cases.
This is baseline regression evidence, not acceptance of the failed DPMI probe.
