# Remaining mirror differences: original-first re-audit

[review-register.csv](review-register.csv) is the current ordered re-audit
worklist. [progress.json](progress.json) records the baseline and counts;
[file-pins.json](file-pins.json) pins current file bytes;
[excluded-records.json](excluded-records.json) prevents reopening completed
work or counting deleted files as current mirrors. The old T483-D register and
execution ledgers remain history/evidence, not a second active work queue.

## Binding method

1. Review T483-R0001 onward strictly in sequence, directory order then the
   approved within-file difference order. Do not cherry-pick categories.
   Historical line anchors must be relocated against current pinned source
   before a conclusion; record current ranges. If a point has disappeared,
   prove it and retire it. A missing previously unregistered difference must
   be inserted at its file/range position with a stable ID and evidence.
2. Presume the original implementation is valid. The author of a deviation
   bears the burden of proving that restoration breaks a supported actual
   context. Accept concrete source/ABI proof or observed compile/link/runtime
   failure tied to that context. Generic risk, hypothetical remapping,
   defensive preference, prior acceptance, historical classification and
   passing modified-code tests alone do not prove original code must change.
3. If exact original restoration is appropriate, restore it directly under
   the owner's authorization, preserving original line mapping and necessary
   coupled caller consistency. Do not replace an unproved adaptation with a
   different autonomous implementation. Never modify/rebuild original guest.
4. If a difference is necessary, establish its smallest possible form. Do not
   move equal complexity into shims merely to reduce mirror diff.
5. Implement evidence-backed reductions of necessary deviations directly.
   Keep residual differences in the pending decision backlog; minimization is
   not retention approval. Only net increases require prior implementation
   approval. Exact original restoration remains directly authorized.
6. Missing evidence is not a retain decision. Investigate the bounded actual
   call chain; record what is unknown. No blanket restoration without checking
   actual callers, compile context and source policy, and no invented original
   bug. Dependencies may be read without completing later queue items.
7. Update the item, progress and current snapshot at every delivery. A completed
   audit is not a completed repair; keep implementation/verification/delivery
   explicit. Complete a point before moving on; do not repeatedly count it.
   Apply the impact-based, complete-file batch cadence below, with cached full
   build, regression, coherent publication, commit/push and approval rules.

## Initial scope

2720 entries across365 files:1765 formerly retained,200 formerly correct,
755 formerly pending evidence. All start pending under the new standard.
Of3042 old entries,8 deleted-file records and313 other completed records are
excluded; D2258 is additionally excluded because vdmapi.h now matches its pinned
original byte for byte. It must not be repaired again. These are entry counts,
not counts of compiler diff hunks or independently reproduced failures.
The current host C/H mirror population is1052 files; deleted files are outside
that population. No product changes or fresh runtime verification are claimed
by list initialization. S6 is not separately admitted.

## Per-point owner report

