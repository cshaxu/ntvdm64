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

## P37 task-switch debug event audit

[R0416-R0417](r0416-r0417-task-debug-event.json) qualify two one-line residuals for owner decision; redundant comment removed, mirror diff net-1. No code-token or logical-line changes. [Receipt](batch-p37-checkpoint.json) records a fresh real CCPU T-bit/no-T-bit probe and reuse of P36 published package/Full. Progress424/2720 points,60/365 files;151 restored published,2 approved,271 pending decisions,2296 unreviewed.

## P38 exception source delivery

[R0418-R0429 review](r0418-r0429-exception-review.json) restores original diagnostic macro with minimal valid-stream guard, removes redundant header/comments, retains necessary five hook scopes. Mirror diff net-23. [Line proof](batch-p38-line-check.json); [receipt](batch-p38-checkpoint.json). Cached full build, publication11x2/hash,27 positive cases,101 expected original-negative failures, null/valid stream controls,64 x87/143 CPU and Full16 passed.436/2720 reviewed;61/365 files;152 restored delivered,0 local,2 approved,282 pending,2284 unreviewed. Receipt accompanies Git delivery.

## P39 transfer header restored

[R0430](r0430-transfer-header.json) removes unused TLB include and redundant line directives: mirror diff net-1;285 original matched lines correct. Same-path product-context paired objects differ only in COFF timestamp. [Receipt](batch-p39-checkpoint.json) reuses P38 package/Full without repeating behavioral tests.437/2720 reviewed;62/365 files;153 restored delivered,2 approved,282 pending decisions,2283 unreviewed.

P40: [original interface restoration](r0431-r0432-original-interface.json),
[line check](batch-p40-line-check.json), [delivery checkpoint](batch-p40-checkpoint.json).
Two original restorations verified: cached build, publication11x2/hash,64 x87/143 CPU, original/restored selector tests and Full16 passed.439/2720 reviewed;63/365 files;155 restored delivered;2 approved;282 pending decisions;2281 unreviewed. No native-fault production repro claimed.

P41: [CALL file review](r0433-r0437-call-review.json),
[line proof](batch-p41-line-check.json), [checkpoint](batch-p41-checkpoint.json).
P41 cached build, publication11x2/hash,36 stack cases,2 expected original CALL failures,64 x87/143 CPU and Full16 passed.444/2720 reviewed;64/365 files;159 restored delivered (four reconciled P19 removals),2 approved,283 pending,2276 unreviewed.

P42: [original build metadata](r0438-original-build-metadata.json) restored;
mirror diff net-2. All supported generated graphs remain byte-identical;
CPU30 not selected. [Receipt](batch-p42-checkpoint.json) reuses P41 publication
and runtime.445/2720 reviewed;65/365 files;160 restored,2 approved,283 pending,2275 unreviewed.

P43: [SAS call-chain and controls](r0439-r0453-investigation.json),
[line proof](batch-p43-line-check.json), [checkpoint](batch-p43-checkpoint.json).
Whole-file diff97->86;1486 original logical lines.460 points source-reviewed;
162 restored delivered,0 local,2 approved,296 pending,2260 unreviewed.
66/365 files complete; P43 cached build,publication11x2/hash,mapping/90 BIOS accesses,64 x87/143 CPU and Full16 passed.

P44: [original register declarations](r0454-r0466-original-declarations.json),
[line proof](batch-p44-line-check.json), [checkpoint](batch-p44-checkpoint.json).
13 original restorations verified;mirror diff32->0.473/2720 reviewed;67/365 files
complete;175 restored delivered,0 local,2 approved,296 pending,2247 unreviewed.
P44 cached build,publication11x2/hash,mapping/90 BIOS accesses,64 x87/143 CPU and Full16 passed.

P45: [FPU review](r0467-r0472-fpu-review.json),[line proof](batch-p45-line-check.json),
[checkpoint](batch-p45-checkpoint.json).5restorations delivered,1minimal residual;
mirror diff16->3.479/2720 reviewed;68/365files complete;180restored delivered,
0local,2approved,297pending,2241unreviewed. P45 cached build,publication11x2/hash,FIST64/mapping/BIOS/x87/CPU and Full16 passed.

## R0473 IRET

Historical-contract audit and minimal predicate: [review](r0473-iret-review.json),
[line proof](batch-p46-line-check.json), [delivery](batch-p46-checkpoint.json).
480/2720 reviewed;180 restored delivered,0 local restorations,2 approved,
298 pending,2240 unreviewed. 69/365 files source-complete;296 remain.
P46 mirror diff5->2 (-3);cached build,publication11x2/hash,focused and Full16 passed.

## R0474-R0475 localfm

[Review](r0474-r0475-localfm-review.json),[lines](batch-p47-line-check.json),
[delivery](batch-p47-checkpoint.json). Mirror diff10->3 (-7).
482/2720 reviewed;181 restored delivered,0local,2approved,299pending,2238unreviewed.
70/365 files complete;295 remain.P47 cached build,publication11x2/hash,focused and Full16 passed.

## R0476-R0478 ntthread

[Review](r0476-r0478-ntthread-review.json),[lines](batch-p48-line-check.json),
[delivery](batch-p48-checkpoint.json). Mirror diff25->15 (-10).
485/2720 reviewed;183restored delivered,0local,2approved,300pending,2235unreviewed.
71/365 files complete;294 remain.P48 cached build,publication11x2/hash,focused and Full16 passed.

R0479: [header review](r0479-ntthread-header-review.json);mirror diff2->1,comment-only.
486/2720 reviewed;183 restored delivered,0local restorations,2approved,301pending,2234unreviewed.
72/365 files complete;293remain.P48 remains published;local comment cleanup awaits next batch.

R0480: [original POPFD source](r0480-popf-review.json) restored byte-exact.
[Batch line proof](batch-p49-line-check.json),[P49 checkpoint](batch-p49-checkpoint.json).
487/2720 reviewed;183restored delivered,1local,2approved,301pending,2233unreviewed.
73/365 files complete;292remain.P49 net mirror diff-3;delivery pending.

P49 [actual compiler controls](batch-p49-compile-controls.json) separate link-retained
code/data/relocations from compiler linker-remove checksum metadata.

P49 delivered:cached build,11x2 publication/hash,focused and Full16 passed.
487/2720 reviewed;184restored delivered,0local,2approved,301pending,2233unreviewed.
73/365 files complete;292remain. Next R0481.

