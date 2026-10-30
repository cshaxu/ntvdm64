# T473 S11 CPU40 monitor â€” original recovery and minimization audit

Status: active audit and implementation, not S11 closure. Owner's 2026-10-30 request admits this complete monitor review inside S11. Baseline: `350dde1ca`, published ten-image package identified in [descriptor/reentry evidence](s11-foundation-evidence/string-descriptor-reentry/publication.json). Original guest/media remain immutable. The selected implementation uses CPU40 CCPU, not a request to repair 486-specific internals.

## Accounting boundary

Against `2f5273440^`, before the monitor group, at baseline the three mirrored files have textual changes: `nt_inthk.c` +326/-1, `c_tlb.c` +40/-0, `c_main.c` +53/-11 (git numstat; comments and blanks included). Total +419/-12, net +407 across these files. This is a task-start comparison, not percentage of executed code or an OpenNT-to-current semantic proof. New non-mirror test/evidence files and existing independently developed adapters are accounted separately when the final diff is known. Whole-mirror per-file estimates remain in [retention CSV](mirror-diff-code-retention.csv).

## Calling chain and bounded decisions

| Group | Original authority and dependency | Current connected chain and evidence | Classification, decision, unresolved boundary |
| --- | --- | --- | --- |
| Decode and failure | NT4 `ke/x86/instemul.asm:Ki386DispatchOpcode`, `OpcodeGenericPrefix`, `Opcode0F`; `vdm/x86/vdmop0f.asm` and `vdmoprnd.asm` use NT kernel trap frames, selector and VDM process state. Failed dispatch reflects through `Ki386VdmReflectException` in `trap.asm`. | CCPU `c_xcptn.c:contributory_exception` calls `host_exint_hook`; host `monitor_fetch_byte` and opcode branches. Fixture cases and full-package results are in [monitor evidence](s11-foundation-evidence/code32-prebuilt-search/review.md). | Host decoding is an independent adaptation, not directly imported original code. Keep only branches with source-defined semantics; compare all prefix, length and failure outcomes. Importing the assembly wholesale brings kernel trap-frame/selector/exception machinery, not a local helper. Actual CPU `d_mem` masks 16-bit offsets and sign-extends forms differently from `VdmDecodeOperand` (cases 53â€“54), so direct substitution is not equivalent. |
| CLI/STI and IRQ | NT4 `OpcodeCLI`, `OpcodeSTI`, `SetVirtualBits`, `VdmDispatchIntAck`; original SoftPC ICA owns interrupt acknowledgment. | Host updates the existing 714h state and selected CCPU flags; `ccpu_monitor_dispatch_irq` consumes the CPU event and calls the same factored `ccpu_dispatch_accepted_irq` used by normal CCPU dispatch. Selected pending/masked/AEOI/cascade/concurrency fixtures; no proof of every interrupt interleaving. | Keep the shared CPU dispatcher and atomic event boundary: it connects existing owners, not a second PIC. Recheck original flags, completion and nested delivery; assess whether the 714h read and lock can use an existing owner without changing atomicity. |
| Scalar and string I/O | NT4 `OpcodeIN*`/`OUT*`, `Ki386VdmDispatchIo`/`Ki386VdmDispatchStringIo`; original MVDM `EventVdmIo`/`EventVdmStringIo` and IOS tables. Original kernel and user string paths have different buffer handling. | Host routes scalar to original `IN8/16` and `OUT8/16`, string to original IOS entries after mapping and selected SIM32 descriptor provider. Cases 10â€“14, 15, 21â€“25, 28â€“29, 52, 59â€“61, 64â€“75 exercise selected boundaries. | Existing I/O implementations are reused; the host's validation, buffer and index logic is an adaptation. Contiguous guest storage is live, noncontiguous storage still stages: reentrant visibility, remapping and partial progress are not equivalent and remain open. Do not claim parity from these fixtures. |
| LMSW, CR and DR | `VdmOpcodeLmsw`, `VdmDecodeOperand`, `KiVdmSetUserCR0`, `VdmOpcodeGet/SetCrx`, `VdmOpcodeGet/SetDrx`; NT4 NPX save area, trap frame and scheduler state. | Host decoder uses CCPU register getters/setters, `c_get/setCR0`; `ccpu_monitor_write_dr` invokes original `MOV_DR` and existing breakpoint check. Cases 31â€“51 and 53â€“58, plus cross-page controls, cover selected results. | Register switch/operand logic is independent code; audit use of original index APIs without changing NT4's operand quirks. CR0's CCPU state is not automatically NT4's NPX context; preserve open NPX/DR lifetime and failure boundaries. |
| Address, paging, TLB and A/D | NT4 `CsToLinearPM`, `VdmDecodeOperand`, SIM32 selector conversion and native memory/exception machinery; selected CCPU `lin2phy`, `xtrn2phy`, `c_tlb.c` and SAS. | Host preflights RAM/page-table bounds, asks `ccpu_monitor_cached_access`, then calls `xtrn2phy` and `monitor_mark_access`. `ccpu_monitor_cached_access` duplicates the cache-hit portion of `xtrn2phy`; tests include stale/multiple mappings, permissions, A20, A/D and wrapped addresses. | Duplicated cache lookup is a concrete minimization target: share one decision with `xtrn2phy` only if tri-state miss/denial/hit and FAST_TLB priority are preserved and no unsupported 486 repair is introduced. Bounds/unsupported memory checks are not automatically redundant: original native mappings differ from SAS. Current CS/LMSW use cached segment descriptors whereas NT4 looks up selector parameters: verify changed-descriptor cases before changing. Exact instruction-fetch A/D, missing pages, aliases and 4-GiB-end behavior remain open. |
| Reentry and failures | NT4 `trap.asm:Kt0e8` restores the user frame and dispatches a user access exception for unresolvable faults in kernel VDM support; guest page faults use a distinct path. Original CCPU `c_xcptn.c` and SIM32 own their state. | Current host fallback calls the existing exception handler; it suspends the accepted contributory-fault state around descriptor resolution to avoid stale #DF on nonlocal exit. Cases 73â€“75 establish one nested #PF, later I/O, and normal failure restoration. | This is an adapter boundary, not proof that NT4 user exception delivery is restored. Trace the actual DOS and WOW exception owners, nesting and lifecycle before replacing fallback. Noncontiguous buffers cannot be fixed by merely copying elsewhere or adding another unowned authority. No new observable concession is approved. |

