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

## Read-only DPMI dispatch observation

The diagnostic-only dpmi_dispatch_trace.c wraps the unchanged selected
dpmi32.c translation unit. Build-T420S35EnvironmentTrace.ps1 now optionally
selects this owner with `-Boundary dpmi`, using its exact per-object flags
from the formal Ninja graph (generic cflags do not compile that owner).
The default COMMAND environment wrapper remains selectable unchanged.
Reused object/library hashes are captured in reused-inputs.json. No original
source, CPU state or guest bytes are modified by this observation wrapper.

The successful diagnostic build is
`build/M0-T420/S36/dpmi-dispatch-r2/ntvdm.exe`, SHA-256
`cf1a401110a2ed1cca70d1a77e50c50a89215fd904a16e1a4272896748d9ec66`.
The initial r1 build failed because it used generic rather than owner-specific
flags; it was never deployed. The r2 executable was temporarily selected in
the idle runtime package for one probe, and the formal worker was restored
afterward to verified SHA-256
`4e7d060134e138da57e5fddbfd56543ff89f8daba2814bc08b15eb5f74148a69`.

`s36-dpmi-dispatch-r1.txt` (launcher 2660) reproduces exit 0x42b and the same
Console text. Its events log demonstrates:

- BOP 53:01 returns with PE=1, CS:IP=00CF:1101, SS:SP=00B7:7D8A.
- Descriptor publications complete; DpmiInUse (0C) and InitApp (05) return.
- INT 21 translations of AX=3576 and AX=2576 return.
- The last dispatch is AX=0900, DX=025C, SS:SP=019F:12B2; it has no after
  record. No allocation (07) is observed in this client sequence.

This narrows the failure to the first protected-mode string-output translation,
not inability to enter protected mode. Original int21map.c::DisplayString
first switches the CPU to real mode, then explicitly requests
Sim32GetVDMPointer(..., TRUE) for the still-protected client string selector.
Original sim32_effective_addr honors its pm argument independently of CPU PE.
The current CPU40 adapter instead consults c_getMSW and returns selector*16
when PE is clear, even though the SIM32 caller explicitly requested descriptor
translation. This is a source-proven mismatch in the existing binding, not
an original allocator or CCPU instruction defect. The next repair must separate
explicit protected-address resolution from current-CPU-mode address resolution,
preserve the latter for its other callers, and prove the fix through this
same real probe and the existing product regressions. Runtime causality of
that repair has not yet been tested; no S36 closure is claimed.

## Address contract and missing FLAGS binding repair

The SIM32 call now selects explicit protected-address translation. The
adapter's existing descriptor-base helper becomes that checked numeric helper;
its current-mode API delegates to it only for protected execution, preserving
all other callers. The mirror changes one call, not the original descriptor
algorithm. The first candidate still failed; the blank captured Console was
not evidence of earlier startup failure: dispatch tracing reached the same
AH=09h translation. Native exception observation records C0000005, execute
address zero. Return-stack/map correlation identifies setSTATUS called from
DpmiSimulateIretCF, and the generator explicitly assigned Cpu.SetSTATUS=0.

The missing slot now uses the unchanged eleven flag assignments from original
OpenNT host/src/nt_cpu.c::setSTATUS, through the already selected CCPU setters.
The original whole-unit route is excluded by its A3CPU host-initialization and
IRET-hook backend; the minimal same-shaped extracted binding is selected.
No external-source intrusion or novel algorithm is needed. Ordinary CCPU
setFLAGS is not substituted: its guest CPL/IOPL filtering is different from
this host-supplied state restoration. The source hash and boundary rationale
are registered in the softpc owner README. The executable code is in the
existing binder, with no new mirror file, overlay or CPU instruction change.

Verification deliberately reuses the existing formal x86 graph at
build/M0-T420/S35/formal-native-wait-r2, whose reused input identities are
captured by the S36 diagnostic manifests. Header consumers, SIM32, numeric
binding, generated C-VID binder, worker and Redirector DLL were rebuilt;
the VdmTib storage gate passes. The C-VID fixture now checks every original
SetSTATUS flag both set and clear and passes its existing video-vector tests.
All 17 product routes pass in s36-address-status-product-r1-summary.json.

