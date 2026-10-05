# T430 non-WOW contract completion plan

## Authority and scope change

Owner direction on 2026-10-04 supersedes the candidate's audit-only S2–S4
sequence: close S1's inherited/current audit, then plan required non-WOW gap
completion within this task. Known WOW gaps go to the existing WOW32
candidates, not a parallel recovery program. CURRENT is the only admission
authority. S1 and S2 are delivered; S3's applicability review retains the
original selected-NTVDM response limitation without adopting an inapplicable
SoftPC patch. See [S3 proof](../evidence/m0-t430-s3-controller-pic-boundary.md).
S4 is delivered as fb5843dc6. S5 a143894f1 proves ordinary FCB/fallback
behavior and retains original deleted-name/missing indexed-service limits.
S6 is now admitted in CURRENT; S7 remains planned, not active.

[S1 conclusions](../evidence/m0-t430-s1-inherited-contract-audit.md) freeze
inputs and distinguish source risk, missing evidence and original limitations.
The T429 eight-file package is the S2 recovery baseline; the delivered S2
package and bounded profile proof are in the [S2 evidence](../evidence/m0-t430-s2-softpc-repair-gate.md).

## Sequential stages

Owner correction on 2026-10-04 restricts every stage: original guest defects
remain untouched; original host defects use only an existing matching SoftPC
repair, otherwise TODO and deferral; attributable project adaptation defects
receive designed, verified fixes. Provenance overrides file location. Unknown
risks first need proof, not a novel original repair to satisfy a checklist.

| Stage | Bounded deliverable | Required exit evidence |
| --- | --- | --- |
| S1 | Current/inherited reconciliation, source dispositions and unique gap owners. | Identity checks, current receiver ledger, qualified findings, non-WOW plan and WOW handoff; documentation/tool verification, commit/push. Research closure only. |
| S2 | CCPU40 privilege-changing stack profile: CALL gates, outer RETF/IRET; compare pinned source and referenced SoftPC repair, then apply the smallest justified correction. | Actual instruction entrypoints; operand16/32 × new SS address16/32; high ESP bits, frames, stack adjustment and fault-before-commit. Preserve interrupt/VM return. Helper-only tests do not close it. Record applicability of scalar, RMW/string, stack and control groups; no CPU30/global opcode proof. |
| S3 | Original 8042/PIC boundary: C0 input-port response and output-buffer-read IRQ1 deassertion. | Initialized response, masked polling, deassertion, subsequent queued bytes and normal interrupts. Review non-JOKER semantics before changing them. Session keyboard reset is not this repair; no bulk sibling port. |
| S4 | VDMREDIR K02–K04: original ABI/order comparison, controlled reproduction, then minimal corrections to demonstrated host/adapter differences. | OEM byte capacity/terminator, CX0/1 and DBCS; async error/count/payload destinations failing separately, callback ordering, cancellation/dead session and release once; CD-name NULL/overlap/unwritable buffers and NetAPI cleanup. K04 has no original error return: no invented CF/AX. K03 is not presumed atomic without original comparison. |
| S5 | DEM directory reset K01: original indexed restart versus current slow fallback under mutation. | Real FCB enumeration after remembered-file deletion/reordering, status and handle cleanup. Recover the smallest original VdmQueryDir carrier/binding only if behavior requires it. Otherwise retain a documented performance limitation, not a dummy binding. |
| S6 | Current non-WOW DPMI/XMS/EMS/IRQ/lease proof completion, not a new census. | Identity-reconcile existing proof, then supplement selected unproved boundaries: invalid DOSX IDT/no partial CPU mutation, PM-stack/exception-return lifecycle, EMS AH56/BOP68 stack/mapping restoration, lease failure/teardown and IRQ sentinel/concurrent notification. Reuse compatible proof; external-device prerequisites and original guest defects remain non-pass. New owner/scope needs brief revision. |
| S7 | Integrated non-WOW closure and final ledger. | Each planned obligation proved or explicitly dispositioned, no orphan repair; coherent x86 package/hashes, Console17/Window17, DOS↔native final-frame/receipt/cleanup and independent WOW frontier non-regression. Owner acceptance before T closure. |