R0481:[RET review](r0481-ret-review.json),[lines](batch-p50-line-check.json),[checkpoint](batch-p50-checkpoint.json).
Mirror diff5->2,final delivery pending.488/2720 reviewed;184restored delivered,0local restorations,2approved,302pending,2232unreviewed.74/365files complete;291remain.

P50 delivered:cached build,11x2 publication/hash,36stack/2expected original failures,focused and Full16 passed.
488/2720 reviewed;184restored delivered,0local,2approved,302pending,2232unreviewed.
74/365files complete;291remain.Next R0482.

R0482:[SasVector review](r0482-sascdef-review.json),[lines](batch-p51-line-check.json),[checkpoint](batch-p51-checkpoint.json).
Mirror diff6->0;delivery pending.489/2720 reviewed;184restored delivered,1local,2approved,302pending,2231unreviewed.75/365files complete;290remain.

P51 delivered:cached build,11x2 publication/hash,C-VID/vector,focused and Full16 passed.
489/2720 reviewed;185restored delivered,0local,2approved,302pending,2231unreviewed.
75/365files complete;290remain.Next R0483.

R0483: [selected CCPU manifest review](r0483-sources-review.json), +0/-1 unchanged.
Actual original selection fails on the excluded alternate stub; no product edit.
490/2720 reviewed;76/365 files;185 restored,2 approved,303 pending,2230 unreviewed.
P51 remains the build/publication/runtime baseline.

P52: [zfrsrvd whole-file review](r0484-r0487-zfrsrvd-review.json), four points restored.
Nonexcluded mirror diff +6/-2 to0,net-8;[logical lines](batch-p52-line-check.json) checked.
494/2720 reviewed;77/365 files;185 delivered,4 local,2 approved,303 pending,2226 unreviewed.
[Delivery checkpoint](batch-p52-checkpoint.json) remains pending until final-source gates pass.

P52 delivered:cached build,11x2 publication/hash,focused and Full16 passed.
494/2720 reviewed;189 restored delivered,0 local,2 approved,303 pending,2226 unreviewed.
77/365 files complete;288 remain.Next R0488.

P53: [original C-VID metadata](r0488-original-cvidc-metadata.json) restored,mirror diff net-2.
Both generated architecture graphs identical;P52 package/runtime reused,no new runtime claim.
[Receipt](batch-p53-checkpoint.json).495/2720 reviewed;78/365 files;190 restored,2 approved,303 pending,2225 unreviewed.

P54 local:ev_glue.c six points audited;mirror diff54->25(net-29).
501 reviewed;190 delivered restorations,2 local,2 approved,307 pending,2219 unreviewed.
79/365 files disposed;286 remain.Final-source delivery pending.

P54 delivered:cached build,11x2 publication/hash,focused and Full16 passed.
501/2720 reviewed;192 restored delivered,0 local,2 approved,307 pending,2219 unreviewed.
79/365 files complete;286 remain.Next R0495.

P55 local R0495:exact original header restored,mirror diff net-30;two product consumer object controls equal.
502 reviewed;192 delivered,1 local,2 approved,307 pending,2218 unreviewed;80/365 files.
Nonbinding batch accumulates;no full build or new publication claimed.

P55 R0496-R0499:four paired boundaries pending;mirror diff net-3.
506 reviewed;192 delivered,1 local,2 approved,311 pending,2214 unreviewed;81/365 files.
Two-header batch net-33;next R0500.

P55 R0500 original restored;mirror diff net-2;10 object sections equal.
507 reviewed;192 delivered,2 local,2 approved,311 pending,2213 unreviewed;82/365 files.
Three-file nonbinding batch net-35;next R0501.

P55 R0501 restored;mirror diff net-2;70 compile controls,4722 sections equal.
508 reviewed;192 delivered,3 local,2 approved,311 pending,2212 unreviewed;83/365 files.
Four-file nonbinding batch net-37;next R0502.

R0502 qualified residual:current16/16,original shift12/16 mismatches;one-line diff unchanged.
509 reviewed;192 delivered,3 local,2 approved,312 pending,2211 unreviewed.
83/365 files complete;sevid0001/70 points reviewed,next R0503.

P55 R0503-R0506: four individual original-restoration controls prove address/latch or nested-local failure; no product changes. See [review](r0503-r0506-word-read-review.json). Reviewed513/2720;316 pending;83/365 files complete, sevid0005/70.

P55 R0507-R0508: second-word and composition controls;R0508 minimized+2/-2 to+1/-1. [Evidence](r0507-r0508-dword-review.json). Reviewed515/2720;318pending;83/365files complete;sevid0007/70. Batch net-39;full-file delivery pending.

P55 R0509 original internal call restored;stride remains pending. Mirror+3/-3 -> +1/-1,net-4;128/128 focused cases pass. [Evidence](r0509-string-entry-review.json). Reviewed516/2720;319pending;sevid0008/70;batch net-43;full-file delivery pending.

P55 R0510-R0515: five address residuals qualified;R0513 original completion call restored,net-4. [Evidence](r0510-r0515-string-loop-review.json). Reviewed522/2720;324pending;4local restorations;sevid00014/70;batch net-47;full delivery pending.

P55 R0516: unchained dword original offset fails32cases;current64/64. [Evidence](r0516-unchained-dword-review.json). Reviewed523/2720;325pending;sevid00015/70;batch net-47 unchanged.

P55 R0517 mode1 address residual qualified;[evidence](r0517-mode1-byte-address-review.json). Reviewed524/2720;326pending;sevid00016/70. R0518+ field sources still require investigation, not pending-owner admission.

P55 R0518-R0521: original updater transition proves offset regressions;current256/256,full CHAIN2 comparison coverage explicitly not claimed. [Evidence](r0518-r0521-mode1-field-review.json). Reviewed528/2720;330pending;sevid00020/70;batch net-47.

P55 R0522-R0523 qualified with comparison-field-independent word controls. [Evidence](r0522-r0523-mode1-word-address-review.json). Reviewed530/2720;332pending;sevid00022/70;batch net-47 unchanged.

P55 R0524-R0533: ten single-line original controls fail96/152of256;current256/256. [Evidence](r0524-r0533-mode1-word-fields-review.json). Reviewed540/2720;342pending;sevid00032/70;batch net-47 unchanged.