The same D36.COM now prints ENTERING, ENTERED and ALLOCATED. Diagnostic
s36-dpmi-status-trace-r1 then records successful BOP 07 allocation, 09 resize
and 08 free. The probe checks both sentinel DWORDs before its free; this
demonstrates the actual shared-pool caller and data preservation. However,
the final AH=09h output, now entered with ES=0 after descriptor disposal, does
not return within 15 seconds. This is still a FAILED full-client test, not
an accepted timeout, and its exact return/output boundary remains open.
Only the identified test launcher's worker/broker children were terminated.
The runtime package was then restored to the non-diagnostic formal candidate;
no observation executable remains deployed. The earlier original rollback
limitations and complete DPMI lifecycle verification are still unresolved.

All four XMS routes also pass under s36-address-status-xms-r1. The deployed
non-diagnostic worker hashes to
`a48bc3de09ad48c73e95e71a9d465f2166e9b4f61069cb74ac22a6a84906a498`
and VDMREDIR.dll to
`29e07a07228b07fff1bfb7f8cb96b74975dd29e5a4f33318319724bf16a08ca1`.

## Final-output timeout: wrong local segment-index binding

The independent probe now has two builds from one source: default D36N.COM
retains ES=0 before disposing its descriptor; D36E.COM uses -DRESTORE_ES to
restore the initial PSP selector instead. The latter passes on the preceding
product (s36-dpmi-restore-es-r1, launcher 44632); the default still times out
(s36-dpmi-null-es-control-r1, launcher 12148). This is a diagnostic contrast,
not permission to reject null ES or weaken the guest contract.

The original CCPU c_reg.h defines ES/CS/SS/DS/FS/GS as 0/1/2/3/4/5. The local
DIV-230 mode-switch binding incorrectly called load_pseudo_descr(4/5/6) for
ES/FS/GS. Consequently ES retained its protected null-selector access state,
and index 6 lay outside the six original segment slots. The correction uses
six explicitly named constants matching c_reg.h and changes only those three
call arguments. The original CCPU core and original guest media are untouched.

The identical D36N.COM then completes its entire output and exits zero
(s36-dpmi-null-es-fix-r1, launcher 20652). The new reusable
Verify-T420S36DpmiGuest.ps1 additionally checks all six constants against the
original header and runs both binaries directly and through two COMMAND /c
levels. All four cases pass with the complete guest success marker under
s36-dpmi-segment-fix-r1. The matrix records each input SHA-256 and process state
before test-owned cleanup; cleanup is not natural worker teardown evidence.
Both source builds are generated with NASM -f bin from dpmi_suballoc.asm,
with -DRESTORE_ES only for D36E.COM, below build/M0-T420/S36/dpmi-es-r1.

Formal x86 product-programs was rebuilt through the intentionally reused
formal graph; the VdmTib gate passed. Non-diagnostic deployed worker SHA-256 is
`568292e2640d82cd8c5a542b489bc291e0decefe70a063be3aa3e0651f5853e2`,
and VDMREDIR.dll is
`96c6bc2718c99a269ab42eda91aaed400de77530b6309855f7ec09723a37a399`.
The current memory/output/exit probe is now accepted, superseding the earlier
timeout finding. The original allocation callback rollback findings and their
worker-level failure/cleanup disposition remain open, so S36 is not closed.

Post-repair regression confirms all 17 product routes in
`s36-segment-product-r1-summary.json`, all four XMS routes in
`s36-segment-xms-r1-summary.json`, and a repeated four-case DPMI matrix with
the six-register source-consistency gate in
`s36-dpmi-segment-fix-r2-summary.json`. These summaries reside under
`O:\winnt\logs`; acceptance checks guest text as well as process completion.

## Worker callback failure boundary

The expanded existing `xms_mapped_memory_fixture.c` compiles the production
XMS callback, guest-memory carrier, session and lease translation units under
Win32/x86 CPU40. Only the backing read/write endpoints are test doubles; the
allocator is not linked in this focused boundary test. Generate with
`New-T322XmsMappedMemoryFixtureNinja.ps1 -Architecture x86 -BuildRoot
build/M0-T420/S36/callback-failures-r1`, then build and run the resulting
`x86/xms-mapped-memory-fixture.exe` using MSVC and Ninja.