S2/S3 concern registered host-source defects. S4–S6 are investigate-and-complete
stages, not claims that all suspected paths are broken. Retained limitations
keep their reproducer and reason; never weaken an assertion to pass.

The [source gate](../evidence/m0-t430-s2-softpc-repair-gate.md) confirms the
referenced SoftPC commit contains the S2 stack-width and S3 controller fixes;
S3's selected-branch proof supersedes the assumption that both fixes apply:
the IRQ correction is excluded under NTVDM, and C0 assignment alone cannot
repair that branch's response delivery. No production change is justified.
S4 separates original
CX0/other inherited defects from project Unicode/OEM and staging adaptations;
without a matching SoftPC repair, originals are TODO, not fixed incidentally.
S5 may restore missing project service binding but not invent a correction
to an original fallback algorithm. S6 supplements proof under the same gate.

S2/S3's comparative reference is SoftPC commit
ce5f53515d3e6ce0a64e66a5465aa7a8fbca00f7. It is evidence, not a source/build/
runtime dependency or permission to import the whole patch. Exact source
policy, body identity and every affected profile test remain mandatory.

## Implementation gates

- Original owner first: directly composed source, smallest same-shaped facade,
  registered exceptional hook, new mechanism only when earlier rungs cannot
  compose. Record provenance, unavailable dependency and mirror diff.
- Commit tests with implementation. Invoke the selected production provider
  and guest boundary where applicable; mocks supplement, not replace proof.
- Every production-code delivery: affected x86 /MT CCPU40 build,
  positive/negative/lifecycle checks, Console17/Window17, independent WOW
  frontier non-regression, coherent eight-file publication/recovery/hashes,
  diff review and commit/push. No runtime gates for documentation-only P.
- Runtime groups remain serial (BaseSrv/Z:). No guest patches, helper,
  second executor, scheduler, CPU30 or permanent per-instruction trace.
- WOW missing capabilities are not repaired here. Existing accepted frontiers
  must not regress, and their outstanding obligations retain owners below.

## Known WOW handoff

The [WOW32 program](../../proposals/proposal-wow32-production-completion-002.md)
retains its unique-owner order. This handoff does not admit those T packages
or certify every WOW contract.

| Boundary | Receiver | Required next evidence |
| --- | --- | --- |
| G03 BOP51/task/dispatch/callback return | [W01 execution](../../proposals/proposal-wow32-message-task-execution-001.md) | Actual production call/return, failure, reentry and cleanup; reuse T422 evidence only under matching inputs. |
| G03/G13 guest USER/object shared view | [W02 shared view](../../proposals/proposal-wow32-user-objects-shared-view-001.md) | View ownership, identity, mutation and teardown; an unused publisher symbol proves nothing. |
| W01 inherited WRITE initialization OOM; first provider unknown | [W03 modules/memory](../../proposals/proposal-wow32-modules-memory-aliases-001.md), initial diagnostic receiver | Original initialization's first failed load/allocation under pinned media. Diagnostic ownership is not heap attribution. Transfer to another existing owner only with exact first-interface evidence; do not patch WRITE. |
| Resource discrepancy if proven as first WRITE failure | [W05 resources](../../proposals/proposal-wow32-resource-loading-001.md) | Actual resource-provider difference, not generic OOM dialog. |
| O01 original SOUND six unimplemented ordinals | [W16 sound](../../proposals/proposal-wow32-sound-multimedia-001.md) | Actual ordinal/wCallID if reached; retain original limitation until admitted provider recovery. No claim WINMINE currently hits it. |

Other WOW families stay with their assigned candidates; W20 acceptance cannot
absorb unfinished implementations. Queue order is unchanged.