## Finite implementation sequence

1. Establish whether a single shared CCPU cache-hit routine can replace the two existing copies without changing cached deny vs miss, fast/slow ordering, write-dirty behavior, A/D side effects, original public return contract or unselected configurations. Measure both mirror-text and total code effects, prove positive and negative cases first; reject the change if semantics or total complexity are not improved.
2. Reconcile current CS and LMSW selector/base lookup with the original paths using changed-descriptor positive/negative controls and existing selected CCPU/SIM32 owners; only implement any correction with exact evidence and exception-state handling.
3. Finish the noncontiguous string-I/O and original user-exception call-chain assessments. Reuse existing mappings and dispatch if it preserves synchronous callback visibility, fault ordering and lifecycle; otherwise retain and explicitly document the bounded gap. Do not implement a semantic retreat.
4. Review all remaining six groups against the table, consolidate only proven duplicate/dead code, register every mirror exception, then run cached builds, focused boundaries, required DOS/WOW/Win3.x and Full package gates, review, publish coherent ten-image package, commit and push. Preserve earlier failing evidence and unrelated work.

This is a finite review of this monitor group, not a promise that downstream S stages are complete. The initial product candidate had a Window-17 input-observation failure before an unmodified full rerun passed; this remains recorded in the baseline evidence and is not attributed to a proven cause.

## C1: shared cache decision â€” candidate

`xtrn2phy` already implements the same fast-index lookup, slow-table hit, protection and dirty checks copied into DIV-474. A pre-change real CCPU witness (`cache-share-baseline`, case 76) records miss=0, hit=1, cached denial=-1, clean-write miss=0, plus original public translation success/failure. The candidate keeps that original lookup and page walk in one private core, with the existing public `xtrn2phy` Boolean wrapper and existing monitor tri-state wrapper. Cache-only denial and miss return before page walking; normal translation retains original A/D behavior. This removes the separate copied lookup rather than moving it to another component. On selected CCPU, all 75 fixture cases pass (`cache-share-minimal`), including the prior fast-table/duplicate mapping and paging controls. The `SPC486` check is the existing original branch; no 486-specific repair is performed or claimed tested.