P55 R0534 real nested dword control: original internal call loses local activation;current16/16. [Evidence](r0534-mode1-nested-local-review.json). Reviewed541/2720;343pending;sevid00033/70;batch net-47.

P55 R0535-R0544: eight field residuals+two dword residuals qualified;R0544 minimized net-2 with final16/16. [Evidence](r0535-r0544-mode1-word-dword-review.json). Reviewed551/2720;353pending;sevid00043/70;batch net-49.

P55 R0545 original internal call restored;stride remains pending;net-4,current candidate128/128. [Evidence](r0545-mode1-string-entry-review.json). Reviewed552/2720;354pending;sevid00044/70;batch net-53.

P55 R0546-R0559: thirteen bounded residuals;R0553 original completion restored,net-4,final512/512. [Evidence](r0546-r0559-mode1-string-loop-review.json). Reviewed566/2720;367pending;5local restorations;sevid00058/70;batch net-57.

P55 R0560-R0561 CHAIN4 byte offset evidence uses actual scalar updater;current128/128. [Evidence](r0560-r0561-chain4-byte-review.json). Reviewed568/2720;369pending;sevid00060/70;batch net-57.

P55 R0562/R0563: necessary but incomplete word mask adaptation;current320/512fail. [Root cause/proposal](r0562-r0563-chain4-word-gap-review.json). Net-increase repair not implemented;not a pass. Reviewed570/2720;371pending;sevid00062/70;batch net-57.

P55 sevid00070/70 dispositions complete. R0568-R0571 restored ineffective offsets;net-8;UNCHAINED zero-output baseline defect explicit,not passed. [Review](r0564-r0571-final-string-review.json),[whole-file lines/diff](p55-sevid000-file-check.json). Reviewed578/2720;375pending;9local restorations;84/365files complete;batch net-65;full delivery pending.

P55 delivery:cached build,11x2 publication/hash,focused and Full16 passed.
578/2720 reviewed;201 restored delivered,0 local,2 approved,375 pending;2142 unreviewed.
84/365 files disposition complete;281 remain.Next R0572.Known CVIDC baseline failures remain,not fixed.

P56 sevid00133/33 dispositions:8 restorations local,25 residual decisions;mirror diff74->50(-24).
611 reviewed;201 restored delivered,8 local,2 approved,400 pending,2109 unreviewed;85/365 files.

P56 delivered:cached build,11x2 publication/hash,focused and Full16 passed.
611 reviewed;209 restored delivered,0 local,2 approved,400 pending,2109 unreviewed;85/365 files.
Known backward UNCHAINED width defect remains.Next R0605.

P57: sevid01946 original restorations; mirror diff net-140. Cached full build,
11x2 publication/hash, focused and Full16 passed. 687 reviewed;255 restored
delivered,2 approved,430 pending,2033 unreviewed;90/365 files.
See [receipt](batch-p57-checkpoint.json) and [file review](sevid019-file-review.json).

P58: sevid02016/16 restored byte-exact,mirror diff net-44. Cached full build,
11x2 publication/hash, focused and Full16 passed.703 reviewed;271 restored
delivered,2 approved,430 pending,2017 unreviewed;91/365 files.
See [receipt](batch-p58-checkpoint.json) and [file review](sevid020-file-review.json).

P59: emm_mngr3/3;duplicate declaration removed,diagnostic coordinates verified,
CCPU selection minimized;mirror diff net-6. Cached full,11x2 publication/hash,
focused,EMS Mapping+CallReturn and Full16 passed.706 reviewed;273 delivered,
2 approved,431 pending,2014 unreviewed;92/365 files.
See [receipt](batch-p59-checkpoint.json) and [file review](emm-mngr-file-review.json).

P60: ckmalloc/cpu_vid/egacpu5/5 original selected declarations restored;
mirror diff net-23;all exclusion guards retained. Cached full,11x2 publication/
hash,focused,EMS2 and Full16 passed.711 reviewed;278 delivered,2 approved,
431 pending,2009 unreviewed;95/365 files.
See [receipt](batch-p60-checkpoint.json),[ckmalloc](ckmalloc-h-file-review.json),
[cpu_vid](cpu_vid-h-file-review.json),[egacpu](egacpu-h-file-review.json).

P61: error/gfx_upd2/2 original subset/declarations restored;mirror diff net-16.
Cached full,11x2 publication/hash,focused,EMS2 and Full16 passed.713 reviewed;
280 delivered,2 approved,431 pending,2007 unreviewed;97/365 files.
See [receipt](batch-p61-checkpoint.json),[error](error-h-file-review.json),
[gfx_upd](gfx_upd-h-file-review.json).

P62: gmi original declarations restored;mirror diff net-14.
Cached full,11x2 publication/hash,focused,EMS2 and Full16 passed.714 reviewed;
281 delivered,2 approved,431 pending,2006 unreviewed;98/365 files.
See [receipt](batch-p62-checkpoint.json),[review](gmi-h-file-review.json).

P63: gvi residual pending owner;host original declaration restored;mirror diff net-7.
Cached full,11x2 publication/hash,focused,EMS2 and Full16 passed.716 reviewed;
282 delivered,2 approved,432 pending,2004 unreviewed;100/365 files.
See [receipt](batch-p63-checkpoint.json),[gvi](gvi-h-file-review.json),
[host](host-h-file-review.json).

P64 audit in progress: [ICA](ica-h-file-review.json) and [mouse header](mouse-io-h-file-review.json) complete.
Four residual points await owner;two files have comment-only diff reductions(net-6),
accumulating under format-only cadence. No new build/publication claim;next R0714.

SAS header investigation: [actual product compiler controls](sas-h-compiler-controls.json). R0714/R0715 remain outside completed counts pending whole-file disposition; these are not full-build or runtime results.

P64 [SAS whole-file review](sas-h-file-review.json):two original declarations restored,mirror diff-8;batch total-14. Six actual product translation units compile with identical875 code/data sections and resolved relocations.722 points reviewed;2 local restorations await delivery;next R0716.

P64 delivered:two SAS original restorations;ICA/mouse residuals remain owner decisions.
Mirror diff net-14;cached full,11x2 publication/hash,focused,EMS2 and Full16 passed.
722 reviewed;284 delivered,2 approved,436 pending,1998 unreviewed;103/365 files.
See [delivery receipt](batch-p64-checkpoint.json) and [logical lines](batch-p64-line-check.json).