The run exits zero and reports
`S36_XMS_CALLBACK_FAILURE_LEASE_RELEASE_REUSE_OK` and
`S36_XMS_MOVE_FAILURE_CANCEL_OK`. Injected backing-read rejection reaches the
real commit failure, backing-write rejection reaches the real decommit
failure, and occupying all eight lease slots rejects another commit. Failed
leases retain neither active slots nor bounce buffers; releasing the occupied
slots permits commit again with an active session. Move failure requests the
existing cancellation state, after which lease/session teardown succeeds.

This is boundary evidence, not a real-host out-of-memory reproduction or an
allocator rollback pass. Production SAS read/write endpoints reject invalid
ranges but otherwise return success after their original void SAS operations;
arbitrary backing failure is therefore only an injected test condition.
Lease allocation and slot availability are separate real failure points.
The outstanding work is a composed allocator/worker-callback failure test
and disposition of the original bitmap rollback defect; no production code
or immutable guest media changes are made by this test delivery.

## Composed original allocator and real worker callbacks

The same builder now also compiles unchanged `suballoc.c`; the fixture's
separate `--allocator-failure-audit` mode binds SAInitialize directly to
production xmsCommitBlock/xmsDecommitBlock/xmsMoveMemory. The backing is
128 KiB. Allocate the entire pool, preserve a 4 KiB sentinel at offset 65536,
reject backing writes, and request SAFree for that page. SAFree returns FALSE
and the sentinel survives. Nevertheless SAQueryFree reports 4096 free bytes.
The next 4096-byte SAAllocate returns FALSE: FreeChunk restored FirstFree
from its fixed-size local structure, but not the trailing bitmap it cleared.

The first test incorrectly expected immediate successful reassignment and
exited 23. Source inspection plus that result disproved the expectation;
the corrected assertion checks the observed query/allocation disagreement,
not a fabricated overwrite. The resulting x86 run in
`build/M0-T420/S36/callback-composed-r1/x86` exits zero with
`S36_KNOWN_DEFECT_FAILED_FREE_PHANTOM_CAPACITY_REPRODUCED`. This denotes
successful defect reproduction, NOT allocator acceptance. Normal callback
failure/lease reuse and move-cancellation checks also pass in that build.

The experiment injects backing write rejection; it does not claim that the
production SAS provider normally rejects a valid range. It does prove that
the original rollback defect survives composition with actual worker
callbacks. Product behavior is unchanged. Further disposition must preserve
the original allocation contract and address partial multi-page failure,
not merely repair this single bitmap example or conceal it with success.

## Multi-page consequence and historical callback alternative

The composed fixture now counts backing writes and rejects the second write
during an 8 KiB free. Actual xmsDecommitBlock clears the first 4 KiB, the second
page retains its sentinel, SAFree returns FALSE, SAQueryFree reports 8 KiB
free, and the session remains active. The rebuilt x86 fixture reports
`S36_KNOWN_DEFECT_PARTIAL_FREE_ZEROED_DATA_REPRODUCED`; its ordinary boundary
checks still pass. This disproves a bitmap-only repair: the local callback
has already destroyed part of the allocation before reporting failure.

Source-first review also identifies an earlier unselected alternative:
original `xms.486/xmsmemr.c` calls sas_manage_xms for commit/decommit, whereas
the local callback zeroes through allocating bounce leases. The mirror and
pinned OpenNT file both hash to
`70970ed738101458f8036b5a283f034eb64baf32ead84a59a6b02278b327612c`.
Original `softpc.new/host/src/stubs.c` supplies the CPU_40_STYLE
sas_manage_xms body that prints a diagnostic and returns TRUE without changing
backing. Thus zero-on-decommit is a local behavior, not established by that
original CCPU40 path. The existing adapter rationale discusses the i386
direct-host-pointer backend but does not settle this RISC/CCPU alternative.

Before adding rollback machinery, the next source-recovery check must attempt
composition of that original callback unit, classify its mover's GetVDMAddr /
RtlMoveMemory / sas_overwrite_memory boundary, and verify the selected pool's
backing and caller requirements. No no-op has been installed or declared
correct solely because the original stub returns success. The original unit
and product callback remain unchanged at this checkpoint.