Relative to the immediate baseline, mirrored `c_tlb.c` is +13/-36, net -23 lines. Relative to `2f5273440^`, its mirror text changes shrink from +40/-0 to +18/-1 (comments/blanks included). This intermediate candidate was superseded by the smaller shared-core implementation and the fully verified product package below.

## Noncontiguous storage and failure-owner evidence

The selected SAS physical store is reserved with `VirtualAlloc` (`host/src/nt_mem.c:InitIntelMemory`) and `c_GetPhyAdd` resolves physical bindings (`base/ccpu386/ccpusas4.c`). Two nonadjacent guest physical pages do not thereby form one contiguous native pointer. Original user `EventVdmStringIo` passes a direct SIM32 pointer; the distinct NT4 kernel `VdmCallStringIoHandler` deliberately uses a kernel buffer. The selected SoftPC IOS table is reached through the user-event-equivalent path, so citing the kernel buffer does not establish equivalent reentrant visibility. Creating an additional copied alias or imposing a new shared-memory backing is not an already established same-semantics solution. The two-byte noncontiguous fallback remains explicitly partial while existing page/mapping owners and reversible alias options are examined.

For unresolvable monitor-internal reads, `trap.asm:Kt0e8` dispatches a user exception from the saved user frame; the selected CCPU callback runs inside the exception delivery and may nonlocally return. `W32Exception` is present at WOW thunk boundaries and its existence alone does not establish a DOS or nested CPU-to-WOW exception handoff. Calling `RaiseException` or substituting guest #PF/#GP without tracing that owner would change observable behavior. No replacement is admitted by this observation.

## Selector ownership finding

A direct NT4 source check adds a material qualification to the existing tests: `CsToLinearPM` calls `Ki386GetSelectorParameters`, which accepts a user LDT selector and requires an executable segment; it reads the current descriptor rather than the CPU's hidden segment cache. Many existing selected-CCPU fixtures use synthetic user GDT selectors, so their successes do not prove NT4's CS-selector admission semantics. The current monitor accepts those fixtures through cached CS base/limit. The real installed guest selector and error path must be traced before restricting this path: rejecting previously accepted GDT execution without that evidence would be a new observable compatibility change. Changed-descriptor tests should use a real CCPU LDT as well as the existing GDT controls. The same review applies to LMSW default DS/SS lookup, whose original `VdmSegParams` uses the descriptor table. This is not resolved by the TLB-cache simplification.

## C2: original descriptor view â€” candidate

`CsToLinearPM` resolves a present executable descriptor and snapshots its base/limit before dispatch; `VdmSegParams` resolves the default LMSW segment through the current table. The old adapter instead used hidden CCPU segment-cache values. A real CCPU LDT witness loaded CS, changed its descriptor, and then trapped I/O: the original-source expectation is the new descriptor's opcode; the old monitor executed the cached opcode (`ldt-selector-baseline`, cases 77/78). Additional controls changed the current code descriptor to non-executable or shortened its limit, and changed the current DS descriptor before LMSW; the old monitor failed cases 79â€“81. The candidate extends the already-owned CPU40 descriptor provider to expose base/limit/access, snapshots CS once, and uses current DS/SS for LMSW. It reuses one descriptor walk, rather than recreating another descriptor parser. Existing GDT-admission compatibility remains unchanged and explicitly differs from NT4's LDT-only selector check; no claim is made that this boundary is resolved. Descriptor lookup uses the existing nonlocal-exception scope and cases 73â€“75 still pass.

## C3: original IOS entries â€” candidate