P65 audits: [time declaration](timeval-h-file-review.json) has real original-restoration C2371 failure and awaits owner;[VGA header](vgaports-h-file-review.json) removes7 obsolete line directives. Old489 mismatch claim was form-feed counting error;real793 preprocessed lines/coordinates match original before and after. Diagnostic-only batch accumulating;724 reviewed,284 delivered,1 local,2 approved,437 pending,1996 unreviewed;105/365 files.

P65 [R0718 keyboard boundary](r0718-keyboard-boundary-review.json):comment diff-1;residual6 lines pending owner;current device7/return10 extracted cases pass with explicit limits.725 reviewed;105 complete files unchanged;keyba1/32 in progress.

P65 [R0719 XMS include](r0719-xms-include-review.json):original include set restored;mirror diff-1;actual67 code/data sections and resolved relocations identical.726 reviewed;284 delivered,2 local,2 approved,438 pending,1994 unreviewed;105 complete files;keyba2/32 ongoing. Batch diff-9,not published.

P65 [R0720 slot-state review](r0720-keyboard-slot-state-review.json):original early-EOI overwrite defect evidenced;existing state alternatives checked;two declarations pending owner and seven comment lines removed.727 reviewed;439 pending;1993 unreviewed;keyba3/32;batch diff-16 accumulating.

P65 [R0721 immediate-origin review](r0721-immediate-origin-review.json):original RESEND transition invalidates empty-slot-zero assumption;positive/negative extraction controls0/1.728 reviewed;440 pending;1992 unreviewed;keyba4/32;batch mirror diff remains-16.

P65 [R0722 enqueue-origin review](r0722-enqueue-origin-review.json):packet markers cannot identify raw events;single-hook restoration fails make/ignored-release control.729 reviewed;441 pending;1991 unreviewed;keyba5/32;batch mirror diff remains-16.

P65 [R0723 dequeue-origin review](r0723-dequeue-origin-review.json):independent transfer/clear deletion controls fail;730 reviewed;442 pending;1990 unreviewed;keyba6/32;mirror diff remains-16.

P65 [R0724 ring-clear review](r0724-ring-clear-review.json):clear plus original RESEND exposes stale identity without hook;731 reviewed;443 pending;1989 unreviewed;keyba7/32;mirror diff remains-16.

P65 [R0725 pending-snapshot review](r0725-pending-snapshot-review.json):original count/reset cannot replace identity snapshot;comment removed,diff-1.732 reviewed;444 pending;1988 unreviewed;keyba8/32;batch mirror diff-17.

P65 [R0726 reset minimization](r0726-reset-minimization-review.json):redundant clear removed with unused helper/declaration;device/return/carrier pass.733 reviewed;445 pending;1987 unreviewed;keyba9/32;mirror diff-18;substantive delivery waits for whole file.

P65 [R0727 reset flags restored](r0727-reset-flags-restoration.json):two redundant assignments deleted;device7/return10/carrier7 pass.778 reviewed;3 local restorations;445 pending;1986 unreviewed;keyba10/32;mirror diff-20.

P65 [T483-R0728 review](r0728-reset-irq-review.json):Remove1 explanatory comment;retain1 cancellation call pending owner. 735 reviewed;446 pending;1985 unreviewed;keyba11/32;mirror diff-21.

P65 [T483-R0729 review](r0729-held-make-review.json):No source change;three added lines remain pending owner. 736 reviewed;447 pending;1984 unreviewed;keyba12/32;mirror diff-21.

P65 [T483-R0730 review](r0730-held-release-review.json):No source change;three added lines remain pending owner. 737 reviewed;448 pending;1983 unreviewed;keyba13/32;mirror diff-21.

P65 [T483-R0731 review](r0731-immediate-slot-restoration.json):Delete all3 added assignments;check invalid prepublication slot does not select old input. 738 reviewed;448 pending;1982 unreviewed;keyba14/32;mirror diff-24.

P65 [T483-R0732 review](r0732-held-replay-review.json):No source change;three added lines pending owner. 739 reviewed;449 pending;1981 unreviewed;keyba15/32;mirror diff-24.

P65 [T483-R0733 review](r0733-replay-origin-end-review.json):No source change;three added lines pending owner. 740 reviewed;450 pending;1980 unreviewed;keyba16/32;mirror diff-24.

P65 [T483-R0778 review](r0778-early-eoi-irq-gate-review.json):No mirror change;extend existing actual controller/PIC test to retry continue_output between early EOI and port60 read and assert no new pending IRQ. 741 reviewed;451 pending;1979 unreviewed;keyba17/32;mirror diff-24.

P65 [T483-R0735 review](r0735-cancel-tail-minimization.json):Remove19 mirror lines;fold predicates into R0737/R0738 existing changed lines;remaining cancellation semantic stays pending owner. 742 reviewed;452 pending;1978 unreviewed;keyba18/32;mirror diff-43.

P65 [T483-R0736 review](r0736-unconditional-clear-review.json):No product edit;one deleted original line pending owner. 743 reviewed;453 pending;1977 unreviewed;keyba19/32;mirror diff-43.

P65 [T483-R0737 review](r0737-eoi-observation-review.json):No further edit;R0735 coupled assignment reviewed in order here. 744 reviewed;454 pending;1976 unreviewed;keyba20/32;mirror diff-43.

P65 [T483-R0738 review](r0738-eoi-retirement-minimization.json):Delete1 added origin clear;replace raw-inactive-storage test assertion with unchanged no-pending-input assertion. 745 reviewed;455 pending;1975 unreviewed;keyba21/32;mirror diff-44.

P65 [T483-R0739 review](r0739-output-publication-minimization.json):Delete translated=0 from do_q_int;remaining3 additions pending owner. 746 reviewed;456 pending;1974 unreviewed;keyba22/32;mirror diff-45.

P65 [T483-R0740 review](r0740-input-port-reply-review.json):No source change;one assignment pending owner. 747 reviewed;457 pending;1973 unreviewed;keyba23/32;mirror diff-45.

P65 [T483-R0741 review](r0741-ccpu-reset-boundary-review.json):Remove11 added diagnostic/comment lines;remaining call plus line anchor pending owner. 748 reviewed;458 pending;1972 unreviewed;keyba24/32;mirror diff-56.