## Original callback translation-unit compile proof

`Test-T420S36OriginalXmsCompile.ps1` extracts the exact formal xmsblock object
flags and compiles the unchanged original xmsmemr.c in a fresh build root.
The x86 attempt at `build/M0-T420/S36/original-xms-compile-r1` exits zero
without warnings; result.json retains source/graph hashes and flags, and
compile.log retains the compiler output. This is a compile feasibility gate,
not a runtime acceptance or product link selection.

dumpbin identifies only memmove, sas_manage_xms, Sim32pGetVDMPointer and
sas_overwrite_memory as undefined providers. The existing formal
ntvdm.exe.map already resolves the latter three respectively from stubs.obj,
sim32.obj and accessfn.obj. There is no demonstrated missing-provider reason
to reject this original unit. The original CCPU overwrite provider is itself
an intentional no-op because CCPU has no compiled-code cache to invalidate.

The remaining mover issue is precise: Sim32pGetVDMPointer(0,FALSE) returns
the SAS pointer for address zero (not an acquired bounce lease). Adding an
arbitrary source/destination offset to that pointer bypasses subsequent
physical alias/external-map resolution performed by c_GetPhyAdd. Thus a
successful compilation does not prove that the unchanged flat-base mover
honors this product's selected mapped-memory contract. The next recovery
step must retain bounded mapped movement while composing the original
commit/decommit owner; it must not recreate all three callbacks solely to
avoid the mover boundary. No production or guest file changes accompany
this compile proof.

## Original SAS callbacks restored in the formal product

The formal graph now adds original xmsmemr.c to its six common XMS units.
Its original commit/decommit bodies are unchanged; original CPU40
sas_manage_xms already owns the preallocated-backing operation. The only
mirror insertion (DIV-271, eight added lines) routes the flat-base mover to
the existing bounded mapping-aware operation. Local mvdm_xms_memory.c loses
44 lines and gains one renamed signature, its header gains one declaration:
net 42 fewer worker implementation/header lines. There is no new overlay,
allocator implementation, mirror file or guest modification.

This selects source-recovery rung 2: original translation unit plus one
finite mapped-movement hook. Rung 1 was actually compiled, but its flat-base
pointer arithmetic bypasses the selected physical mappings. No external
intrusion or new callback policy is required. Generic suballoc callback
rollback defects remain true of providers that can fail; the selected original
SAS commit/decommit provider does not use malloc, leases, range writes or
decommit, and therefore does not introduce those failure transitions.

The revised composed fixture fills all eight lease slots, rejects backing
I/O, frees and reallocates two pages through the original callbacks, and
checks capacity, untouched bytes and zero backing accesses. It also verifies
overlapping mapped movement and failure cancellation. Both markers pass in
`build/M0-T420/S36/original-callback-fixture-r1/x86`. The startup fixture's
two original branches pass in `build/M0-T420/S36/original-startup-r1`; its
old zeroing assertions were local-policy expectations and are replaced by
the source-proven no-clearing behavior. These fixture SAS endpoints are
explicit mocks; real product selection is independently verified by the map.

A fresh 526-edge Win32/x86 formal build completes in
`build/M0-T420/S36/original-callback-product-r1`, followed by a final incremental
relink after the mover conditional was tightened. ntvdm.exe.map places all
three xms callbacks in original-mvdm-xms:xmsmemr.obj and sas_manage_xms in
original-softpc-host-roots:stubs.obj. Deployed non-diagnostic hashes:
ntvdm.exe `d24629a9a55e6423ae8ae1d46a8204b917fb591743281e45cff8f4ef694706c5`;
VDMREDIR.dll `6f5d28e1d5a8c4c96f5f7a9f90ee66851d42cfb51682ac6e9a8d0f8caa169012`.
All four real XMS routes pass under `s36-original-callback-xms-r1`, and all
four real DPMI routes pass under `s36-original-callback-dpmi-r1`. Raw summaries
and guest transcripts remain under `O:\winnt\logs`.
All 17 text-gated product routes also pass in
`s36-original-callback-product-r1-summary.json`, including COMMAND nesting,
MEM, EDIT return, native streams/EOF and expected guest exit codes. This is
the recovery delivery; final S36 capability/teardown closure remains separate.