The original SoftPC `base/support/ios.c` contains `insb`, `insw`, `outsb` and `outsw`, gated to `NTVDM && MONITOR`; the selected CCPU production link had only the tables, not these four entries. Enable those unchanged original bodies for CCPU and call them from the host string adapter instead of maintaining a second table lookup. The `LOCAL` reset in that conditional has no subsequent LOCAL definitions in this translation unit; the preexisting MONITOR path is unchanged. Case 82 uses different input and output adapter registrations and confirms the original monitor's input-adapter routing even for output. All 81 selected CCPU cases pass (`monitor-original-ios-routing`). This does not resolve noncontiguous buffer aliasing or descriptor-fault delivery.

## C4: register access â€” candidate

The original CCPU `c_reg.h` defines `A_EAX` through `A_EDI` in ModR/M order and `GET_GR`/`SET_GR` over its `CCPU_GR` array. `c_reg.c`'s eight getters and setters do exactly those accesses, including ESP, with no extra state update. Before changing the monitor, case 83 exercised all eight register indices in both CR-read and CR-write monitor operations and verified that untouched registers remain unchanged (`monitor-register-baseline`). The candidate replaces both duplicate switches with the original CPU macros; the same real-CCPU matrix and all 82 selected cases pass (`monitor-register-candidate`). This is reuse of the single CPU register authority, not a replacement register array or a claim about every debug/NPX behavior.

## Integrated review and accounting before product gates

The C1 implementation was simplified further after comparison: instead of deleting the original `xtrn2phy` cache loop, both existing APIs now enter a private core containing that original loop and page walk. Its cache-only mode distinguishes denial from miss; its original public Boolean path still follows the original walk on a miss. Immediate mirror diff is +13/-36 in `c_tlb.c`, net -23; against pre-monitor `2f5273440^`, this file's difference is now +18/-1 instead of +40/-0. The selected CCPU witness retains the same hit, miss, denied and dirty-write outcomes, including the duplicate FAST_TLB mapping case. No unselected 486 runtime result is asserted.

C2's LDT controls now include current CS base, non-executable/not-present/short-limit CS, and current DS and SS bases for LMSW. The former monitor failed changed-CS case 78 and the independent negative cases 79â€“81; the candidate passes cases 77â€“81 and 84â€“85. Existing GDT acceptance is deliberately retained as an explicitly open difference rather than silently narrowed. Cases 73â€“75 continue to exercise descriptor page fault/reentry and restoration. The selected full fixture currently passes 84 cases (`monitor-minimal-boundary`).

For the four mirrored implementation files against `2f5273440^`, current textual diff is `c_main.c` +53/-11, `c_tlb.c` +18/-1, `ios.c` +2/-1 and `nt_inthk.c` +318/-1: total +391/-14, net +377. At audit start the comparable monitor group was +419/-12, net +407 (the `ios.c` change was zero), so mirror net is 30 lines smaller. Relative to the audit-start commit, the three changed mirror files are +65/-95, net -30. These are Git line counts including comments/blanks, not recovered behavioral percentages. The independent project adapter and header are +20/-4, net +16, because the shared descriptor provider now also returns the existing parsed limit and access. Combined implementation change from audit start is +85/-99, net -14; tests, documentation and release binaries are excluded from this accounting.

Noncontiguous string-buffer reentry and native monitor exception delivery remain partial as described above. The code-segment selector-kind mismatch, instruction-fetch A/D behavior, device-backed operands and mapping changes during callbacks remain unproved. These are not declared equivalent or converted to new approved behavior. This audit does not assert full S11 or whole-T closure.

## Product and original-mirror accounting

For actual OpenNT source comparison, normalize line endings and apply `git diff --no-index --numstat` to the four whole mirrored files, including historical differences outside this monitor group. `c_main.c` remains +199/-39; `c_tlb.c` changes from +40/-0 to +18/-1; `ios.c` from +0/-0 to +2/-1; `nt_inthk.c` from +354/-0 to +346/-0. Overall original-to-working-tree text changes go from +593/-39 (net +554) to +565/-41 (net +524): 28 fewer added lines, two more deleted original lines and net 30 fewer lines. This is textual accounting, not a claim that unchanged code is connected or that remaining changes are semantically equivalent. Exact per-file counts and normalized inputs are retained with the final evidence.