P65 [T483-R0742 review](r0742-original-reset-body-review.json):No source change;correct historical deletion scope from three to two remaining original statements because #else is already restored. 749 reviewed;459 pending;1971 unreviewed;keyba25/32;mirror diff-56.

P65 [T483-R0743 review](r0743-controller-reply-flags-restoration.json):Delete two added state writes;mirror diff net-2. 750 reviewed;459 pending;1970 unreviewed;keyba26/32;mirror diff-58.

P65 [T483-R0744 review](r0744-controller-reply-publication-minimization.json):Mirror diff +5 to +2,net-3;partial restoration,residual remains pending owner. 751 reviewed;460 pending;1969 unreviewed;keyba27/32;mirror diff-61.

P65 [T483-R0745 review](r0745-split-prefix-origin-review.json):No source change;remaining +3/-0 pending owner. 752 reviewed;461 pending;1968 unreviewed;keyba28/32;mirror diff-61.

P65 [T483-R0746 review](r0746-unused-pending-origin-restoration.json):Restore original continue_output branch;mirror diff net-4. Reduce autonomous state and argument;retain original pending_8042 variable/logic. 753 reviewed;461 pending;1967 unreviewed;keyba29/32;mirror diff-65.

P65 [T483-R0747 review](r0747-resume-handshake-review.json):No source change;remaining +2/-0 pending owner. 754 reviewed;462 pending;1966 unreviewed;keyba30/32;mirror diff-65.

P65 [T483-R0748 review](r0748-port60-retirement-minimization.json):Mirror +9 to +7,net-2;remaining semantic diff stays pending owner. 755 reviewed;463 pending;1965 unreviewed;keyba31/32;mirror diff-67.

P65 [T483-R0749 review](r0749-init-flags-restoration.json):Original restored;mirror diff net-2. 756 reviewed;463 pending;1964 unreviewed;keyba32/32;mirror diff-69.

P65 final-code gates passed:seven restorations;mirror diff net-68 (keyba-61,vgaports-7).
291 restored/delivered,2 approved,463 pending;756/2720 reviewed,1964 remain;106/365 files.
[Receipt](batch-p65-checkpoint.json);[line proof](batch-p65-line-check.json).

P66 [R0750](r0750-keyboard-table-address-review.json):comment-only mirror reduction7;remaining+2/-1 pending owner;keybd_io1/3. 757/2720 reviewed,1963 remain.

P66 [T483-R0751](r0751-keyboard-sas-address-review.json):No source change;remaining+1/-1 pending owner. 758/2720 reviewed;1962 remain.

P66 [T483-R0752](r0752-stream-buffer-pointers-review.json):Format-only consolidation+4 to+2;net mirror diff-2;expressions and evaluation order unchanged. 759/2720 reviewed;1961 remain.

P66 [keybd_io file review](keybd-io-c-file-review.json):3/3 complete;raw mirror diff579->570 (-9);tokens equal;selected original lines aligned;format-only local batch. Files107/365;759/2720 points reviewed;1961 remain.

P66 [T483-R0753](r0753-mouse-route-declaration-review.json):No source change at R0753; one-line explicit boundary dependency retained pending owner decision. 760/2720 reviewed;1960 remain.

P66 [R0754](r0754-mouse-range-original-review.json): original range ABI restored locally with paired provider; mirror diff-17. 761/2720 reviewed;1959 remain. Final batch verification pending.

P66 [T483-R0755](r0755-mouse-current-mode-review.json):No source change; +1/-1 retained pending owner with actual scene and before/fixed evidence. 762/2720 reviewed;1958 remain.

P66 [T483-R0756](r0756-mouse-cache-assignment-review.json):Removed one added explanation line contents; physical blank retained. Mirror diff +1/-1 becomes +0/-1, net-1. 763/2720 reviewed;1957 remain.

P66 [T483-R0757](r0757-video-mouse-instance-review.json):Removed added comment; +2/-0 becomes +1/-0, mirror delta-1; original selector call remains pending owner. 764/2720 reviewed;1956 remain.

P66 [T483-R0758](r0758-mouse-position-owner-review.json):Deleted two added comment lines; +3/-0 becomes +1/-0, net-2. 765/2720 reviewed;1955 remain.

P66 [R0759](r0759-mouse-draw-comment-review.json): explanation removed;mirror diff-1. 766/2720 reviewed;1954 remain. Not delivered.

P66 [T483-R0760](r0760-mouse-draw-selection-review.json):Removed redundant nested guard at R0760; paired closing restoration belongs to next R0761. This point +3/-0 -> +2/-0, net-1. 767/2720 reviewed;1953 remain.

P66 [R0761](r0761-mouse-draw-ending-review.json): original ending restored with R0760;mirror diff-3. 768/2720 reviewed;1952 remain. Not delivered.

P66 [T483-R0762](r0762-mouse-callback-stack-review.json):Removed3 comment lines, added one original-coordinate #line; +17/-0 becomes +15/-0, net-2. 769/2720 reviewed;1951 remain.

P66 [T483-R0763](r0763-mouse-callback-local-mask-review.json):Retained one local declaration; relocated existing line directive, no mirror size growth. 770/2720 reviewed;1950 remain.

P66 [T483-R0764](r0764-mouse-callback-restore-review.json):No statement change; relocated existing #line2692 to #line2683 immediately after ten restores. Mirror size unchanged. 771/2720 reviewed;1949 remain.

P66 [T483-R0765](r0765-mouse-return-draw-mask-review.json):No source change; +1/-1 retained. 772/2720 reviewed;1948 remain.

P66 [T483-R0766](r0766-mouse-show-comment-review.json):Removed two added comments; original absence restored, mirror diff-2. 773/2720 reviewed;1947 remain. Not delivered.

P66 [T483-R0767](r0767-mouse-show-selection-review.json):Removed nested !MONITOR; this point mirror diff-1. Paired ending restoration R0768 follows. 774/2720 reviewed;1946 remain.

P66 [T483-R0768](r0768-mouse-show-ending-review.json):Restored #endif /* NTVDM */; mirror diff-3. 775/2720 reviewed;1945 remain. Not delivered.

P66 [T483-R0769](r0769-mouse-hide-restore-review.json):Removed added hide-count explanation. Mirror diff-1. 776/2720 reviewed;1944 remain. Not delivered.