Report each new R point separately: current added/deleted logical code lines
against the pinned original (state treatment of #line/format), cause and problem,
restoration feasibility and disposition, then cumulative reviewed, fully restored,
pending-decision and unreviewed counts. Approved retain is separate; partial
restoration remains pending until the whole point is resolved.

## Owner correction: strict decision admission

A pending owner decision requires concrete evidence that removing the difference
and coherently restoring the original would break an actual supported contract.
A current caller dependency, an unverified comment, missing research, or inability
to prove safety is not sufficient admission evidence. Technical uncertainty stays
with the agent as unfinished investigation, not an owner decision. When original
restoration is feasible and verification reveals no issue, restore it directly.
Only evidenced necessary deviations and their minimal design tradeoffs enter the
owner decision backlog. Existing backlog entries must be requalified against this
standard; their historical reviewed count is not a count of qualified conclusions.

## Current verification cadence

There is no numeric file threshold. The owner's examples of three or five
files are illustrative, not batch sizes. Pure newline, formatting and
reference-only changes without binding or behavior changes may accumulate.
A substantive original restoration or necessary-diff reduction triggers delivery of the whole accumulated
batch only after every outstanding difference in the current file and its
coupled batch edits has been fully handled. Do not build a partially handled
file. An unresolved investigation is not a completed file or owner decision.

Then run cached full compilation, coherent publication, applicable tests,
commit and push under the existing delivery rules. P5 was already built and
published when the complete-file refinement arrived; its prerequisite change
does not establish completion of cmdmisc.c or cmdexec.c.

## Owner refinement: same-file minimization

Implement supported reductions of necessary differences directly, whether or
not the file already contains an original restoration.
Net increases still need prior approval. Minimization does not approve residual
differences: keep them in the pending backlog with current evidence and size.
Only fully restored points leave that list.

R0029 removes activate and restores original cmdExec32 assignments; two captured
pointer callsites remain pending. The prior P6 candidate passed Full, but does
not validate these newer edits. [Evidence](r0029-minimization.json).

The complete current reporting and delivery standard is the authoritative
[17-rule execution policy](../../../docs/rules/EXECUTION.md#original-first-mirror-diff-audit).

Current int21map.c call-chain evidence: [R0187-R0203](r0187-r0203-call-chain-review.json);
[local minimization](int21map-local-minimization.json),
[line check](batch-p20-line-check.json),
[register consistency](batch-p20-register-consistency.json), and
[P20 checkpoint](batch-p20-checkpoint.json). Source conclusions do not imply delivery.

[P20 binary comparison](batch-p20-binary-identity.json) identifies exact metadata-only
differences; it is separate from runtime verification.

Current modesw.c review: [R0204-R0213](r0204-r0213-call-chain-review.json).
Two BOP functions restored to their original i386/dpmi386.c owner.
[P21 checkpoint](batch-p21-checkpoint.json) records cached build, publication11x2,
focused and Full16 verification; this receipt accompanies the P21 delivery.

Current dbgexp.h ABI review: [R0222-R0224](r0222-r0224-debug-abi-review.json).
Three declarations remain pending; comment-only reduction was delivered in P24.

Current demexp.h ABI review: [R0225-R0227](r0225-r0227-dem-abi-review.json).

Legacy inc/makefile restoration: [R0228-R0229](r0228-r0229-legacy-makefile-review.json).

Memory ABI review: [R0230-R0234](r0230-r0234-memory-abi-review.json); original-header Gz/Gd comparisons.

P24: [checkpoint](batch-p24-checkpoint.json), [line check](batch-p24-line-check.json), [R0235](r0235-mvdm-access-review.json).

P25: [softpc.h complete ABI review](r0236-r0272-softpc-abi-review.json),
[line check](batch-p25-line-check.json), [checkpoint](batch-p25-checkpoint.json).

P26: [suballocator review](r0273-suballocator-granularity-review.json),
[line check](batch-p26-line-check.json), [checkpoint](batch-p26-checkpoint.json).

P27: [async header review](r0274-r0276-async-header-review.json),
[line check](batch-p27-line-check.json), [checkpoint](batch-p27-checkpoint.json).

P28: [WOW header review](r0277-wow-header-review.json), [line check](batch-p28-line-check.json), [checkpoint](batch-p28-checkpoint.json).

P29: [OEM file review](r0278-r0290-oem-file-review.json), [line check](batch-p29-line-check.json), [checkpoint](batch-p29-checkpoint.json).

P30 directory provider: [directory provider review](r0291-directory-provider-review.json).

P30 whole-file: [review](r0291-r0340-oem-process-review.json), [line check](batch-p30-line-check.json), [checkpoint](batch-p30-checkpoint.json).

P31: [BIOS line restoration](r0341-bios-line-review.json).

P31: [Reset continuation call-chain proof](r0342-r0343-reset-call-chain.json).

P31: [Slave EOI minimal-diff review](r0344-slave-eoi-review.json).

P31: [Tape XMS declaration review](r0345-tape-xms-review.json).

P31: [Virtual whole-file investigation](r0346-r0352-virtual-investigation.json) (not a completed disposition).

P31: [Virtual completed source review](r0346-r0352-virtual-review.json); [batch checkpoint](batch-p31-checkpoint.json), [line checks](batch-p31-line-check.json). P31 verified: cached full, publication11x2, focused NIDDB, fresh Full16 and governance; receipt records the post-build blank-line exception.

P32: [CCPU FLAGS declaration review](r0353-ccpu-flags-declaration.json).

P32: [CCPU debug event review](r0354-r0356-debug-events.json).

P32: [Debug event interface review](r0357-debug-event-interface.json).

P32: [Main decoding/reset declaration review](r0358-r0359-main-declarations.json).

P32: [Main provider and event declaration review](r0360-r0362-main-provider-review.json).

P32: [Debug boundary review](r0363-r0365-debug-boundary-review.json).

P32: [IRQ and jump-frame review](r0366-r0368-irq-frame-review.json).

P32: [HLT and event review](r0369-r0373-halt-review.json).

P32: [Instruction boundary review](r0374-r0382-boundary-review.json).

P32: [IRQ, resume and lifecycle tail review](r0383-r0398-tail-review.json).

P32: [Complete main source line check](batch-p32-main-line-check.json).

P32 verified: [delivery receipt](batch-p32-checkpoint.json),
[batch line references](batch-p32-line-check.json),
[binary comparison](batch-p32-binary-identity.json),
[register consistency](batch-p32-local-consistency.json).

P33: [Operand whole-file review and line/token checks](r0399-r0408-operand-review.json).
Only comments and a redundant diagnostic directive removed; P32 runtime baseline remains.

P34: [Original PROD check_D provider restoration](r0409-page-debug-provider.json),
[line verification](batch-p34-line-check.json), [delivery checkpoint](batch-p34-checkpoint.json),
[binary identity comparison](batch-p34-binary-identity.json).

P35: [Register diagnostic original restoration](r0410-register-diagnostic.json),
[Fixed selector minimization](r0411-fixed-selector-minimization.json),
[line checks](batch-p35-line-check.json), [checkpoint](batch-p35-checkpoint.json).
P35 focused witness: [segment cache state probe](r0411-segment-state-probe.c);
compiled only in the task build directory, not a new production/test target.

## P36 original TLB restoration delivered

[R0412-R0415 evidence](r0412-r0415-original-tlb-restoration.json) restores original xtrn2phy and removes coupled duplicate page-table preflight; mirror diff net-31. [Line checks](batch-p36-line-check.json) preserve excluded branches and694 original logical lines. [Checkpoint](batch-p36-checkpoint.json) records final-source cached build,11-image publication/hash at both locations,64 x87/143 CPU cases and Full16 passing. Progress:422/2720 reviewed;59/365 files;151 restored verified published,0 local,2 approved,269 pending decisions,2298 unreviewed. This receipt accompanies Git delivery.