The independent compat files `mvdm_softpc_effective_address.c` and its header change +20/-4, net +16, relative to audit start; they expose the original CCPU descriptor view and factor its existing address calculation. These counts classify by file location: autonomous monitor code inside mirrored files is included in the mirror count, not double-counted as compat. Combined production source delta is +85/-99, net -14; test, documentation, binaries and existing unrelated adapters are excluded. This calculation complements, rather than replaces, whole-mirror retention estimates.

The selected target build completed all eight cached Ninja steps; an immediate no-change build reported no work in all eight. Original SoftPC IOS entrypoints and the CPU descriptor/cache owners are present in the actual `ntvdm.exe` link map. The selected CCPU boundary suite passed 84 cases including a 16-operation all-register matrix, separate input/output adapter routing, LDT descriptor changes, descriptor fault reentry, FAST_TLB and access/dirty controls. The CALLF and external-address selected-host fixture passed. The complete 16-gate package verification and Winmine/SOL/WRITE/four SOL Options witnesses passed. A test-owned copy of the current Win3.1 installation passed standard/enhanced x DOSONLY 0/1, each with startup, expected mode and normal exit. Before and after, 86 system binary hashes matched both the original read-only installation and the isolated copy; only test-copy configurations and independent test probes were changed. Ten components were published to each destination and all 20 hashes matched. Product gates and these witnesses do not prove the remaining noncontiguous, exception, selector-kind, device-memory or arbitrary reentry boundaries.

Compact build, original-diff, baseline, verification, publication and isolated Win3.1 evidence is archived in [monitor-minimization-evidence](s11-monitor-minimization-evidence/). The previous baseline’s first Window-17 observation failure and later unmodified pass remain in the separately linked foundation evidence; the cause is not asserted.

## CR0 EM/MP four-state boundary witness

NT4 `vdmnpx.c` defines four input mappings and eight output mappings to
shadow the guest-visible EM/MP values separately from kernel scheduling state.
`KiVdmSetUserCR0` installs the mapped state and `VdmOpcodeGetCrx` maps it
back, clearing the scheduler TS bit in the returned value. Selected fixture
case 86 exercises all four guest input pairs through the actual `nt_inthk.c`
MOV-to-CR0 and MOV-from-CR0 dispatch, verifies other CCPU CR0 bits are
unchanged by the write, checks the readback pair and checks TS is hidden.
The selected fixture build and all 85 boundary cases pass. This tests the
observable bit projection only: it does not establish that CCPU and NT4 have
equivalent FPU execution, NPX context switching, actual #NM delivery,
callbacks or process teardown. The fixture uses its documented
`/force:multiple` host seams and no guest binary is changed.

## Instruction-fetch page-access accounting

NT4 `Ki386DispatchOpcode` and `OpcodeGenericPrefix` read instruction bytes
through the host virtual address, so a real page-table walk accounts for the
read. The selected CPU40 host adapter previously translated and read bytes
without the access-marking step. Real selected-CCPU case 87 starts with a
cold translation and clear instruction PTE A/D bits. Before the change its
I/O instruction completed with PTE `000f1007` (accessed still clear); after
using the existing `monitor_mark_access` / `xtrn2phy` path it completes with
`000f1027` (A set, D clear). Case 88 warms the old translation, then changes
the PTE to a different page without a TLB invalidation: execution still uses
the valid old cached mapping and the replacement PTE remains `000f2007`.
These witnesses distinguish walk accounting from incorrectly consulting a
new PTE for an existing cached translation. They do not establish all
concurrent remapping, exception, device-backed or host-processor cases.

The 87-case selected CCPU boundary suite passed, followed by cached full
product build, all 16 Full gates, Winmine/SOL/WRITE and four SOL Options
cycles. Win3.1 standard/enhanced by DOSONLY 0/1 passed, and the 86 monitored
original and isolated guest binaries retained their hashes. The coherent ten
component package was published to each destination and all 20 hashes
matched. The initial Full run used Windows PowerShell 5.1 and failed in the
RPC harness because `ProcessStartInfo.ArgumentList` was unavailable; the
recorded complete rerun used PowerShell 7 and passed. Compact evidence is in
[fetch-access](s11-monitor-minimization-evidence/fetch-access/).