P66 [T483-R0770](r0770-mouse-hide-selection-review.json):Removed redundant !MONITOR; +3/-0 becomes +2/-0, net-1. Paired extra ending removed at R0771. 777/2720 reviewed;1943 remain.

P66 [T483-R0771](r0771-mouse-hide-restore-review.json):Removed redundant nested condition ending; original outer ending retained. Mirror diff-1. 778/2720 reviewed;1942 remain. Not delivered.

P66 [T483-R0772](r0772-mouse-position-common-path-review.json):Restored absence of opening route branch and its two comments; paired R0773 replaces unconditional return with route-dependent return. Mirror diff-3. 779/2720 reviewed;1941 remain. Not delivered.

P66 [T483-R0773](r0773-mouse-position-single-call-review.json):Removed closing branch, two explanation lines and duplicate host call; replaced original return with route-dependent return. Point delta-3; paired group delta-6. 780/2720 reviewed;1940 remain.

P66 [T483-R0774](r0774-mouse-shape-restore-review.json):Removed added shape explanation. 781/2720 reviewed;1939 remain. Not delivered.

P66 [T483-R0775](r0775-mouse-shape-selection-review.json):Removed redundant guard: +3/-0 -> +2/-0, net-1. Corrected stale #line2936 to3311 without adding directives. 782/2720 reviewed;1938 remain.

P66 [T483-R0776](r0776-mouse-shape-provider-review.json):No source change at this point; +3/-5 retained. 783/2720 reviewed;1937 remain.

P66 [T483-R0777](r0777-mouse-shape-restore-review.json):Restored original #endif /* NTVDM */ after redundant guard removal; historical anchor includes empty line. 784/2720 reviewed;1936 remain. Not delivered.

P66 [T483-R0778](r0778-mouse-checksum-index-review.json):No source change at index; +1/-0 retained. 785/2720 reviewed;1935 remain.

P66 [T483-R0779](r0779-mouse-checksum-sum-review.json):Removed2 added explanation lines; +6/-0 -> +4/-0, net-2. Repositioned existing #line3723 to3722 before original Save Cookie comment. 786/2720 reviewed;1934 remain.

P66 [T483-R0780](r0780-mouse-geometry-restore-review.json):Removed duplicate geometry explanation. Mirror diff-1. 787/2720 reviewed;1933 remain. Not delivered.

P66 [T483-R0781](r0781-mouse-geometry-selection-review.json):Added content +6/-0 -> +3/-0, net-3; corrected/repositioned stale #line4764 to5228 after runtime guard. 788/2720 reviewed;1932 remain.

P66 [T483-R0782](r0782-mouse-geometry-host-position-review.json):No change at R0782. 789/2720 reviewed;1931 remain.

P66 [T483-R0783](r0783-mouse-geometry-restore-review.json):Removed redundant MONITOR closing condition; original ending retained. Mirror diff-1. 790/2720 reviewed;1930 remain. Not delivered.

P66 [T483-R0784](r0784-mouse-painter-restore-review.json):Removed duplicate painter explanation. 791/2720 reviewed;1929 remain. Not delivered.

P66 [T483-R0785](r0785-mouse-painter-selection-review.json):Removed redundant !MONITOR, +3/-0 -> +2/-0, net-1. Also removed superseded #line4791 immediately preceding #line5251 (line-only delta-1 counted separately). 792/2720 reviewed;1928 remain.

P66 [T483-R0786](r0786-mouse-painter-route-review.json):Removed inner #ifdef NTVDM/#endif; +3/-0 -> +1/-0, net-2. 793/2720 reviewed;1927 remain.

P66 [T483-R0787](r0787-mouse-painter-host-provider-review.json):No source change at R0787. 794/2720 reviewed;1926 remain.

P66 [T483-R0788](r0788-mouse-painter-restore-review.json):Restored original #endif /* !NTVDM */; historical count included empty line. 795/2720 reviewed;1925 remain. Not delivered.

P66 [T483-R0789](r0789-mouse-erase-comment-review.json):Removed two explanation lines; original absence restored, mirror diff-2. 796/2720 reviewed;1924 remain. Not delivered.

P66 [T483-R0790](r0790-mouse-saved-owner-review.json):Restored original surrounding structure; full pinned-original raw comparison changes +491/-992 to +492/-980, net-11 attributable solely to this edit. Per-point historical +14/-28 becomes attributable +15/-16; whole-file final relocation will reconcile anchors. 797/2720 reviewed;1923 remain.

P66 [T483-R0791](r0791-mouse-saved-dispatch-review.json):Restored exact original EGA call and #line5385; mirror diff-1. 798/2720 reviewed;1922 remain.

P66 [T483-R0792](r0792-mouse-saved-dispatch-review.json):Restored exact original VGA call and #line5412; mirror diff-1. 799/2720 reviewed;1921 remain.

P66 [T483-R0793](r0793-mouse-saved-dispatch-review.json):No change; obsolete conditional ending remains deleted, +0/-2. 800/2720 reviewed;1920 remain.

P66 [T483-R0794](r0794-mouse-erase-tail-review.json):Restored original HERC/NTVDM endings and CGA call with original #line5423/5424/5429; removed redundant MONITOR close. Tail net-4; paired opening R0790 net-1 separately synchronized. Whole-file measured +492/-978 -> +490/-975, net-5. 801/2720 reviewed;1919 remain.

P66 [T483-R0795](r0795-mouse-route-edge-review.json):Deleted3 comment lines plus repeated selector; historical +11/-0 becomes +7/-0, mirror diff-4. 802/2720 reviewed;1918 remain.

P66 [T483-R0796](r0796-text-save-provenance-review.json):No change at R0796; +1/-1 retained. 803/2720 reviewed;1917 remain.

P66 [T483-R0797](r0797-text-restore-address-review.json):Removed explanation contents; +2/-2 -> +1/-2, net-1. 804/2720 reviewed;1916 remain.

P66 [T483-R0798](r0798-ega-scale-pair-review.json):No mirror source change at this point. Corrected focused fixture to model mode change at SAS read, not dirty marking. 805/2720 reviewed;1915 remain.

P66 [T483-R0799](r0799-ega-scale-pair-review.json):No mirror source change at this point. Corrected focused fixture to model mode change at SAS read, not dirty marking. 806/2720 reviewed;1914 remain.

P66 [T483-R0800](r0800-ega-background-geometry-review.json):No source change; necessary residual pending owner. 807/2720 reviewed;1913 remain.

P66 [T483-R0801](r0801-ega-background-geometry-review.json):No source change; necessary residual pending owner. 808/2720 reviewed;1912 remain.

P66 [T483-R0802](r0802-ega-background-geometry-review.json):Removed two comment lines; residual +6/-0 becomes +4/-0; mirror diff net-2. 809/2720 reviewed;1911 remain.

P66 [T483-R0803](r0803-vga-saved-kind-review.json):No source change; necessary residual pending owner. 810/2720 reviewed;1910 remain.

P66 [T483-R0804](r0804-cga-saved-kind-review.json):No source change; necessary residual pending owner. 811/2720 reviewed;1909 remain.

P66 [T483-R0805](r0805-callback-entry-review.json):Mirror diff net-1; removed redundant explanation where applicable; no behavior change. 812/2720 reviewed;1908 remain.

P66 [T483-R0806](r0806-callback-entry-review.json):Mirror diff net-1; removed redundant explanation where applicable; no behavior change. 813/2720 reviewed;1907 remain.

P66 [T483-R0807](r0807-callback-entry-review.json):Mirror diff net-5; removed redundant explanation where applicable; no behavior change. 814/2720 reviewed;1906 remain.

P66 [T483-R0808](r0808-callback-entry-review.json):Mirror diff net0; removed redundant explanation where applicable; no behavior change. 815/2720 reviewed;1905 remain.

P66 [T483-R0809](r0809-callback-original-save-review.json):Restored exact original parameter/setup tail, including saved field writes and original comments. Entire point restored locally. 816/2720 reviewed;1904 remain. Not delivered.

P66 [T483-R0810](r0810-callback-original-save-review.json):Restored exact original parameter/setup tail, including saved field writes and original comments. Entire point restored locally. 817/2720 reviewed;1903 remain. Not delivered.

P66 [T483-R0811](r0811-callback-original-save-review.json):Restored exact original parameter/setup tail, including saved field writes and original comments. Entire point restored locally. 818/2720 reviewed;1902 remain. Not delivered.

P66 [T483-R0812](r0812-callback-original-save-review.json):Restored exact original parameter/setup tail, including saved field writes and original comments. Entire point restored locally. 819/2720 reviewed;1901 remain. Not delivered.

P66 [T483-R0813](r0813-callback-original-save-review.json):Restored exact original parameter/setup tail, including saved field writes and original comments. Entire point restored locally. 820/2720 reviewed;1900 remain. Not delivered.

P66 [T483-R0814](r0814-callback-original-save-review.json):Restored exact original parameter/setup tail, including saved field writes and original comments. Entire point restored locally. 821/2720 reviewed;1899 remain. Not delivered.

P66 [T483-R0815](r0815-callback-original-save-review.json):Restored exact original parameter/setup tail, including saved field writes and original comments. Entire point restored locally. 822/2720 reviewed;1898 remain. Not delivered.

P66 [mouse file review](mouse-io-c-file-review.json):63/63;whole-file mirror net-127 across3 files, point aggregation reconciliation+2;108/365 files audited. Build/delivery pending.

P66 [delivery evidence](batch-p66-checkpoint.json):cached full,publication11x2,mouse controls,Full and governance passed;22 restorations verified;mirror diff-127;822/2720 points and108/365 files audited;Receipt accompanies P66 commit.

P67 ios.c3/3: [R0816](r0816-ios-string-selection-review.json), [R0817](r0817-ios-string-selection-review.json), [R0818](r0818-ios-string-selection-review.json). Format-only net-3 plus original CRLF restoration;825/2720 points,109/365 files;batch pending.

P67 [R0819](r0819-main-early-binding-restoration.json):restore original include/startup pairing;R0822 source removal coupled but not counted yet.826/2720 points;main1/4;no partial-file build.

R0820 declaration minimization: [review](r0820-main-declaration-review.json). Source/ABI evidence only; P67 final verification pending.

main.c final points: [R0821](r0821-main-review.json), [R0822](r0822-main-review.json); P67 source changes await final verification.

P67 whole-file checks: [ios.c](ios-c-file-review.json), [main.c](main-c-file-review.json), [logical lines](batch-p67-line-check.json).

P67 [delivery checkpoint](batch-p67-checkpoint.json): cached build, dual publication and Full regression passed; included in the owner-requested combined Git delivery. Three full restorations are now verified/published; residual decisions remain pending.

## Supplemental new mirror differences

[MVDM-HOST-DIV-901: Console font restoration](div-901-console-font-restore.json) is recorded in the pending decision tracker as implemented and published, with long-term retention still pending. Its +10/-0 physical lines are a new owner-requested bug repair, separate from the frozen 2720-point R census; this registration does not advance the ordered audit or mark retention approved.

[RDP input checkpoint](rdp-input-checkpoint.json) records the current published adaptation and Full19 pass, with the owner subsequently accepting the input fix and requesting its retention. The exit is suspected to be test interference; the service-identity collision hypothesis was not established by the attempted probe.

P68 [terminat.c review](r0823-r0824-terminat-review.json): minimal cleanup entry retained pending owner; comment-only mirror diff-3.

P68 [xt.c review](r0825-r0828-xt-review.json): original register control fails compilation; duplicate parity provider excluded; added comments removed, mirror diff-14.

P68 [ica.c review](r0829-r0831-ica-review.json): PIC reset/EOI proof and24 passing production-object assertions;format-only mirror diff-7.

P68 [idetect.c review](r0832-idetect-review.json): unnecessary replacement include removed; original path fails, omission preserves code/data/relocations;mirror diff-1.

P68 [cga.c review](r0833-r0834-cga-review.json): original function-pointer definitions restored, object equivalence proved;mirror diff-6.

P68 [delivery receipt](batch-p68-checkpoint.json) and [line checks](batch-p68-line-check.json): five mirror files, original-relative diff net-31; cached build passed and 11 images published to both destinations; Full passed; Git delivery accompanies this receipt.

P69 [ega_prts.c review](r0835-r0836-ega-prts-review.json): two minimal void prototypes pending owner; comment-only diff-2; actual omitted-prototype compile warns C4013, not runtime failure.

P69 [ega_read.c review](r0837-r0838-ega-read-review.json): exact return declarations pending owner; diff-3, original line138 restored; compiler controls report incompatible implicit-int declarations.

P69 [ega_vide.c review](r0839-r0841-ega-vide-review.json): virtual geometry retained with historical failure proof; two original casts restored and57-section object identity verified;diff-7; batch delivery pending.

P69 [batch receipt](batch-p69-checkpoint.json) and [line checks](batch-p69-line-check.json): three frozen sources, diff-12; cached build, publication11x2 and Full19 passed; Git delivery accompanies receipt.

P70 [ega_writ.c review](r0842-r0845-ega-writ-review.json), [batch receipt](batch-p70-checkpoint.json), [line checks](batch-p70-line-check.json): 8 original declarations restored;2 residuals qualified;mirror diff-21; cached build, publication11x2 and Full19 passed; Git delivery accompanies receipt.

P71 [egawrtm0.c review](r0846-r0851-egawrtm0-review.json): 4 scalar-address negatives verified;2 existing excluded-error blocks preserved under owner policy;comment-only diff-11(includes7 adjacent table-explanation lines outside those frozen point ranges);no selected token changes.

P71 [egwrtm12.c review](r0852-r0863-egwrtm12-review.json):10 per-point scalar negatives,2 approved exclusions;comment-only diff-11(includes6 adjacent carrier-explanation lines);no selected token changes.

P71 [gfx_updt.c initial decisions](r0864-r0865-gfx-start-review.json):2/77 points;R0864 minimum prototypes pending;R0865 original boolean restoration decided, not implemented until whole-file review.

- [R0866-R0870 dirty scan/copy controls](r0866-r0870-gfx-dirty-review.json): five qualified residuals; production unchanged.

- [R0871-R0902 callback/text review](r0871-r0902-gfx-callback-text-review.json):12 exact restoration decisions,20 residuals;135 candidate object sections equal for zero-address cleanup.

- [R0903-R0914 CGA controls](r0903-r0914-gfx-cga-review.json):one zero-address restoration,11 qualified residuals; separates wrong pixels from excess dirty-span repaint.

- [R0915-R0940 and whole-file counter-review](r0915-r0940-gfx-final-review.json):gfx_updt.c77/77 complete,19 original points restored locally; original-relative diff-92; P71 three files-114.

- P71 [checkpoint](batch-p71-checkpoint.json) and [line checks](batch-p71-line-check.json): completed three-file batch, delivery pending.

P71 final delivery: cached x86/x64 full build,11x2 publication,Full19,CVID,143 WOW objects/9 negative controls and governance passed; checkpoint carries package identity and explicit DOS4GW skip.

- [R0941-R0942 V7 pointer review](r0941-r0942-v7-ports-review.json):two qualified residuals; comment-only mirror diff-2; P72 accumulation.

- [R0943-R0946 whole V7 mode file](r0943-r0946-v7-video-review.json):4/4 complete;2 original restorations,2 minimized residuals pending owner; mirror diff-10, P72 cumulative-12; delivery pending.

P72 final delivery: [checkpoint](batch-p72-checkpoint.json), [original line proof](batch-p72-line-check.json); cached x86/x64 build,11x2 publication,CVID,Full and governance passed; mirror diff-12. Git delivery accompanies record.

- [R0947-R0949 VGA scanout review](r0947-r0949-vga-mode-review.json):3/3 complete;all residuals qualified,one minimized;mirror diff-6;P73 delivery pending.

P73 final delivery: [checkpoint](batch-p73-checkpoint.json), [line proof](batch-p73-line-check.json);cached x86/x64 build,11x2 publication,CVID,Full and governance passed;mirror diff-6. Git delivery accompanies record.

- [R0950-R0953 VGA port review](r0950-r0953-vga-ports-review.json):4/4 complete;2 original restorations,2 minimized residuals pending owner;mirror diff-22;P74 delivery pending.

P74 final delivery: [checkpoint](batch-p74-checkpoint.json), [line proof](batch-p74-line-check.json);cached x86/x64 build,11x2 publication,CVID,Full and governance passed;mirror diff-22. Git delivery accompanies record.

- [R0954-R0969 video BIOS review](r0954-r0969-video-review.json):16/16 complete;3 original restorations,13 qualified residuals;P75 delivery pending.

P75 final delivery: [checkpoint](batch-p75-checkpoint.json), [line proof](batch-p75-line-check.json);cached x86/x64 build,11x2 publication,CVID,Full and governance passed;mirror diff-10. Git delivery accompanies record.

P76 whole-file batch: [cfpu_def.h15/15](r0970-r0984-cfpu-review.json), [host_cpu.h9/9](r0985-r0993-host-cpu-review.json), [line proof](batch-p76-line-check.json), [checkpoint](batch-p76-checkpoint.json). Two headers net mirror-334;7 original restorations delivered;17 residuals qualified;cached build,publication11x2,Full and governance passed. Git delivery accompanies checkpoint.

P77: [host_def.h4/4](r0994-r0997-host-def-review.json), [line proof](batch-p77-line-check.json), [checkpoint](batch-p77-checkpoint.json). Mirror41->15(-26);two restores delivered,two residuals qualified;cached build,publication11x2,Full and governance passed. Git delivery accompanies checkpoint.

P78: [five files](r0998-r1004-host-small-review.json), [line proof](batch-p78-line-check.json), [checkpoint](batch-p78-checkpoint.json). Mirror net-50;five restores delivered,one minimized residual;R1000 already delivered. Cached full build,publish11x2,Full and governance passed;Git delivery accompanies checkpoint.

P79: [nt_bop10/10](r1005-r1014-nt-bop-review.json), [lines](batch-p79-line-check.json), [checkpoint](batch-p79-checkpoint.json). Mirror329->188(-141);three restores delivered,seven qualified residuals. Cached full build,publish11x2,Full and governance passed;Git delivery accompanies checkpoint.

P80 delivered: [nt_cga six-point review](r1015-r1020-nt-cga-review.json);mirror353->338(-15),comments and lines only;all6 residuals qualified;449 actual selected-line matches;cached build,publish11x2,Full19 passed.
P80 delivery evidence: [checkpoint](batch-p80-checkpoint.json), [line proof](batch-p80-line-check.json).
