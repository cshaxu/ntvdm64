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

P81 delivered: [nt_cpu19-point review](r1021-r1039-nt-cpu-review.json), [lines](batch-p81-line-check.json), [checkpoint](batch-p81-checkpoint.json). Mirror188->118(-70);13 original restorations,5 qualified residuals,1 policy-retained excluded framework;final cached build,publish11x2,Full19 passed.

P82 delivered: [nt_emm7-point review](r1040-r1046-nt-emm-review.json), [lines](batch-p82-line-check.json), [checkpoint](batch-p82-checkpoint.json). Mirror131->92(-39);7 residuals qualified;real controls0/AV/0;all5 directions/failures,cached build,publish11x2,Full19 passed.

P83 audit-only: [nt_eoi R1047](r1047-nt-eoi-review.json);owner-approved hardware monitor declaration removal;diff delta0;176 actual selected-line matches. No source change;P82 runtime baseline unchanged.

P84 delivered: [nt_error8-point review](r1048-r1055-nt-error-review.json), [lines](batch-p84-line-check.json), [checkpoint](batch-p84-checkpoint.json). Mirror52->32(-20);six originals restored,two thread ABI residuals;656 selected-line matches;cached build,publish11x2,Full19 passed.

nt_event in progress: [R1056/R1057 interface evidence](r1056-r1057-nt-event-header-review.json);2/44 points reviewed,whole file not complete. No source change or new build/publication;P84 remains baseline.

P86 audit-only: [R1058-R1063 boundaries](r1058-r1063-nt-event-boundary-review.json);nt_event8/44 points. Current/original function controls distinguish key-origin and BIOS-release failures;actual helper exposes premature-start join hang. Lifetime corrections explicitly pending,not certified. No source changes;P84 baseline unchanged.

P87 audit-only: [R1064-R1069 input dispatch](r1064-r1069-nt-event-input-review.json);nt_event14/44 points. Actual bounded-input fixture and native wait-order counterexample pass;shutdown authority checked in source. No production changes;P84 remains published.

P88 audit-only: [R1070-R1078 origin and mouse lifecycle](r1070-r1078-nt-event-origin-review.json);nt_event23/44 points. Original history restoration/dispatch omission controls fail specifically;delay control quantifies cost without claiming an original functional bug. No production changes;P84 remains published.

P89: [R1079-R1087 handoff](r1079-r1087-nt-event-handoff-review.json);nt_event32/44 points. R1087 restore decided,not applied. Fixed stale publisher duplicate-suppression assertions against explicit T440 contract;targeted fixture passes. No production changes;P84 remains published.

P90 delivered: [whole-file checkpoint](batch-p90-checkpoint.json), [line proof](batch-p90-line-check.json), [R1088-R1099](r1088-r1099-nt-event-return-review.json). nt_event44/44 audited;two originals restored;mirror253->187(-66),937 original selected lines match. Cached full build,publish11x2/hash,Full19 passed;R1062/R1063 corrections remain pending.

P91 delivered: [nt_fdisk review](r1100-r1110-nt-fdisk-review.json), [byte/line proof](batch-p91-line-check.json), [checkpoint](batch-p91-checkpoint.json). All11 points checked;9 newly reconciled/restored,2 prior delivered. File is byte-identical to original;actual header controls and cached full build,publish11x2,Full19 passed.

P92 audit-only: [nt_fulsc first three route boundaries](r1111-r1113-nt-fulsc-route-review.json).3/11 points,source-proven residuals pending owner;no product change. P91 remains published.1117 reviewed,713 pending,1603 unreviewed;143 whole files complete.

P93 audit-only: [hardware exclusion and shared planes](r1114-r1116-nt-fulsc-hardware-plane-review.json).nt_fulsc6/11 reviewed;3 residuals source-qualified;prior CHAIN2 controls explicitly historical. No product change;P91 remains published.1120 reviewed,716 pending,1600 unreviewed.

P94 audit-only: [cursor register ownership](r1117-r1119-nt-fulsc-cursor-review.json).nt_fulsc9/11 reviewed;R1117/R1118 qualified residuals,R1119 original condition restoration decided but not applied. No product changes;P91 remains published.1123 reviewed,718 pending,1 restore decided,1597 unreviewed.

P95 delivered: [final two points](r1120-r1121-nt-fulsc-final-review.json), [lines](batch-p95-line-check.json), [checkpoint](batch-p95-checkpoint.json).nt_fulsc11/11 audited;R1119 original condition restored;mirror1510->1502(-8),including retained exclusions,excluding #line. Cached full build,publish11x2/hash,Full19 passed.

P96 audit-only: [nt_graph header and DAC boundaries](r1122-r1124-nt-graph-headers-review.json).3/24 points reviewed;actual source omission controls distinguish implicit declarations from compile errors. No source edits or new runtime claims;P95 remains published.1128 reviewed,723 qualified pending,1592 unreviewed.

P97 audit-only: [R1125-R1128 publication and clear ownership](r1125-r1128-nt-graph-publish-clear-review.json).nt_graph7/24;R1126 original FULLSCREEN clear guard restoration decided,not applied;three source-qualified residuals.1132 reviewed,726 pending,1 restore decided,1588 unreviewed. No source changes;P95 remains published.

P98 audit-only: [R1129-R1134 flush/tick controls](r1129-r1134-nt-graph-flush-tick-review.json).nt_graph13/24;current graphics flush fails4/6 original-contract cases,candidate passes6/6;blocked and mode-settle omission controls fail. R1129 minimization decided,not applied.1138 reviewed,732 pending,1 restore decided,1582 unreviewed. P95 remains published.

P99 audit-only: [R1135-R1139 cursor ownership](r1135-r1139-nt-graph-cursor-review.json).nt_graph18/24;R1136-R1139 original restoration decided;R1135 duplicate declaration/comment reduction. Original dirty producers refine R1125 without reopening its one-bit publication boundary.1143 reviewed,733 pending,5 restores decided,1577 unreviewed. P95 remains published.

P100 audit-only: [R1140-R1145 presentation boundaries](r1140-r1145-nt-graph-presentation-review.json). nt_graph24/24 dispositions complete;five restorations and associated reductions await whole-file implementation.1149 reviewed,739 pending,5 restores decided,1571 unreviewed;145/365 files. P95 remains published.

P101 delivered: [whole-file restoration/minimization](batch-p101-checkpoint.json), [final original logical lines](batch-p101-line-check.json). nt_graph original-relative diff329->262 (-67);five original restorations,19 residual decisions. Cached full x86/x64,11x2 publication/hash verification,focused andFull19 passed;Git delivery accompanies record.

P102 delivered: [nt_hosts whole-file review](r1146-r1150-nt-hosts-review.json), [lines](batch-p102-line-check.json), [checkpoint](batch-p102-checkpoint.json). Mirror86->56(-30);four original restorations and one QuickEdit residual;121 original selected-line matches. Cached full x86/x64,publish11x2/hash,Full19 and governance passed;Git delivery accompanies receipt.

P103 delivered: [nt_inthk whole-file review](r1151-r1169-nt-inthk-review.json), [line check](batch-p103-line-check.json), [checkpoint](batch-p103-checkpoint.json). Mirror diff net-329 across2 files;21 points restored;cached full build,publish11x2,focused/Full19 and governance passed;Git delivery accompanies receipt.

P104 [nt_keycd whole-file review](r1170-r1172-nt-keycd-review.json): R1170 original blank restored;
R1171/R1172 actual-object boundary controls justify two minimal residuals pending owner.
Mirror6->4(-2);148/365 files,1176/2720 points reviewed;428 restored,8 approved,740 pending,1544 unreviewed.
Comment/newline-only delivery reuses published P103 package; no fresh full build claimed.

P105 [nt_mem whole-file review](r1173-r1175-nt-mem-review.json):R1173 restored;R1174/R1175 minimized residuals pending owner.
Mirror145->139(-6),539 exact original logical lines,20 object sections/bindings equal.
149/365 files;1179 reviewed,429 restored,8 approved,742 pending,1541 unreviewed.
No-binding/comment-only delivery reuses P103 publication and runtime evidence.

P106 [nt_mouse whole-file review](r1176-r1183-nt-mouse-review.json):3 P66 restorations confirmed,5 residuals minimized/pending.
Mirror706->698(-8),993 original lines exact,48 object sections/bindings equal;17 original coordinate cases pass.
150/365 files;1187 reviewed,432 restored,8 approved,747 pending,1533 unreviewed.
Comment/CRLF-only delivery reuses published P103.

P107 [nt_msscs review](r1184-r1195-nt-msscs-review.json):whole-file delivered mirror171->129(-42).
[Checkpoint](batch-p107-checkpoint.json);[line proof](batch-p107-line-check.json).
151/365 files reviewed;1199 points:440 restored delivered,0 local,8 approved,751 pending,1521 unreviewed.
Cached full build,publish11x2,Full19 and governance passed;Git delivery accompanies receipt.

P108 [nt_pif review](r1196-r1201-nt-pif-review.json):delivered mirror31->14(-17).
[Checkpoint](batch-p108-checkpoint.json);[line proof](batch-p108-line-check.json).
152/365 files;1205 points reviewed:441 delivered restorations,0 local,8 approved,756 pending,1515 unreviewed.
Cached full build,publish11x2,PIF focused/negative controls,Full19 and governance passed;Git delivery accompanies receipt.

P109 [nt_reset review](r1202-r1205-nt-reset-review.json):delivered mirror115->99(-16).
[Checkpoint](batch-p109-checkpoint.json);[line proof](batch-p109-line-check.json).
153/365 files;1209 points reviewed:442 delivered restorations,0 local,8 approved,759 pending,1511 unreviewed.
Cached full build,publication11x2,Full19 and governance passed;Git delivery accompanies receipt.

P110 [nt_rez review](r1206-r1208-nt-rez-review.json):delivered mirror15->5(-10).
[Checkpoint](batch-p110-checkpoint.json);[line proof](batch-p110-line-check.json).
154/365 files;1212 points reviewed:444 delivered restorations,0 local,8 approved,760 pending,1508 unreviewed.
Cached full build,publication11x2,Full19 and governance passed;Git delivery accompanies receipt.

P111 [nt_rflop review](r1209-r1228-nt-rflop-review.json):mirror349->339(-10).
[Checkpoint](batch-p111-checkpoint.json);[line proof](batch-p111-line-check.json).
155/365 files;1232 points reviewed:461 restored/confirmed,8 approved,763 pending,1488 unreviewed.
One include restored;16 prior restorations confirmed;three thread residuals pending.43 object sections/relocations equal;P110 publication/Full reused.

P112 [sound/thread review](r1229-r1230-sound-thread-review.json):mirror4->2 and10->4(net-8).
[Checkpoint](batch-p112-checkpoint.json);[line proof](batch-p112-line-check.json).
157/365 files;1234 points reviewed:463 restored/confirmed,8 approved,763 pending,1486 unreviewed.
Both files selected-code original;294 logical lines exact,20 object sections/relocations equal;P110 publication/Full reused.

P113 [timer review](r1231-r1233-nt-timer-review.json):delivered mirror109->96(-13).
[Checkpoint](batch-p113-checkpoint.json);[line proof](batch-p113-line-check.json).
158/365 files;1237 points reviewed:466 delivered restorations,0 local,8 approved,763 pending,1483 unreviewed.
Cached full build,x86/x64 native alert,publish11x2,Full19 and governance passed;Git delivery accompanies receipt.

P114 [nt_unix review](r1234-r1239-nt-unix-review.json):delivered mirror100->29(-71).
[Checkpoint](batch-p114-checkpoint.json);[line proof](batch-p114-line-check.json).
159/365 files;1243 reviewed:471 delivered restorations,0 local,8 approved,764 pending,1477 unreviewed.
Whole-file original restored except one SDK-conflicting declaration and two unchanged exclusions;cached full build,publish11x2,firmware/media/boot x86/x64,Full19 and governance passed.

P115 [nt_vga review](r1240-r1241-nt-vga-review.json):mirror566->564(-2);two executable residuals pending.
[Checkpoint](batch-p115-checkpoint.json);[line proof](batch-p115-line-check.json).
160/365 files;1245 reviewed:471 delivered restorations,0 local,8 approved,766 pending,1475 unreviewed.
Actual painter current passes;original and no-clip controls fail specified assertions.941 line checks;26 product sections/relocations identical;P114 publication/Full reused.

P116 [SIM32 review](r1242-r1248-sim32-review.json):whole-file mirror151->126(-25);all seven residuals pending owner.
[Checkpoint](batch-p116-checkpoint.json);[line proof](batch-p116-line-check.json).
161/365 files;1252 reviewed:471 delivered restorations,8 approved,773 pending,1468 unreviewed.
Final compile,156 original logical lines,cached full build,publish11x2,Full19 and memory/descriptor fixtures pass;Git accompanies receipt.

P117 [sources/stubs review](r1249-r1250-original-restoration.json):two original declarations restored;mirror net-6.
[Checkpoint](batch-p117-checkpoint.json);[line proof](batch-p117-line-check.json).
163/365 files;1254 reviewed:473 delivered restores,0 local,8 approved,773 pending,1466 unreviewed.
Real source-list parser unchanged;full product link before/after raw-identical;204 selected original lines exact. Published11x2 identities verified;P116 Full19 reused;Git accompanies receipt.

P118 [entry review](r1251-r1255-entry-review.json):mirror36->27(-9);five residuals pending owner.
[Checkpoint](batch-p118-checkpoint.json);[line proof](batch-p118-line-check.json).
164/365 files;1259 reviewed:473 delivered restores,8 approved,778 pending,1461 unreviewed.
158 original logical lines exact;ABI/object controls,cached full build,publish11x2 and Full19 pass;Git accompanies receipt.

P119 [metadata restoration](r1256-r1257-metadata-restoration.json):two files raw-identical to pinned Git originals;mirror-8.
[Checkpoint](batch-p119-checkpoint.json). Actual worker graph/parser unchanged;P118 published11x2 hashes and Full19 reused.
166/365 files;1261 reviewed:475 delivered restores,8 approved,778 pending,1459 unreviewed;Git accompanies receipt.

P120 [VCDEX entry review](r1258-vcdex-entry-review.json):+1/-1 remains;inline comment removed,mirror line delta0.
[Checkpoint](batch-p120-checkpoint.json). Original link fails entry ABI;global stdcall fails12 imports;final object identity and published11x2 hashes pass;P118 Full19 reused.
167/365 files;1262 reviewed:475 restores,8 approved,779 pending,1458 unreviewed;Git accompanies receipt.

P121 [DLC review](r1259-r1260-vrdlc5c-review.json):R1259 line mapping confirmed,R1260 +1/-1 residual pending owner. Source unchanged,mirror delta0.
[Checkpoint](batch-p121-checkpoint.json):1579 line matches,current33/0 versus original35/5 initializer assertions;P118 published11x2/Full19 reused.
168/365 files;1264 reviewed:476 restored/confirmed,8 approved,780 pending,1456 unreviewed.

P122 [DLC line review](r1261-r1263-dlc-lines-review.json):3 files confirmed already correct;mirror delta0.
[Checkpoint](batch-p122-checkpoint.json):431+1401 selected lines and3 header consumers;source unchanged,P118 published11x2/Full19 reused.
171/365 files;1267 reviewed:479 restored/confirmed,8 approved,780 pending,1453 unreviewed.

P123 [redirector ABI review](r1264-r1265-redir-abi-review.json):vrdll line mapping confirmed;vrica mirror4->2(net-2),residual pending.
[Checkpoint](batch-p123-checkpoint.json):actual original and /Gz negative links,4 object identities;P118 publication/Full reused.
173/365 files;1269 reviewed:480 restored/confirmed,8 approved,781 pending,1451 unreviewed.

P124 [redirector line review](r1266-r1268-redir-lines-review.json):3 files confirmed already correct;mirror delta0.
[Checkpoint](batch-p124-checkpoint.json):264+16+456 selected lines;source unchanged,P118 published11x2/Full19 reused.
176/365 files;1272 reviewed:483 restored/confirmed,8 approved,781 pending,1448 unreviewed.

P125 in progress: [NetAPI investigation](r1269-r1279-investigation.json). Actual CCPU alias counterexample and current12-case provider baseline pass;obsolete pre-init frame assertion removed. No point disposition or delivery claimed.

P125 [whole-file review](r1269-r1279-vrnetapi-review.json):mirror814->747(-67);provider16/copy14 and1448 logical lines pass. Full delivery pending;3 local restores,8 qualified residuals.

P125 [frozen build checkpoint](batch-p125-checkpoint.json) and [line proof](batch-p125-line-check.json);delivery pending.

P125 final cached x86/x64,publication11x2,Full19 and governance passed;Git accompanies checkpoint.177/365 files;1283/2720 points:486 restored/confirmed,8 approved,789 pending,1437 unreviewed.

P126 [vrnetb review](r1280-vrnetb-review.json):185 original lines,4 DBG exclusions,zero source change.
[Checkpoint](batch-p126-checkpoint.json):P125 publication/Full reused with22 hashes.178/365 files;1284/2720 points.

P127 source review complete, delivery pending: [review](r1281-r1296-vrnmpipe-review.json), [controls](r1281-r1296-vrnmpipe-investigation.json), [native API control source](r1281-r1296-native-count-control.c). vrnmpipe mirror685->646(-39);7 local restorations,9 residual decisions. Final-source build/publication/tests pending;179/365 files,1296/2720 points reviewed.

P127 [sealed inputs](batch-p127-checkpoint.json); final delivery remains pending.

P127 counter-review removed circular fresh-request initialization/check. Initial candidate published and focused26 passed; Full intentionally deferred until final candidate rebuild/republication. Current counts483 delivered,7 local,8 approved,798 pending,1424 unreviewed.

P127 [final compiler line proof](batch-p127-line-check.json) checks967 selected lines with0 mismatches and unchanged exclusions.

P127 [count reconciliation](p127-progress-reconciliation.json):P111 double-counted four previously delivered points. Main-register dispositions remain unchanged; cumulative counts now derive from actual rows, not batch additions.

P127 final delivery gates passed:cached full,publish11x2,focused27,old-adapter negative,Full19,967 lines and governance. Git accompanies receipt.179/365 files;1296/2720 points:490 delivered,8 approved,798 pending,1424 unreviewed.

P128 [vrputil review](r1297-vrputil-review.json), [receipt](batch-p128-checkpoint.json):only one required diagnostic #error replacement;75 original lines match;source unchanged. P127 package/Full reused with22 hashes checked.180/365 files,1297/2720 points:491 confirmed/restored,8 approved,798 pending,1423 unreviewed.

P129 [source checkpoint](batch-p129-checkpoint.json), [line proof](batch-p129-line-check.json):vrremote mirror43->34(-9);644 shared selected lines0 mismatch.181/365 files source-reviewed;1301/2720 points:491 delivered,2 local pending verification,8 approved,800 qualified pending,1419 unreviewed. Cached build/publication/final tests and Git delivery remain pending.

P129 final cached build,11x2 publication,focused4 and Full19 passed;delivery governance/Git follow.181/365 files;1301/2720 points:493 delivered,8 approved,800 pending,1419 unreviewed.

P130 [fastwow review](r1302-fastwow-review.json), [receipt](batch-p130-checkpoint.json):four excluded bodies only;both WOW contexts retain original declarations/lines. No product change;P129 package/Full19 reused with22 hashes.182/365 files;1302/2720 points;494 delivered/confirmed,8 approved,800 pending,1418 unreviewed.

P131 [isvwow review](r1303-isvwow-review.json), [receipt](batch-p131-checkpoint.json):diff3->2(-1),comment/line-only. Original product /Gz passes;external-default /Gd fails;residual explicit APIENTRY pending owner.183/365 files;1303/2720 points;494 restored/confirmed,8 approved,801 pending,1417 unreviewed. P129 runtime/publication reused.

P132 [mapembed review](r1304-mapembed-review.json), [receipt](batch-p132-checkpoint.json):one DEBUG exclusion only;242 selected lines0 mismatches/changes. Source unchanged;P129 package/Full19 reused with22 hashes.184/365 files;1304/2720 points;495 restored/confirmed,8 approved,801 pending,1416 unreviewed.

P133 in progress: [R1305 restoration evidence](r1305-precomp-investigation.json). Guard-only removal fails because of duplicated autonomous enum;removing both restores original inclusion and passes3 actual-source controls. Local mirror delta-6;R1306-R1308 must finish before final build/publication/tests.184/365 complete files;1305 reviewed points,1 local restoration,1415 unreviewed.

P133 whole-file source review complete: [checkpoint](batch-p133-checkpoint.json), [line proof](batch-p133-line-check.json). Mirror41->16(-25);R1305 restored locally,R1306/R1308 minimized,R1307 required include path unchanged. Cached full passed;11x2 published;final focused/Full and Git pending.185/365 files;1308/2720 points:495 delivered,1 local,8 approved,804 qualified pending,1412 unreviewed.

P133 final cached build,publish11x2,public ABI,menu-resource and Full19 passed. Mirror41->16(-25);96 static include-line anchors match;WOW boundary checked.185/365 files;1308/2720 points:496 delivered/confirmed,8 approved,804 pending,1412 unreviewed. Final governance and Git accompany receipt.

P134 [waccel review](r1309-waccel-review.json), [receipt](batch-p134-checkpoint.json):only3 DBG exclusions;224 selected lines match original content and lines. Source unchanged;P133 package/Full19 reused with22 hashes.186/365 files;1309/2720 points:497 restored/confirmed,8 approved,804 pending,1411 unreviewed.

P135 [whole-file restoration checkpoint](batch-p135-checkpoint.json), [line proof](batch-p135-line-check.json):walias mirror164->135(-29);original GDI suffix restored without changing live aliases.342 selected lines match;syntax passes. Cached full/publication/runtime/Git pending.187/365 files;1311/2720 points:497 delivered,2 local,8 approved,804 pending,1409 unreviewed.

P135 final cached build,publish11x2,public ABI,menu-resource,Full19 and WOW boundary passed. Mirror164->135(-29);342 selected lines match.187/365 files;1311/2720 points:499 delivered/confirmed,8 approved,804 pending,1409 unreviewed. Final governance and Git accompany receipt.

P136 [whole-file checkpoint](batch-p136-checkpoint.json), [line proof](batch-p136-line-check.json):walias.h mirror51->31(-20);six unused macros restored;remaining six residuals pending owner.188/365 files;1323/2720 reviewed;499 delivered,6 local,8 approved,810 pending,1397 unreviewed. Cached build/publication/tests/Git pending.

P136 final cached build,publish11x2,source contrasts,public ABI,menu-resource,Full19 and WOW boundary passed. Mirror51->31(-20);82 selected lines per normal/Unicode context match.188/365 files;1323/2720 points:505 delivered/confirmed,8 approved,810 pending,1397 unreviewed. Final governance and Git accompany receipt.

P137 [whole-file checkpoint](batch-p137-checkpoint.json), [line proof](batch-p137-line-check.json):wcall16 mirror222->198(-24);original frame reads and cast-lvalues restored;duplicate LastError handling removed.189/365 files;1335/2720 reviewed;505 delivered,5 local,8 approved,817 pending,1385 unreviewed. Final verification and delivery pending.

P137 final cached build,publish11x2,callback/CCPU source contrasts,public ABI,menu-resource,Full19 and WOW boundary passed. Mirror222->198(-24);370 selected lines match.189/365 files;1335/2720 points:510 delivered/confirmed,8 approved,817 pending,1385 unreviewed. Final governance and Git accompany receipt.

R1336-R1370 [initial source and diagnostic evidence](r1336-r1370-initial-evidence.md): wcntl32 whole-file review in progress; mirror +54/-32 (86). All14 fixture control baseline bodies match pinned OpenNT. Existing transaction fixture passes after its source check accepts diagnostic #line directives. No mirror change, completed-count advance or new delivery claimed.

P138 [whole-file checkpoint](batch-p138-checkpoint.json), [line proof](batch-p138-line-check.json):wcntl32 mirror86->69(-17);R1350 restored,34 minimized/unchanged residuals pending.190/365 files;1370/2720 points;510 delivered,1 local,8 approved,851 pending,1350 unreviewed. Final delivery pending.

P138 final cached build,publish11x2,transaction167/parameter-map54 source contrasts,public ABI,menu-resource,Full19 and WOW boundary passed. Mirror86->69(-17);540 selected lines match.190/365 files;1370/2720 points:511 delivered/confirmed,8 approved,851 pending,1350 unreviewed. Final governance and Git accompany receipt.

P139 [whole-file checkpoint](batch-p139-checkpoint.json), [line proof](batch-p139-line-check.json):wcommdlg mirror239->234(-5);selected differences27->22,excluded212 unchanged. R1379 restored,R1371 structure confirmed,11 residuals pending.191/365 files;1383/2720 points;511 delivered,2 local,8 approved,862 pending,1337 unreviewed. Final delivery pending.

P139 final cached build,publish11x2,transaction169,common-dialog callback,native42/two references,public ABI,menu-resource,Full19 and WOW boundary passed. Mirror239->234(-5);1218 selected lines match.191/365 files;1383/2720 points:513 delivered/confirmed,8 approved,862 pending,1337 unreviewed. Final governance and Git accompany receipt.

P140 partial: [R1384-R1395 review](r1384-r1395-partial-review.json), [initiator original/current control](r1384-r1386-initiator-review.json). wdde.c12/29 points complete, file not complete. Local original-relative diff171->170; final line/build/publication/testing pending. P139 delivered as fa427360e.

P140 [R1396-R1404 converter review](r1396-r1404-conversion32-review.json): original read order restored, exact-body candidate8 cases pass.21/29 points reviewed; whole file and delivery remain incomplete.

P140 [R1405-R1412 tail review](r1405-r1412-tail-review.json): wdde.c29/29 source-reviewed; COPYDATA empty-list adaptation restored, DDE residual evidence qualified. Final validation/publication remains pending.

P140 [targeted checkpoint](p140-targeted-checkpoint.json):717 selected lines match, DDE alias and COPYDATA59 pass; cached full build started. No publication or Full result yet.

P140 [delivery checkpoint](batch-p140-checkpoint.json) and [compiler line proof](batch-p140-line-check.json); delivery status is updated only after each gate completes.

P140 final gates passed:cached full,11x2 publication,DDE conversion8/alias101,COPYDATA59,public ABI,menu-resource,Full19,WOW boundary. Mirror wdde171->143,header-1;total-29.192/365 files;1412/2720 points:516 delivered,8 approved,888 pending,1308 unreviewed. Final governance/Git accompany receipt.

P140 delivered as0efde9480;remote matched and worktree clean. P141 [wdde.h whole-file review](r1413-r1417-wdde-header-review.json):5/5 points source-reviewed,mirror27->24(-3);final line/alias/build checks running.193/365 files;1417/2720 points.

P141 [checkpoint](batch-p141-checkpoint.json), [actual header line check](batch-p141-line-check.json); publication/Full/Git pending.

P141 first Full attempt failed version-negatives:reply-protocol server exited1740(RPC duplicate endpoint), after three negative variants passed. Failure-time occupant was not captured; do not attribute it retrospectively. Publication remains complete, commit/push stopped; isolated process-identity diagnosis running. See batch-p141-checkpoint.json.

P141 final gates passed:cached full,11x2 publication,alias101,public ABI,menu-resource,Full19,WOW boundary. Mirror header27->24(-3);62 selected lines match.193/365 files;1417/2720 points:516 delivered,8 approved,893 pending,1303 unreviewed. Final governance/Git accompany receipt.

P141 delivered e3b634f6c;remote matched,tree clean. [wdib whole-file review](r1418-r1420-wdib-review.json):R1418-R1420 complete;mirror7->6(comment only),556 lines match;original GDI cleanup fails,current passes.194/365 files;1420/2720 points,516 delivered,2 local,8 approved,894 pending,1300 unreviewed.

[wdos whole-file review](r1421-r1422-wdos-review.json):R1421-R1422 complete;mirror33->32(comment only),367 lines match;original stdcall versus worker cdecl proved by actual object symbols.195/365 files;1422/2720 points,516 delivered,3 local,8 approved,895 pending,1298 unreviewed. Cumulative local mirror delta-2;no fresh Full claim.

[wgdi whole-file review](r1423-r1424-wgdi-review.json):R1423-R1424 complete;mirror32->31,original LF and#line2365 restored;2713 lines match;one release retains explicit original-failure ownership evidence.196/365 files;1424/2720 points,516 delivered,4 local,8 approved,896 pending,1296 unreviewed. Cumulative local mirror delta-3;format/line only.

[wgdi31 whole-file review](r1425-r1426-wgdi31-review.json):R1425-R1426 complete;mirror12->11,LF/#line348 restored;639 lines match;original20/2 versus current20/0 font output controls.197/365 files;1426/2720 points,516 delivered,5 local,8 approved,897 pending,1294 unreviewed. Cumulative local mirror delta-4;format/line only.

[wgmeta whole-file check](r1427-wgmeta-review.json):R1427 confirms selected original code/317 lines;no source change,35->35 exclusions only. P141 source identity/build/Full reused.198/365 files;1427/2720 points;517 delivered/confirmed,5 local,8 approved,897 pending,1293 unreviewed.

[wgpal whole-file review](r1428-r1429-wgpal-review.json):R1428-R1429 complete;mirror3->2,original LF restored;165 lines match;original19/5 versus final19/0 native palette controls.199/365 files;1429/2720 points,517 delivered,6 local,8 approved,898 pending,1291 unreviewed. Cumulative local mirror delta-5;format/line only;existing test accepts Source for same-fixture controls.

[wgprnset whole-file review](r1430-r1433-wgprnset-review.json):R1430-R1433 complete;mirror41->38,344 lines match;three original early-return leaks source-proved.200/365 files;1433/2720 points;517 delivered,7 local,8 approved,901 pending,1287 unreviewed. P142 cumulative six-file net-8 enters cached full/publication/testing.

P142 [delivery checkpoint](batch-p142-checkpoint.json) and [actual preprocessor line proof](batch-p142-line-check.json);gates and publication remain pending until their exit evidence is recorded.

P142 cached full passed;all11 published and hash-verified in both roots,package eca2357e-a38c-401d-9296-3c9f471544b9. Focused/Full and Git delivery still pending;see checkpoint.

P142 final gates passed:cached x86/x64,11x2 publication,DIB82/font20/palette19,ABI/menu-resource,isolated Full19,WOW boundary. Six mirror files net-8;5101 selected lines match.200/365 files;1433/2720 points:524 delivered/confirmed,0 local,8 approved,901 pending,1287 unreviewed. Initial runner failure preserved;final governance/Git accompany receipt.

P142 committed/pushed as 0410aabe573c89b86d58ac5b8f0783668ac5a651;clean worktree verified after delivery.

[wheap whole-file check](r1434-wheap-review.json):R1434 has only the approved DEBUG exclusion,+1/-3,selected diff0;17 selected logical lines match. No source change;P142 product verification reused.201/365 files;1434/2720 points:525 restored/confirmed delivered,0 local,8 approved,901 pending,1286 unreviewed. Next R1435-R1438.

[winsockp whole-file review](r1435-r1438-winsockp-review.json):R1435-R1438 complete;mirror26->24;LF/typedef#line72 restored;105 selected lines match. Three evidence-qualified residuals stay pending. Current rundown323/0,original-finder control281/39,real Win16 Completion/Cancel/TaskLoss passed;not full-original guest-race proof.202/365 files;1438/2720 points:525 delivered/confirmed,1 local,8 approved,904 pending,1282 unreviewed. P143 format/line-only accumulation;no fresh build/publication claim.

[wkernel whole-file review](r1439-r1459-wkernel-review.json):R1439-R1459 complete;mirror166->133 net-33;398 selected lines match and#error unchanged. Ten points restored locally,eleven evidenced residuals pending. Profile absolute SYSTEM.INI versus renamed-file control proves native mapping;exact original module thunk faults3,final faults0 under controlled mappings.203/365 files;1459/2720 points:525 delivered/confirmed,11 local,8 approved,915 pending,1261 unreviewed. P143 cumulative net-35 now requires full delivery.

P143 final gates passed:cached x86/x64,11x2 publication,module filename boundary4,Winsock rundown323 and real Win16 Completion/Cancel/TaskLoss,ABI/menu-resource,isolated Full19,WOW boundary. Two mirrors net-35;520 shared selected lines match.203/365 files;1459/2720 points:536 delivered/confirmed,0 local,8 approved,915 pending,1261 unreviewed. Initial launcher failure preserved;final governance/Git accompany receipt.

[wkfileio complete review](r1460-r1466-wkfileio-review.json):R1460-R1466;mirror29->20 net-9;1222 selected line matches;#error unchanged. Original/final cache579/8 versus577/0;attributes before/final52/0. Two restored locally,five evidenced residuals;204/365files,1466/2720points. P143 delivered62da218dc;P144 final delivery pending.

P144 final gates passed:cached x86/x64,publish11x2,file cache577,attribute cleanup52,isolated Full19,WOW boundary/governance. Mirror29->20 net-9;1222 line matches.204/365files;1466/2720points:538 delivered,0 local,8 approved,920 pending,1254 unreviewed. Git accompanies receipt.

[wkgthunk review](r1467-r1468-wkgthunk-review.json):R1467-R1468 complete;mirror17->16 net-1;108 same-branch selected lines match;#error unchanged. Controlled originalC0000409,final matrix errors0. Format/line-only accumulates for next semantic batch.205/365files;1468/2720points:538 delivered,1 local,8 approved,921 pending,1252 unreviewed. P144 delivered76e57fb44.

[wkman review](r1469-r1496-wkman-review.json):R1469-R1496 source complete;mirror318->272 net-46;2238 selected common lines match;excluded branches unchanged. Three restorations local,25 minimal residuals pending. Paired KnownDLL21 checks pass;final batch delivery pending.206/365files;1496/2720points:538 delivered,4 local,8 approved,946 pending,1224 unreviewed.

[P145 frozen batch](batch-p145-checkpoint.json) and [line proof](batch-p145-line-check.json):two mirrors net-47;2346 common selected lines match;cached full/publication/tests/Git pending.

P145 final gates passed:cached x86/x64,publish11x2,KnownDLL21/lifecycle focused,isolated Full19,WOW boundary/governance. Mirror net-47;2346 line matches.206/365files;1496/2720points:542 delivered,0 local,8 approved,946 pending,1224 unreviewed. Git accompanies receipt.

[wkman header review](r1497-r1501-wkman-header-review.json):five points complete;mirror13->4 net-9;four original restorations local,one minimum cdecl boundary pending. Actual same-path object differs only COFF timestamp.207/365files;1501/2720points:542 delivered,4 local,8 approved,947 pending,1219 unreviewed. P145 delivered051fb3b27;P146 nonbehavioral batch accumulates.

[wkmem.c check](r1502-wkmem-c-review.json) and [wkmem.h check](r1503-wkmem-h-review.json):selected originals/lines confirmed;only approved #error subsets remain;no edits or fresh runtime claims.209/365files;1503/2720points:544 delivered/confirmed,4 local,8 approved,947 pending,1217 unreviewed. P146 net-9 unchanged.

P146 in-progress [R1509 finalizer reduction](r1509-finalizer-reduction-progress.json): duplicate incomplete-state predicate removed, mirror net-1 locally. Whole wmdisp32 review and final-code verification remain unfinished; no reviewed/restored count increment. Pre-edit exact-body171 scenarios passed, not final-code runtime evidence.

P146 coupled [R1513 exact-original WM32Create control](r1513-original-create-control.json): normal case both residual0; invalid-window original residual2 versus current0. This qualifies the early-return cleanup defect only; no file/point completion increment or full runtime claim.

P146 coupled [R1510 initialization reduction](r1510-create-initialization-progress.json): redundant nested-owner zero removed after all producer/caller inspection, mirror net-2 locally; remaining direct-CBT selector not yet disposition-complete. No completion count increment.

P146 [entry group R1504-R1510](r1504-r1510-entry-group-review.json): seven evidenced residuals queued, local mirror net-5; whole wmdisp32 incomplete.1510/2720points,209/365files;544 delivered/confirmed,4 local,8 approved,954 pending,1210 unreviewed. Final-code delivery pending.

P146 [CREATE R1511-R1515](r1511-r1515-create-review.json): five evidence-qualified residuals; standalone comment removed net-1.1515/2720points;209/365files;544 delivered/confirmed,4 local,8 approved,959 pending,1205 unreviewed. P146 local mirror net-15; whole-file gates pending.

P146 [CREATE owner review/correction](r1516-r1521-create-owner-review.json): R1513/R1518 restored exact original returns after all-caller cleanup proof; R1513 removed from pending, superseding isolated-function necessity claim. Five residuals qualified;171 scenarios passed.1521/2720points,209/365files;544 delivered/confirmed,6 local,8 approved,963 pending,1199 unreviewed. Local mirror net-23; whole-file gates pending.

P146 [text R1522-R1529](r1522-r1529-text-review.json): three original restorations, five minimized acquisition residuals; mirror net-9, unused autonomous STACK define removed.171 paired-header scenarios pass.1529/2720points,209/365files;544 delivered/confirmed,9 local,8 approved,968 pending,1191 unreviewed. P146 local mirror net-32; file/full delivery pending.

P146 [R1530-R1546 heap/MDI review](r1530-r1546-heap-mdi-review.json): group mirror net-11, wmdisp32 394->360;171-scenario paired control passed.1546/2720 reviewed;544 delivered,10 local,8 approved,984 pending,1174 unreviewed.Whole-file build/publication pending.

P146 [R1547 selector restoration](r1547-clipboard-selector-review.json): redundant table converter assignment removed;mirror net-1;1547/2720 reviewed,11 local restorations,1173 unreviewed.Final candidate verification pending.

P146 [R1548-R1554 clipboard ownership](r1548-r1554-clipboard-ownership-review.json):original paired controls compile0/run1;current171pass;one comment removed.1554/2720 reviewed;544 delivered,11 local,8 approved,991 pending,1166 unreviewed.Whole-file delivery pending.

P146 [R1555-R1556 DDE initiator](r1555-r1556-dde-initiate-review.json):restore original parameter authority;drop duplicate HWND state/local;mirror net-3;success-only deletion remains pending.1556/2720 reviewed,1164 unreviewed;final DDE and whole-file gates pending.

P146 [R1557-R1561 ACK/ADVISE review](r1557-r1561-dde-ack-advise-review.json):restore ACK node access and direct ADVISE alias API;remove empty conversion state;mirror net-12. Source-inapplicable poison test removed, ACK96pass;subsequent ADVISE edits await final verification.1561/2720 reviewed;544 delivered,14 local,8 approved,995 pending,1159 unreviewed.

P146 [R1562-R1572 DATA/POKE/EXECUTE](r1562-r1572-dde-data-poke-execute-review.json):keep actual child ownership,restore direct outer registration,share original failure returns;remove EXECUTE empty transaction.Mirror net-22;wmdisp32 394->321.Final caller verification pending.1572/2720 reviewed;544 delivered,16 local,8 approved,1004 pending,1148 unreviewed.

P146 [R1573-R1576 DROP/COPYDATA entry](r1573-r1576-drop-copydata-entry-review.json):remove redundant constructor initialization/invalid-input branch,mirror net-6;COPYDATA59/filter14/queue18 pass.1576/2720 reviewed;544 delivered,17 local,8 approved,1007 pending,1144 unreviewed;whole-file gates pending.

P146 [R1577-R1593 COPYDATA/WinHelp](r1577-r1593-copydata-winhelp-review.json):remove duplicate state/inner cleanup,restore original WinHelp assignment;mirror net-18,wmdisp32 394->297.90/90 source dispositions;line/full gates pending.1593/2720 reviewed;544 delivered,23 local,8 approved,1018 pending,1127 unreviewed.

- [P146 wmdisp32 compiler line proof](p146-wmdisp32-line-proof.json): 1267 shared selected lines, zero mismatch; final delivery pending.

- [P146 DDE caller controls](p146-dde-caller-controls.json): four exact functions, 80 cases per version, identical results; no guest proof.

- [P146 side gallery review](p146-side-gallery-review.json): original captures and P144 package identities verified.
- [P146 pending delivery checkpoint](batch-p146-checkpoint.json), [line checks](batch-p146-line-check.json).

- [P146 final message verification](p146-final-message-review.json): 171 cases; retained failed attempt and corrected caller context.

P146 final gates passed:cached x86/x64,publish11x2,focused controls,isolated Full19,WOW boundary/governance. Mirror net-106;1267 wmdisp32 line matches.210/365files;1593/2720points:567 delivered,0 local,8 approved,1018 pending,1127 unreviewed. Git accompanies receipt.

- [R1594-R1596 wmdisp32 header](r1594-r1596-wmdisp32-header-review.json): comment-only mirror12->8;80 selected lines match;two residuals await owner.

- [R1597 wmmalias.c](r1597-wmmalias-c-review.json):138 selected lines exactly original;10 exclusions retained;no source edit.

- [R1598 wmmalias.h](r1598-wmmalias-h-review.json):32 selected lines exactly original;exclusions retained;no source edit.

- [R1599 wmmedia.h](r1599-wmmedia-h-review.json):162 selected lines exactly original;exclusions retained;no source edit.

- [R1600 wmmstruc.h](r1600-wmmstruc-h-review.json):14 selected lines exactly original;exclusions retained;no source edit.

- [R1601-R1604 wmsg16 entry progress](r1601-r1604-wmsg16-entry-progress.json): local diff-5;whole-file audit and final verification outstanding.

- [R1605-R1607 wmsg16 text progress](r1605-r1607-wmsg16-text-progress.json): restore original declarations/call;remove redundant captures;whole-file verification pending.

- [R1608 WinHelp original failure contract](r1608-wmsg16-winhelp-review.json): local restoration; whole-file validation pending.

- [R1609 DDE initiate ownership](r1609-wmsg16-dde-initiate-review.json): minimized acquisition and paired release; whole-file validation pending.

- [R1610-R1612 DDE ownership](r1610-r1612-wmsg16-dde-ownership-review.json): ACK post ownership, synchronous fallback and redundant reset removal.

- [R1613-R1616 POKE failure ownership](r1613-r1616-wmsg16-poke-review.json): restore redundant failure exit; retain evidenced allocation rollback pending owner.

- [R1617-R1626 ADVISE/DATA](r1617-r1626-wmsg16-advise-data-review.json): unified failure flow and initialization; original empty DATA retained.

- [R1627-R1628 EXECUTE initialization](r1627-r1628-wmsg16-execute-review.json): original handle declaration and single record initialization.

- [R1629 original EXECUTE lock/allocation flow](r1629-wmsg16-original-execute-review.json): restore original zero-size/constant-handle path and paired native declaration.

- [R1630-R1635 EXECUTE release](r1630-r1635-wmsg16-execute-release-review.json): allocation ownership, single unlock and original empty-result propagation.

- [R1636 COPYDATA entry](r1636-wmsg16-copydata-entry-review.json): duplicate zeroing removed; direct dialog-message caller traced.

- [R1637-R1643 COPYDATA construction](r1637-r1643-wmsg16-copydata-review.json): original field reads and caller-owned exception cleanup.

- [R1644-R1651 final release points](r1644-r1651-wmsg16-release-review.json): caller cleanup, original text release and minimal owned-resource retirement.

- [P147 final wmsg16 compiler line proof](p147-wmsg16-final-line-proof.json): 1022 shared selected lines agree; cached full, publication, focused and Full19 passed.

- [P147 frozen source checkpoint](batch-p147-checkpoint.json) and [line gate](batch-p147-line-check.json): all gates passed; Git delivery accompanies this record.

- [R1652 wmsg16.h](r1652-wmsg16-header-review.json):14 selected declarations and logical lines identical;only two approved DEBUG exclusions.

- [R1653 wmsgbm.c](r1653-wmsgbm-review.json):selected button translation and logical lines match original;only DEBUG exclusion.

- [R1654-R1660 wmsgcb source review](r1654-r1660-wmsgcb-review.json):mirror66->61;82 retained lines and exact-combo verification passed.

- [P148 checkpoint](batch-p148-checkpoint.json) and [line proof](batch-p148-line-check.json):combo converter minimization;build,publish,focused,Full19/governance passed.

- [P148 counter-review](p148-counter-review.json):source freeze,ownership boundaries and synchronized pending residuals;all gates passed.

- [R1661Ã¢â‚¬â€œR1665](r1661-r1665-wmsgem-review.json): Edit converter whole-file review;comment/line-only diff-4,141 retained lines;P149 full delivery verified.

- [R1666Ã¢â‚¬â€œR1669](r1666-r1669-wmsglb-review.json): ListBox full-file review;comments-only diff-3;original consumer and line evidence,P149 delivered.

- [R1670](r1670-wmsgsbm-review.json): whole-file original selected code and line identity;no source edits.

- [R1671](r1671-wmtbl32-review.json): whole-file original selected code and line identity;no source edits.

- [R1672Ã¢â‚¬â€œR1686](r1672-r1686-wole2-review.json): whole-file OLE alias reduction97->42;native evidence and original line proof;P149 gates passed.

- [P149 checkpoint](batch-p149-checkpoint.json) and [line proof](batch-p149-line-check.json): frozen whole-file batch,all delivery gates passed.

- [P149 counter-review](p149-counter-review.json): source freeze,original contracts,ownership and ordered-register reconciliation.

- [R1687Ã¢â‚¬â€œR1708](r1687-r1708-wow32-review.json): complete wow32.c dispositions,net mirror diff-37;[line proof](batch-p150-line-check.json). P150 full delivery verified.

- [P150 receipt](batch-p150-checkpoint.json) and [counter-review](p150-counter-review.json): cached build,publish11x2,focused andFull19 passed.

- [R1709](r1709-wow32-def-review.json): ticket export ABI and placement;original CRLF restored locally,no normalized diff change.

- [R1710 wow32.h](r1710-wow32-header-review.json):25 approved exclusion blocks only;102 selected original logical lines match actual MSVC preprocessing;no source change.227/365 files;1710/2720 points,1010 remain.

- [R1711Ã¢â‚¬â€œR1720 fax investigation](r1711-r1720-fax-investigation.json): original/current/minimized exact-function contrasts;candidate diff41->30,not applied or delivered;full-file line/binding review remains.

- [R1711Ã¢â‚¬â€œR1720 complete source review](r1711-r1720-fax-review.json):mirror41->30(-11),698 logical lines match;3 local restorations,7 minimized/necessary residuals;P151 delivery pending.

- [P151 receipt](batch-p151-checkpoint.json):cached full,publish11x2,final fax135,Full19/shared boundary/governance passed;mirror diff net-11;Git accompanies receipt.

- [R1721Ã¢â‚¬â€œR1764 hook investigation](r1721-r1764-hook-investigation.json):fixed baseline and complete diff;actual wrapper/scheduler chain and fixture limitations;not a completed disposition.

- [P152 line proof](batch-p152-line-check.json) and [delivery checkpoint](batch-p152-checkpoint.json):wowhooks mirror151->125;1048 original selected lines match;cached/full delivery pending.

- [R1765 header review](r1765-wowhooks-header-review.json):two TLS externs pending owner;format-only mirror4->2;original line36 suffix exact;local accumulated change.

- [R1766/R1767 table review](r1766-r1767-wowtbl-review.json):only existing profiling exclusions;actual product preprocessing verifies selected original code and lines;no source change.

- [R1768-R1771 parameter map](r1768-r1771-parameter-map-review.json):original lookup restored;shared rollback;mirror106->94;54 focused scenarios and289 lines pass;delivery pending.

- [P153 receipt](batch-p153-checkpoint.json) and [line proof](batch-p153-line-check.json):cached full,publish11x2,parameter54,Full19 and governance passed;mirror net-14;supersedes the pending-delivery notes above.

- [R1772-R1774 DC cache](r1772-r1774-dc-cache-review.json): exact original C/header restored;three coupled callers;mirror diff -14;native release controls passed;final delivery pending.

- [P154 receipt](batch-p154-checkpoint.json) and [line proof](batch-p154-line-check.json):cached full,publish11x2,DC/window-borrow and Full19 passed;mirror net-14;supersedes pending-delivery notes above.

- [R1775 resource review](r1775-resource-review.json):only authorized DEBUG exclusion;365actual preprocessed retained lines match;no source change.

- [R1776 wsdata lines](r1776-wsdata-lines-review.json):22 shifted statements corrected;1030actual retained lines match;line-only local change;remaining file points precede next product batch.

- [R1777-R1778 Winsock](r1777-r1778-winsock-review.json):allocation-result and task-ownership source evidence;minimal unchanged residuals pending owner decision;whole file not complete.

- [R1779-R1791 Winsock](r1779-r1791-winsock-review.json):six constructors/three converters checked;qualified residuals pending;redundant comment removed;mirror231->228;file review continues.

- [R1792 cancel comment](r1792-cancel-comment-review.json):original comment content restored;mirror-3;no execution change;whole-file review continues.

- [R1793-R1798 cancellation](r1793-r1798-cancel-review.json):reuse original native removal;delete duplicate alias removal;batch mirror-64;final lines1030/0,focused336/0;delivery pending. Coupled R1438 residual remains pending.

P155 failed-run follow-up: [TaskLoss callback boundary](p155-taskloss-stack-review.md).

- [P155 final receipt](batch-p155-checkpoint.json):mirror net-59;cached,publish11x2,focused353,five Win16 modes,Full19 passed. Approved completion synchronization retained for revisit.

- [R1799â€“R1800 whole-file review](r1799-r1800-wsext-review.json):comments restored;mirror19->8;residual decisions retained.

- [R1801â€“R1809 whole-file review](r1801-r1809-wspool-review.json):original GetPrn16 restored;mirror17->6;three proven residuals remain pending.

- [P156 receipt](batch-p156-checkpoint.json) and [line proof](batch-p156-line-check.json):mirror-22;cached,publish11x2,focused72,raw-spool witness,Full19 passed.

- [R1810-R1814 review](r1810-r1814-wstruc-review.json), [P157 checkpoint](batch-p157-checkpoint.json), [line proof](batch-p157-line-check.json): whole-file source complete;delivery pending.

- [P157 receipt](batch-p157-checkpoint.json):mirror-4;cached,publish11x2,DDE/COPYDATA focused,Full19 passed.

- [R1815 unchanged-source confirmation](r1815-wsubcls-review.json):141 selected lines match;no source change or repeated runtime test.

- [R1816-R1823 call-chain investigation](r1816-r1823-wuclass-investigation.json): source leads only;not a completed audit or pending-owner decision.

- [R1816-R1823 completed file review](r1816-r1823-wuclass-review.json) supersedes investigation leads:mirror51->48,comments only;405 selected lines match;seven residual decisions.

- [R1824-R1843 clipboard investigation](r1824-r1843-wuclip-investigation.json): original open-time retirement chain and deterministic after-close generation counterexample; ongoing whole-file investigation, not final dispositions or product delivery.

- [R1824-R1843 completed file review](r1824-r1843-wuclip-review.json), [P158 checkpoint](batch-p158-checkpoint.json), [line proof](batch-p158-line-check.json): mirror144->90;original open-time cache retirement restored;epoch56/0 and ownership24/0;delivery pending.

- [P158 delivered receipt](batch-p158-checkpoint.json): mirror net-57;cached,publish11x2,epoch56/0,ownership24/0,real Win16 Clipboard transfer and Full19 passed.

- [P159 wucomm review](r1844-r1845-wucomm-review.json): selected original implementation confirmed;DEBUG exclusions retained;header original CRLF restored.

- [P160 dialog review](r1846-r1854-wudlg-review.json), [line proof](batch-p160-line-check.json), [delivery checkpoint](batch-p160-checkpoint.json): original-entry controls and minimal call-owned cleanup.

- [P161 USER initialization review](r1855-r1859-wuman-review.json), [checkpoint](batch-p161-checkpoint.json): original declaration/format restoration and bounded guest address publication evidence.

- [P162 message entry review](r1860-r1894-wumsg-review.json), [line proof](batch-p162-line-check.json), [checkpoint](batch-p162-checkpoint.json): one invocation-mode owner and one failed-post rollback.

- [P163 message header review](r1895-wumsg-header-review.json), [checkpoint](batch-p163-checkpoint.json): comment minimization, original logical-line proof and latest RDP Clipboard witness.

- [P164 USER thunk review](r1896-r1905-wuser-review.json), [line checks](batch-p164-line-check.json), [checkpoint](batch-p164-checkpoint.json): original bitmap restoration and GetDC minimization;cached,publish11x2,resource suite and Full19 passed.

- [P165 USER31 review](r1906-r1911-wuser31-review.json), [line checks](batch-p165-line-check.json), [checkpoint](batch-p165-checkpoint.json): GetDCEx and public Clipboard conversion minimization;cached,publish11x2,window-borrow and Full19 passed.

- [P166 USER client header review](r1912-wusercli-review.json), [checkpoint](batch-p166-checkpoint.json): selected original content and lines confirmed;CRLF restored only.

- [P167 timer entry review](r1913-wutmr-review.json), [checkpoint](batch-p167-checkpoint.json): bounded callback entry evidence and comment reduction.

- [P168 window review](r1914-r1930-wuwind-review.json), [checkpoint](batch-p168-checkpoint.json), [lines](batch-p168-line-check.json): whole-file original-first review;cached,publish11x2,focused and Full19 passed.

- [P169 XMS header review](r1931-xms-header-review.json), [checkpoint](batch-p169-checkpoint.json): original selected subset confirmed;no source edits.

- [P170 A20 review](r1932-xmsa20-review.json), [checkpoint](batch-p170-checkpoint.json): original selected source confirmed;no source edits.

- [P171 XMS block review](r1933-r1941-xmsblock-review.json), [checkpoint](batch-p171-checkpoint.json), [lines](batch-p171-line-check.json): original guest preconditions and reduced numeric memory boundary;delivery pending.

P172: [XMS dispatch review](r1942-xmsdisp-review.json), [receipt](batch-p172-checkpoint.json); original CRLF restored; P171 product verification reused.

P173: [XMS mover review](r1943-r1944-xmsmemr-review.json), [checkpoint](batch-p173-checkpoint.json), [line proof](batch-p173-line-check.json): original mover restored, mirror30->4 (-26);cached,publish11x2,focused,final guest relocation,Full19 and governance passed;Git accompanies receipt.

P174: [CSR capture review](r1945-r1951-csrutil-review.json), [receipt](batch-p174-checkpoint.json): mirror650->622(-28),original header/comment cleanup;ABI residuals pending;P173 product evidence reused.

P175: [Original read-probe review](r1952-r1956-probe-review.json), [receipt](batch-p175-checkpoint.json): mirror125->117(-8);conditional order and lines repaired;P173 product evidence reused.

P176: [Environment restoration review](r1957-r1958-environ-review.json), [checkpoint](batch-p176-checkpoint.json), [lines](batch-p176-line-check.json): original environment names/includes restored;cached,publish11x2,focused,Full19,RDP Clipboard18 passed;Git accompanies receipt.

P177: [Error mapping line review](batch-p177-checkpoint.json): original body retained;both #error guards preserved;logical-line/EOL repair only;P176 product evidence reused.

P178: [Time package boundary and cleanup](batch-p178-checkpoint.json): five points pending source-boundary decision;comment/line/EOL cleanup only;P176 product verification reused.

P179: [BaseClient VDM boundary review](batch-p179-checkpoint.json):13 points,whole-file891->870;12 residual decisions,one confirmed exclusion;P176 product verification reused.

P180: [BaseSrv header review](batch-p180-checkpoint.json):7->4;two qualified residual boundary decisions;actual dispatch controls and original line check;P176 product evidence reused.

P181: [BaseSrv VDM whole-file review](batch-p181-checkpoint.json):93->84;notification/ABI controls;line proof;cached,publish and final regression.

P182: [BaseSrv VDM header review](batch-p182-checkpoint.json):4->2;two minimal residual declarations;P181 product evidence reused.

P183: [NetAPI buffer boundary review](batch-p183-checkpoint.json):original comments restored;public allocator boundary pending;P181 evidence reused.

P184: [NetLib allocation subset review](batch-p184-checkpoint.json):original selected content and7#error;four directive line corrections;P181 evidence reused.

P185: [RAP conversion subset review](batch-p185-checkpoint.json):original selected content and7#error;six directive line corrections;P181 evidence reused.

P186: [Original NetMessage restoration](batch-p186-checkpoint.json):533->7(-526);one header decision;cached,publish11x2,Full and DOS message probe. Shared Clipboard recheck limitation retained.

P187: [Original server API restoration](batch-p187-checkpoint.json):1067->190(-877);two evidenced header/provider decisions;focused37,cached,publish11x2,Full.

P188: [NetService original-body confirmation](batch-p188-checkpoint.json):4 unchanged;one evidenced header decision;no source change;P187 product evidence reused.

P189: [NetUse byte/line confirmation](batch-p189-checkpoint.json):ordinary2 unchanged,raw3->2;redundant#line removed;one header decision;P187 evidence reused.

P190: [NetWksta original-byte confirmation](batch-p190-checkpoint.json):2 unchanged;one header decision;no source change;P187 product evidence reused.

P191: [XACTSRV helper original-byte confirmation](batch-p191-checkpoint.json):2 unchanged;one header decision;no source change;P187 product evidence reused.

P192: [Unicode helper original-byte confirmation](batch-p192-checkpoint.json):2 unchanged;one header decision;no source change;P187 product evidence reused.

P193: [Unicode header line mapping](batch-p193-checkpoint.json):18 excluded unchanged,ordinary0;7 context comparisons and ANSI negative control;P187 evidence reused.

P194: [DLC header line mapping](batch-p194-checkpoint.json):75 excluded unchanged,ordinary0;5 context comparisons and two excluded-profile negative controls;P187 evidence reused.

P195: [Serial header line mapping](batch-p195-checkpoint.json):10 excluded unchanged,ordinary0;3 context comparisons and two excluded-profile negative controls;P187 evidence reused.

P196: [Video header line mapping](batch-p196-checkpoint.json):66 excluded unchanged,ordinary0;6 context comparisons and two excluded-profile negative controls;P187 evidence reused.

P197: [Alignment header restoration](batch-p197-checkpoint.json):mirror5->4,raw8->7;9 contexts and both negative diagnostics checked. Shared Clipboard retry remains failed.

P198: [SMB get/put header mapping](batch-p198-checkpoint.json):246 excluded diff unchanged;14 contexts and3 negative profiles;original CRLF preserved;conditional lines restored.

P199: [SMB macro header mapping](batch-p199-checkpoint.json):6 excluded diff unchanged;7 contexts and NO_PACKING negative control;endif99 restored.

P200: [SMB types header mapping](batch-p200-checkpoint.json):12 excluded diff unchanged;raw26->24;14 contexts and4 negative profiles;original boundary lines restored.

P201: [VDM header mapping](batch-p201-checkpoint.json):26 excluded diff unchanged;8 actual contexts/4 negative profiles;original boundary lines restored without monitor behavior changes.

P202: [Debug header mapping](batch-p202-checkpoint.json):6 excluded diff unchanged;raw9->8;6 contexts/DBG negative;original conditional line and CRLF restored.

P203: [Network debug header mapping](batch-p203-checkpoint.json):380 excluded diff unchanged;20 contexts/3 negative profiles;original conditional lines and CRLF restored.

P204: [Netlib header mapping](batch-p204-checkpoint.json):21 excluded diff unchanged;raw33->25;14 contexts/4 negative controls;original line mapping and CRLF restored.

P205: [RAP header mapping and RDP recheck](batch-p205-checkpoint.json):31 excluded diff unchanged;19 contexts;original boundary mapping restored;P187 shared Clipboard18 passed.

P206: [RAP debug header mapping](batch-p206-checkpoint.json):8 excluded diff unchanged;raw12->10;2 actual contexts and DBG negative;original boundary mapping restored.

P207: [winddi header mapping](batch-p207-checkpoint.json):41 excluded diff unchanged;raw44->43;actual WOW32 fax context and _X86_ negative;original boundary mapping restored.

P208: [Capture header minimization](batch-p208-checkpoint.json):6->4 mirror diff;two native fields pending with actual allocator alignment controls;original comments/lines restored.

P209: [NT executive declaration restoration](batch-p209-checkpoint.json):1777->1188;11 restored/confirmed,7 qualified pending;cached/publish/isolated Full passed.

P209 cumulative mirror-diff evidence: [cumulative-diff-p209.json](cumulative-diff-p209.json). Fixed 365-file scope: 119913 -> 113077, net reduction 6836; excludes diagnostic #line, includes #error subset differences. Not the complete mirror census.

P210: [LPC header original mappings](batch-p210-checkpoint.json):13 excluded difference unchanged;raw24->17;28 actual contexts and3 negatives;P209 product reused,cumulative mirror reduction6836 unchanged.

P211: [Process/thread declaration restoration](batch-p211-checkpoint.json):1268->363(-905);original native alert binding;3 restored/3 qualified pending;cached/publish/Full passed. [Cumulative witness](cumulative-diff-p211-delta.json):119913->112172,net7741;365 fixed files.

P212: [Registry declaration restoration](batch-p212-checkpoint.json):469->16(-453);two minimized qualified pending points;cached,publish11x2,focused registry/Full passed. [Cumulative witness](cumulative-diff-p212-delta.json):119913->111719,net8194;365 fixed files.

P213: [RTL declaration restoration](batch-p213-checkpoint.json):ntrtl6101->253(-5848),nturtl-19,precomp-1;42 restored/confirmed,2 qualified pending. Final cached,publish11x2,focused native/time/RTL and Full passed. [Cumulative witness](cumulative-diff-p213-delta.json):119913->105851,net14062;365 fixed files.

P214: [NTURTL restoration](batch-p214-checkpoint.json):nturtl1021->29(-992),precomp15->14(-1);2 restored,1 qualified pending;original common/WOW types and declarations unified. Final cached,publish11x2,path/RTL/command and Full pass. [Cumulative witness](cumulative-diff-p214-delta.json):119913->104858,net15055;fixed365 files.

P215: [VDM debug header original line mapping](batch-p215-checkpoint.json):no non-exclusion diff;three errors retained;core140 unchanged,raw146->142. Four actual contexts and2 negatives pass;P214 product reused. [Cumulative witness](cumulative-diff-p215-delta.json):net15055 unchanged.

P216 initial investigation, superseded: [BUTTON original/DC lifetime investigation](p216-button-investigation.json). The intermediate87->63 proposal was not applied;the complete-file review and final87->19 delivery below replace that provisional conclusion.

P216: [whole BUTTON review and controls](p216-button-review.json):87->19(-68),22 verified/published restorations and12 minimized residual decisions;cached,11x2,29 WOW fixtures andFull19 passed. Earlier investigation is superseded by this complete-file review.

P216: [delivery receipt](batch-p216-checkpoint.json),[cumulative witness](cumulative-diff-p216-delta.json):net15123.12 residual points remain pending owner decisions.

P217: [cldib whole-file review](p217-cldib-review.json):7->2(-5),one restored diagnostic point and one minimized header substitution pending owner;cached,publish11x2,bitmap focused/Full19 passed. [Receipt](batch-p217-checkpoint.json). [Cumulative witness](cumulative-diff-p217-delta.json):net15128.

P218: [clglobal whole-file review](p218-clglobal-review.json):133->39(-94),coupled user.h -20;three restored points,four minimized residual decisions;cached,publish11x2,31 focused/Full19 passed. [Receipt](batch-p218-checkpoint.json). [Cumulative witness](cumulative-diff-p218-delta.json):net15242.

P220 source-stage record (superseded by the delivery receipt below): [client restoration and verification state](p220-client-progress.json). Mirror diff3664->3572(-92);initial verification state retained for traceability.

P220: [whole-file dispositions and final source controls](p220-client-review.json). Four restorations,seven minimized residual decisions;final product delivery recorded below.

P220: [delivery receipt](batch-p220-checkpoint.json), [cumulative diff](cumulative-diff-p220-delta.json). Cached,publish11x2,32 focused,Full19 passed;four restored points,seven residual decisions.

P221: [clmenu whole-file review](p221-clmenu-review.json). Original parser/task predicate restored;396->343(-53);runtime delivery pending.

P221: [delivery receipt](batch-p221-checkpoint.json), [cumulative diff](cumulative-diff-p221-delta.json). Cached,publish11x2,menu-resource/32 controls,Full19 passed;12 restored points,2 residual decisions.

P222: [DPMI/DOS4GW repair receipt](../dos4gw-pif-sti-20261115/main-production-candidate.json). Supplemental +74 mirror difference;cached,publish11x2,CCPU58,Full and Win3.1 four profiles passed. Original2720-point census unchanged;nextR2175.

P223: [clmsg review](p223-clmsg-review.json), [original restoration controls](p223-clmsg-investigation.json). Whole-file1608->1535(-73);delivery pending.

P223: [delivery receipt](batch-p223-checkpoint.json). Cached,publish11x2,32 controls,Full20 and governance passed;clmsg1608->1535(-73);six restored points,two residual decisions.

P224: [clres investigation and build-only candidate](p224-clres-investigation.json);not production delivery.

P224: [whole-file clres review](p224-clres-review.json);source applied,full delivery pending.

P224: [delivery receipt](batch-p224-checkpoint.json). Cached,publish11x2,32 controls,Full20 and governance passed;clres4129->3437(-692);four restored points,five residual decisions.

P225: [combo line-only review](p225-combo-review.json);local,batch delivery pending.

P225: [dlgbegin active investigation](p225-dlgbegin-investigation.json);not completed audit or production repair.

P225: [dlgbegin complete source review](p225-dlgbegin-review.json);85->44,final-code delivery pending.

P225: [delivery receipt](batch-p225-checkpoint.json). Cached,publish11x2,32 controls,Full20 and governance passed;dlgbegin85->44(-41),combo line metadata restored.

P226 active: [dlgend review](p226-dlgend-review.json). Two format points restored locally; functional review and delivery remain pending.

P226: [delivery receipt](batch-p226-checkpoint.json). Cached,publish11x2,focused32,Full20 and governance passed;dlgend61->42(-19).

P227 active: [dlgmgr ordered review](p227-dlgmgr-review.json);partial file,not delivered.

P227: [delivery receipt](batch-p227-checkpoint.json). Cached,publish11x2,focused32,Full20 and governance passed;dlgmgr918->304(-614);native initialization and two guard negative controls.

P228 active: [dlgmgr2 ordered review](p228-dlgmgr2-review.json);partial file,not delivered.

P228: [delivery receipt](batch-p228-checkpoint.json). Cached,publish11x2,focused32,Full20 and governance passed;dlgmgr2 183->67(-116);original navigation and ANSI return semantics restored.

P229: [dlgmgrc review](p229-dlgmgrc-review.json);original order/format restored,151->88(-63);verified and published.

P229: [dmmnem review and probes](p229-dmmnem-review.json);16/16 reviewed;verified and published.

P229 final source review: dlgmgrc151->88, dmmnem78->27, net reduction114.
R2308 guard-removal candidate was rejected by actual original navigation
evidence;the existing retirement guard remains. See P229 receipt below.

P229: [delivery receipt](batch-p229-checkpoint.json). Cached,publish11x2,focused32,Full20 and governance passed;two-file net mirror reduction114;R2308 retirement guard retained on actual navigation evidence.

P230: [delivery receipt](batch-p230-checkpoint.json). Drawtext239->137(-102);original Unicode bodies restored;ANSI native boundary pending. Cached,publish11x2,focused32,Full20 and governance passed.

P231 source-only reviews (delivery pending):

- [editml](p231-editml-review.json): original selected bodies confirmed; logical-line repair.
- [editsl](p231-editsl-review.json): original selected bodies confirmed; logical-line repair.
- [edmlonce](p231-edmlonce-review.json): original selected bodies confirmed; logical-line repair.
- [edmlrare](p231-edmlrare-review.json): original selected bodies confirmed; logical-line repair.

- [edslrare](p231-edslrare-review.json): exclusion-only line correction.
- [globals](p231-globals-review.json): original declaration restoration;delivery pending.

- [P231 delivery](batch-p231-checkpoint.json): original globals declarations and EDIT line mapping;cached,publish11x2,focused32,Full20.

- [P232 handles source review](p232-handles-review.json): line/comment cleanup;product compile and equivalent preprocessing;batch delivery pending.

P232: [hdata whole-file review](p232-hdata-review.json) restores original copy functions after finite public API binding proof;mirror593->442(-151). Together with handles local batch-158. Cached build/publication/tests/Git pending;2334/2720 points,326/365 files source-reviewed.

- [P232 delivery](batch-p232-checkpoint.json): original DDE copy functions and handles metadata;cached,publish11x2,focused33,Full20;mirror delta-158.

P233 source audit: [lboxctl2](p233-lboxctl2-review.json), [lboxctl3](p233-lboxctl3-review.json) retain only exclusion-policy diffs with original selected-line identity;no source edits. R2337 investigation follows.

P233 [lboxrare review](p233-lboxrare-review.json): original HeapFree BOOL counterexample,current/reset-repeat pass. Remove one comment;retain one polarity correction pending owner. Local mirror delta-1;delivery pending.

- [P233 mngrayc](p233-mngrayc-review.json): original wrappers and source structure restored;110->29;residual ownership/correction decisions.

- [P233 delivery](batch-p233-checkpoint.json): original DrawState wrappers and source layout;cached,publish11x2,focused33,Full20;mirror delta-82.

- [P234 ntstubs review](p234-ntstubs-review.json): original source order/metadata restored;five compile contexts and private USER boundary evidence;mirror delta-119.

- [P234 delivery](batch-p234-checkpoint.json): ntstubs original ordering and metadata;cached,publish11x2,focused33,Full20;mirror delta-119.

- [P235 rtlinit review](p235-rtlinit-review.json): original large capture restored;single product object;mirror91->16(-75);delivery verified below.

- [P235 delivery](batch-p235-checkpoint.json): rtlinit original large capture and single product object;cached,publish11x2,focused34,Full20;mirror delta-75.

- [P236 usercli investigation](p236-usercli-investigation.json): pinned whole-file audit,104 observed dependency edges and paired header-restoration compiles;no production edit or completion claim.

- [P236 isolated usercli candidate](p236-usercli-candidate.json): mirror2170->562(-1608);104 compile contexts and7 lifecycle compile variants pass;not product delivery. Detailed evidence remains in the P236 investigation.

- [P236 usercli final source review](p236-usercli-review.json): original declaration recovery and two-phase consolidation;mirror2170->556(-1614);delivery pending.

- [P236 delivery](batch-p236-checkpoint.json): usercli original declaration recovery and shared inclusion phases;cached,publish11x2,focused34,Full20;mirror delta-1614.

- [P237 winmgrc investigation](p237-winmgrc-investigation.json): individual original restoration compile/link controls;negative results are not product-test passes.

- [P237 winmgrc source review](p237-winmgrc-review.json): restored pragma and compatibility getter body;mirror517->500(-17),residual native/private boundaries pending owner;delivery receipt below.

- [P237 delivery](batch-p237-checkpoint.json): winmgrc original pragma/getter restored;cached,publish11x2,focused34,Full20;mirror delta-17.

### P238 whole-file audit

- [Source/ABI and real compile-link controls](p238-wow-investigation.json).
- [Final local subset and line review](p238-wow-review.json): client/wow.c, R2427â€“R2436, mirror1059â†’1046(âˆ’13); delivery verified below.

- [P238 delivery](batch-p238-checkpoint.json): wow.c selected original subset restored;cached,publish11x2,focused34,Full20;fixed mirror metric1059->1046(-13).

### P239 whole-file audit

- [Review](p239-ntcftxt-review.json) and [counter-review evidence](p239-ntcftxt-investigation.json): R2437-R2448 ntcftxt.h;mirror1373->1205(-168),421 original logical rows,23 error bodies. Two display wrappers restored; six residual native-boundary/cleanup decisions. Delivery verified below;Git delivery accompanies receipt.

- [P239 delivery](batch-p239-checkpoint.json): ntcftxt.h selected original subset restored;cached,publish11x2,focused34,Full20;fixed mirror metric1373->1205(-168).

### P240 whole-file audit

- [Review](p240-ntsend-review.json) and [counter-review](p240-ntsend-investigation.json): R2449-R2459 ntsend.h restored byte-identical to original;mirror212->0(-212);six actual preprocess contexts unchanged apart from whitespace. Delivery verified below;Git delivery accompanies receipt.

- [P240 delivery](batch-p240-checkpoint.json): ntsend.h selected original subset restored;cached,publish11x2,focused34,Full20;fixed mirror metric212->0(-212).

### P241 whole-file candidate

- [Investigation](p241-user-investigation.json): user.h R2460-R2483;original-order candidate and duplicate binding removal applied;mirror2666->265(-2401). Isolated110 contexts and full WOW link pass;cached build/publication/runtime/Git pending.

- [P241 delivery](batch-p241-checkpoint.json): user.h original order restored;mirror2666->265(-2401);cached,publish11x2,focused34,Full20 passed;14 residuals pending owner;Git delivery accompanies receipt.

- [P242 class audit](p242-class-review.json): R2484-R2499;mirror1440->1394(-46);three selected token streams identical;format-only accumulation,delivery pending.

- [P242 cleanup audit](p242-cleanup-review.json): R2500-R2525;mirror82->68(-14);actual object code/data/relocations unchanged;source comparator repaired with two negative controls;accumulated batch -60,delivery pending.

- [P242 exitwin audit](p242-exitwin-review.json): R2526-R2538;mirror502->498(-4);actual optimized code/data/relocations unchanged;64 original rows and LATER frame mapped;accumulated -64,delivery pending.

- [P242 per-file difference-block metrics](p242-diff-point-metrics.json): adds before/after block counts without replacing line/register/delivery metrics. Restored original islands can split one block into two;getset source review and equivalent cleanup are complete;delivery remains pending.

- [P242 getset audit](p242-getset-review.json): R2539-R2551;mirror727->724(-3),blocks12->11;136 original rows,optimized code/data/relocations unchanged;batch -67,delivery pending.

- [P242 handtabl audit](p242-handtabl-review.json): R2552-R2565;mirror2169->2166(-3),blocks33->34;original table/condition lines checked,optimized code/data/relocations unchanged;batch -70,delivery pending.

- [P242 inctlpan audit](p242-inctlpan-review.json): R2566-R2581;mirror1065->1052(-13),blocks8->5;static table restored,extern/separate object removed;batch -83,full delivery required.

- [P242 delivery](batch-p242-checkpoint.json): six mirrors -83;static table restoration and equivalent cleanup;cached,publish11x2,focused34,Full20 passed;Git delivery accompanies receipt.

- [P243 input review](p243-input-review.json): R2582-R2597;mirror5890->5814(-76),blocks25->49;original frames/lines restored;14 qualified residuals,2 original confirmations;format-only local batch,not delivered.

- [P243 miscutil review](p243-miscutil-review.json): R2598-R2603;mirror204->198(-6),blocks3->3;original body/line proof;4 residuals pending owner,2 restored/confirmed;format-only local batch.

- [P243 ntstubs review](p243-ntstubs-review.json) and [non-error diff](p243-ntstubs-non-error.diff):73 excluded bodies normalized for residual review;whole-file complete;mirror10575->10014(-561),raw blocks19->155;non-error +2/-9237;6residuals,1original confirmation;not delivered.

- [P243 profile review](p243-profile-review.json): R2611-R2617;mirror982->882(-100),blocks19->8;original DWORD reader restored,partial R2614 remains pending;full cumulative delivery required.

- [P243 delivery](batch-p243-checkpoint.json): four files,mirror -743;cached x86/x64,publish11x2,focused36,Full20,boundary/governance passed;R2614 residual remains pending owner.

- [P244 queue in progress](p244-queue-in-progress.json): R2618 header reduction applied;remaining file points and delivery not complete;no audited-count increment.

- [P244 delivery](batch-p244-checkpoint.json): queue mirror -63;original parser restored;final cached/publish11x2;focused36/task-order/Full20 with verified final line-only package equivalence;R2627 partial remains pending.

- P245 local whole-file review: [random.c evidence](p245-random-review.json); verified with [P245 receipt](batch-p245-checkpoint.json);Git delivery accompanies receipt.

- P246 [server.c review evidence](p246-server-in-progress.json): original initializer restored;cached full build,publication and runtime gates passed;Git delivery accompanies receipt.

P246 server.c: [review](p246-server-in-progress.json), [delivery](batch-p246-checkpoint.json);original initializer restored,mirror diff reduced269;four residual points pending owner.

- P247 [taskman.c review](p247-taskman-review.json): ordered source audit and original restoration in progress;not delivered.

P247 taskman.c: [review](p247-taskman-review.json), [delivery](batch-p247-checkpoint.json);mirror diff reduced50,lexical blocks reduced15;six residual points pending owner.

P248 userk.h ordered source review: [working evidence](p248-userk-review.json);not verified or delivered.

Owner-requested text-block growth follow-up: [candidate inventory](p248-block-growth-followup.json).
[ntstubs format experiment](p248-ntstubs-format-review.json) was rejected and
withdrawn:source is byte-identical to delivered HEAD;the claimed2897-line
local reduction is withdrawn. P248 whole-file gate remains.

P248 [remaining declaration inventory](p248-userk-declarations.json): prior
missing-type classification withdrawn where based solely on expanded identifier
absence;69 complete original declarations restored with logical-line checks.
Whole-file audit, compilation and delivery remain incomplete.

[Fragmentation review](p248-fragmentation-review.json) records the rejected
pre-withdrawal candidates, not current source metrics. The ntstubs withdrawal
is authoritative in its experiment record.

[P248 global count](p248-global-diff-count.json): all365 fixed re-audit files
recomputed from pinned originals and Git baseline/HEAD/current bytes.
Blocks5297->4855 delivered->5099 local;changed lines119913->96662->93963.
The current userk candidate still adds244 blocks over delivered HEAD;
it is not accepted for delivery. Register2720 IDs are a separate measure.

[Userk isolated-format withdrawal](p248-userk-island-withdrawal.json): rejected
logical-line sorting candidate never applied;physical-order withdrawal preserves
code tokens and errors. Current267 blocks/2641 changed lines;whole file unfinished.

[Complete message data units](p248-userk-message-units.json): four original
structures and ISM flags restored;38 logical rows checked;blocks267 unchanged,
changed lines2641->2605. Final whole-file compile remains outstanding.

[Completed constant groups](p248-userk-constant-groups.json): nine unjustified
omissions restored in allocation/message/profile constant groups;diff blocks
267->258 and changed lines2605->2596. Source evidence,not final build validation.

[Original critical-section assertion group](p248-userk-critical-check.json):
restore empty release CheckCritIn and remove scheduler duplicate;mirror -1line,
-1block,autonomous -1line. Final whole-file validation remains pending.

[Complete original type units](p248-userk-complete-types.json): five units,80 original rows restored and logical lines checked;userk2595->2515,blocks257->255. Whole file remains open;source-only,not compiled or delivered. Global current93963->93883,blocks5099->5097;2720 register points unchanged.

[Public declaration units](p248-userk-public-units.json) and [selected TL structure](p248-userk-thread-lock-type.json):17 original rows restored with logical-line proof,40 #error retained;userk2515->2498,blocks255->243. Source-only;file audit and final build remain outstanding. Global current93866 lines/5085 blocks;delivered totals unchanged.

[Complete macro groups](p248-userk-macro-units.json):88 original rows restored,40 #error retained;userk2498->2410,blocks243->230. Exact-name matching corrected prefix confusion. Source-only;no batch build. Current global93778 lines/5072 blocks;delivered baseline unchanged.

P248 public constants/type follow-up is recorded in [current review](p248-userk-review.json):17 original rows restored,HEVENT duplicate removed from binding;userk2410->2393,blocks230->229,40 #error unchanged. Source-only. Current global93761 lines/5071 blocks;no new completed file or delivered point.

P248 [whole-file source line check](p248-userk-review.json):corrected two assignment-line offsets by moving existing #line;2365 retained original nonempty rows now match,including repeated text. Six added hook/guard rows listed;105 original conditional directives match after excluded-body normalization;all40 excluded bodies contain only #error and optional #line. No semantic or measured diff change;actual compiler verification remains pending.

P248 [pool macro recovery](p248-userk-review.json):restored original quota allocation/free macros;existing native bindings and two fault-injection providers now bind ExAllocatePoolWithQuota/ExFreePool without new functions or state. Mirror2393->2390,blocks229->227;2368 original nonempty source rows mapped. Source-only;allocation failure tests and delivery pending.

P248 [current-owner macro recovery](p248-userk-review.json):restored original PpiCurrent/PtiCurrent;existing TLS/context expressions bind W32GetCurrentProcess/gptiCurrent with no new state/functions. Mirror2390->2387,blocks227->225;2371 original nonempty source rows mapped. Global93755 lines/5067 blocks. Source-only;whole file remains open.

P248 [public prototype group](p248-userk-review.json):ten declaration/macro units plus their five original separators restored;mirror2387->2372,blocks225 unchanged. Rejected un-applied formatting candidate would have made226 blocks.2381 original nonempty rows map correctly;40 errors unchanged. Source-only;current global93740 lines/5067 blocks.

P248 [retained macro groups](p248-userk-review.json):restored original abs and Atom macro units;no actual binding changes. Mirror2372->2360,blocks225->224;current file5294->2360(-2934),13->224blocks(+211),still open. Global93728lines/5066blocks across fixed365 files. Source logical-line check passes;40 excluded errors unchanged. No compilation/publication/test/commit/push yet.

P248 [header/process macro group](p248-userk-review.json):original68-73,108-109 restored as complete units;SDK include dependency already present,no new shim or runtime call. Mirror2360->2352,blocks224 unchanged;global93720lines/5066blocks. Whole file still open;source-line check only,delivery pending.

P248 [accelerator/exit declarations](p248-userk-review.json):kernel/client signature suspicion resolved by actual separate inclusion contexts;both original kernel declarations and ClientExitProcess restored. Mirror2352->2349,blocks224 unchanged;global93717lines/5066blocks.2401 original rows map correctly,40 errors retained. Source-only,current file open.

P248 [dependent declarations](p248-userk-review.json):SharedAlloc,xxxDrawDragRect,FreeCopyHandle restored;actual local header closure has no binding conflict. Mirror2349->2346,blocks224 unchanged;global93714lines/5066blocks.2404 source rows map,40 errors retained. Whole file open;delivery pending.

P248 [shared header/type units](p248-userk-review.json):original user.h and winuserp includes restored,product/fixture include paths updated;complete CLIPDATA and selected CAPTIONCACHE restored without new shim. Corrected earlier winuserp restoration statement:previous range stopped at winuser.h. Mirror2346->2330,blocks224 unchanged;global93698lines/5066blocks. Source mapping verified;no compilation yet.

P248 [whole deletion blocks](p248-userk-review.json):10 complete declaration deletion blocks removed;mirror2330->2306,blocks224->214. No71-unit fragmented candidate applied. Source mapping/error-body integrity verified;current file and delivery remain open.

P248 [whole type/comment blocks](p248-userk-review.json):SYSCFGICO,CREATESTRUCTEX and2 complete section descriptions restored;mirror2306->2273,blocks214->210. Original source mapping and40 errors verified;no build/runtime verification yet.

P248 latest local checkpoint ([review](p248-userk-review.json), [census](p248-global-diff-count.json)): five complete message/menu/cursor/title declaration blocks restored in this pass,122->117 blocks and1663->1585 changed lines (-5/-78),without new compatibility code. File against delivered baseline:13->117 blocks (+104),5294->1585 lines (-3709);still unfinished,not an accepted final result. Fixed365-file census:4959 blocks/92953 changed lines,down338 blocks/26960 lines from audit baseline.3080 nonempty original rows mapped;40 error-only excluded bodies unchanged. Source-only;whole-file completion precedes build,publish,test,commit,push. Delivered totals remain352/365 files and2657/2720 points.

P248 latest source checkpoint ([review](p248-userk-review.json), [census](p248-global-diff-count.json)): message-thunk,window and listbox declaration groups restored;this pass117->111 blocks and1585->1485 changed lines (-6/-100). Existing winuserp declaration boundary gains original opaque PSMS outside the task-order binding;no runtime provider/layout added. File baseline13->111 blocks (+98),5294->1485 lines (-3809),still open. Fixed365-file total4953 blocks/92853 lines (-344/-27060 versus audit baseline).3175 original nonempty rows mapped;40 error-only bodies unchanged. No build,publish,test,commit or push yet;delivered totals unchanged.

P248 latest source checkpoint ([review](p248-userk-review.json), [census](p248-global-diff-count.json)): keyboard layout types and associated declarations restored,111->106 blocks and1485->1446 changed lines (-5/-39). Original tagKbdLayer pointer declaration added to existing winuserp boundary;no keyboard implementation imported. File baseline13->106 blocks (+93),5294->1446 lines (-3848),still incomplete. Fixed365-file total4948 blocks/92814 lines (-349/-27099 versus audit baseline).3209 original nonempty rows mapped;40 error-only bodies unchanged. Updated full current-file lexical block inventory is evidence,not completed disposition. No build,publish,test,commit,push;delivered counts unchanged.

P248 latest source checkpoint ([review](p248-userk-review.json), [census](p248-global-diff-count.json)): seven whole desktop/keyboard/broadcast/task-window declaration deletions restored,106->99 blocks and1446->1413 lines (-7/-33),no added compatibility code. File baseline13->99 blocks (+86),5294->1413 lines (-3881);whole file still open. Fixed365-file total4941 blocks/92781 lines (-356/-27132 versus audit baseline).3239 original nonempty rows mapped;40 excluded error bodies unchanged. Current inventory:40 error-bearing and59 other lexical blocks. SMS and original-layout dependency findings recorded as investigation,not approved retention. No build,publish,test,commit,push;delivered counts unchanged.

P248 latest source checkpoint ([review](p248-userk-review.json), [census](p248-global-diff-count.json)): restored two whole scroll/window declaration blocks;99->97 blocks,1413->1385 lines (-2/-28). Three original opaque pointer declarations added to existing boundary;cleanup name mappings verified to follow userk include. File baseline13->97 blocks (+84),5294->1385 lines (-3909). Fixed365-file total4939 blocks/92753 lines (-358/-27160 versus audit baseline).3266 original nonempty rows mapped;40 error-only bodies unchanged;57 non-error lexical blocks remain. Source-only;whole file and delivery incomplete.

P248 latest source checkpoint ([review](p248-userk-review.json), [census](p248-global-diff-count.json)): restored two class API declaration blocks after proving registration/removal adapters use separate class.c inclusion contexts.97->95 blocks,1385->1370 lines (-2/-15);no new compatibility code. File baseline13->95 blocks (+82),5294->1370 lines (-3924). Fixed365-file total4937 blocks/92738 lines (-360/-27175 versus audit baseline).3281 original nonempty rows mapped;40 error bodies unchanged;55 non-error blocks remain. Source-only;whole file and delivery incomplete.

P248 latest source checkpoint ([review](p248-userk-review.json), [census](p248-global-diff-count.json)): original ProcessKeyEvent declaration restored using original tagKE pointer only.95->94 blocks,1370->1366 lines (-1/-4). File baseline13->94 blocks (+81),5294->1366 lines (-3928). Fixed365-file total4936 blocks/92734 lines (-361/-27179 versus audit baseline).3285 original nonempty rows mapped;40 error bodies unchanged;54 non-error blocks remain. Source-only;whole-file review and delivery pending.

P248 [lock declaration boundary](p248-userk-review.json#lock_declaration_boundary_review): traced original kernel gpresUser ownership to selected host SRW domain and verified the two prototype/macro conflicts. Added partial pending decision for original3281/3284 with include-order alternative;no wrapper or source change. This does not complete R2658/R2666 or the file. Metrics unchanged94 blocks/1366 lines locally,4936/92734 across365 files;build and delivery pending.

P248 latest source checkpoint ([review](p248-userk-review.json), [census](p248-global-diff-count.json)): restored two device declaration blocks with original HDEV and opaque resource-lock declarations.94->92 blocks,1366->1338 lines (-2/-28). Existing nt_event.c and wow32.h include nt.h before winuserp.h;userk products force nt.h. No kernel resource layout or provider imported. File baseline13->92 blocks (+79),5294->1338 lines (-3956). Fixed365-file total4934 blocks/92706 lines (-363/-27207 versus audit baseline).3307 original nonempty rows mapped;40 excluded error bodies unchanged;52 non-error blocks remain. Source-only;whole-file review and all delivery actions pending.

P248 latest source checkpoint ([review](p248-userk-review.json), [census](p248-global-diff-count.json)): complete DESKTOP,WINDOWSTATION/DESKTOPTHREADINIT and selected POPUPMENU restored with existing dependencies;92->90 blocks,1338->1162 lines (-2/-176),no new compatibility code. Adjacent by-value WND types remain separate unresolved dependencies. File baseline13->90 blocks (+77),5294->1162 lines (-4132). Fixed365-file total4932 blocks/92530 lines (-365/-27383 versus audit baseline).3459 original nonempty rows mapped;40 error bodies unchanged;50 non-error blocks remain. Source-only;whole-file review,build,publication,tests,commit and push pending.

P248 latest source checkpoint ([review](p248-userk-review.json), [census](p248-global-diff-count.json)): MENUSTATE and related whole declaration blocks restored as one group,90->88 blocks and1162->1068 lines (-2/-94). Original enumeration callback declaration added to existing boundary;no new menu runtime. File baseline13->88 blocks (+75),5294->1068 lines (-4226). Fixed365-file total4930 blocks/92436 lines (-367/-27477 versus audit baseline).3547 original nonempty rows mapped;40 error bodies unchanged;48 non-error blocks remain. Source-only;whole-file review and all delivery actions pending.

P248 latest source checkpoint ([review](p248-userk-review.json), [census](p248-global-diff-count.json)): restored complete units adjoining retained binding omissions,1068->1015 changed lines (-53),88 blocks unchanged. Includes HOOKMSGSTRUCT,three independent menu inline functions and whole declarations;no new compatibility code. Receive-macro backlog now records restored declaration and only remaining two-line macro omission. File baseline13->88 blocks (+75),5294->1015 lines (-4279). Fixed365-file total4930 blocks/92383 lines (-367/-27530 versus audit baseline).3593 original nonempty rows mapped;40 error bodies unchanged;48 non-error blocks remain. Compiler/link behavior including unused inline emission remains unverified;whole-file review precedes all delivery actions.

P248 source checkpoint ([review](p248-userk-review.json), [census](p248-global-diff-count.json)): paint/property declaration block and profile array declaration restored;88->86 blocks,1015->984 changed lines (-2/-31). Exact original profile constants added to existing private-header boundary;no new runtime. File baseline13->86 blocks (+73),5294->984 lines (-4310). Fixed365-file total4928 blocks/92352 lines (-369/-27561 versus audit baseline).3623 original nonempty rows mapped;40 error-bearing/46 other blocks. Source-only;whole-file review and build,publish,test,commit,push pending.

P248 declaration batch ([review](p248-userk-review.json)): eight boundary ranges restore complete message/window/paint/DDE prototypes without new compatibility declarations;86->86 blocks,984->937 lines. 3667 original nonempty rows mapped;40 error bodies unchanged. Source-only;whole-file audit and all delivery pending.

P248 query/DDE declaration checkpoint ([review](p248-userk-review.json)): three whole omissions restored,86->83 blocks and937->917 lines (-3/-20). Two exact original private USER enums and two opaque DDE tag pointers supplied in the existing declaration boundary;no runtime implementation. File13->83 blocks (+70),5294->917 lines (-4377). Fixed365-file total4925 blocks/92285 lines (-372/-27628 versus audit baseline). Logical original rows checked;40 error bodies unchanged. Source-only;current file remains open and build/publication/test/commit/push pending.

P248 object callback/accelerator checkpoint ([review](p248-userk-review.json)): restore53 original declaration lines,83->81 blocks and917->864 lines. Exact original enum and opaque pointers supplied in existing NT/USER boundaries;no object-manager implementation. File13->81 blocks (+68),5294->864 lines (-4430). Fixed365-file total4923 blocks/92232 lines (-374/-27681 versus audit baseline). Source/logical-line proof only;40 error bodies unchanged. CURRENT five mojibake dashes repaired to UTF-8;not mirror reduction. Whole-file audit and build,publish,test,commit,push pending.

P248 scheduler boundary counter-review ([evidence](p248-userk-review.json#scheduler_boundary_counterreview)): two partial source-proven cases admitted to pending,CheckForClientDeath and IsHooked. Separates needed responsibility,original private-owner gap and specific binding placement;no claim that current omission is the only possible implementation. APC not admitted. Mirror metrics unchanged81 blocks/864 lines;no build or delivery and no additional completed file/register point.

P248 APC boundary review ([evidence](p248-userk-review.json#apc_boundary_review)): trace native alertable wait to STATUS_USER_APC and current terminal provider,contrasted with original kernel/client callback. Partial integration-contract decision recorded;no observed product APC failure or equivalence claim. Existing scheduler fixture does not supply real queued-APC coverage. Source unchanged81 blocks/864 lines;current file remains open;no build/delivery or completed-point increment.

P248 desktop-view group ([review](p248-userk-review.json)): complete DESKTOPVIEW and related declarations restored together;81->80 blocks,864->855 lines (-1/-9). Existing cleanup process typedef moved before userk;flags-only consumer gains original opaque process pointer under existing branch. No new selector/runtime. File13->80 blocks (+67),5294->855 lines (-4439);fixed365-file total4922/92223 (-375 blocks/-27690 lines versus audit baseline).40 error bodies unchanged;source/logical-line evidence only. Whole-file audit and all delivery pending.

P248 switch-info restoration ([review](p248-userk-review.json)): complete26-line PHWND/SWITCHWNDINFO unit restored,no compatibility addition.80 blocks unchanged,855->829 lines. Only separate4-line SWITCHWND still depends on by-value WND. File13->80 blocks (+67),5294->829 lines (-4465);fixed365-file total4922/92197 (-375 blocks/-27716 lines versus audit baseline).40 error bodies unchanged;source/logical-line only. Whole-file audit and delivery pending.

P248 existing-layout reuse ([review](p248-userk-review.json)): five whole type gaps restored using existing checked original client layout header;80->75 blocks,829->779 lines (-5/-50). No copied layout or relaxed assertions;selected binding includes and handtabl forced include updated,cleanup two pointer aliases separated after original declarations. File13->75 blocks (+62),5294->779 lines (-4515);fixed365-file total4917/92147 (-380 blocks/-27766 lines versus audit baseline).40 error bodies unchanged. Source-only candidate;whole-file audit and final include/ABI/build/test/delivery pending.

P248 DDE complete-type restoration ([review](p248-userk-review.json)): full32-line type group restored;75->74 blocks,779->747 lines. Existing original object layout reused;exact security context declaration and opaque DDE-info pointer added without provider or runtime. File13->74 blocks (+61),5294->747 lines (-4547);fixed365-file total4916/92115 (-381 blocks/-27798 lines versus audit baseline).40 error bodies unchanged;source-only,whole-file audit and final compilation/delivery pending.

P248 DCE whole-unit restoration ([review](p248-userk-review.json)):55 original lines restored with no added compatibility code;74 blocks unchanged,747->692 lines. DCE has no dependency on adjacent omitted PROCESSINFO layout. File13->74 blocks (+61),5294->692 lines (-4602);fixed365-file total4916/92060 (-381 blocks/-27853 lines versus audit baseline).40 error bodies unchanged;source/logical-line proof only. Whole-file audit and final build,publish,test,commit,push pending.

P248 handle-entry declaration ([review](p248-userk-review.json)): entire5-line omission restored,74->73 blocks and692->687 lines. Original opaque PHE and cleanup-local host alias separated without new provider/table. File13->73 blocks (+60),5294->687 lines (-4607);fixed365-file total4915/92055 (-382 blocks/-27858 lines versus audit baseline).40 error bodies unchanged;source-only,whole-file build and delivery pending.

P248 scheduler declaration group ([review](p248-userk-review.json)): four original prototypes and associated comment restored,73 blocks unchanged,687->678 lines (-9). Cleanup PSMS now matches its existing host declaration context;standalone opaque context unchanged. No message state added. Unguarded wowuserp include partial boundary registered separately. File13->73 blocks (+60),5294->678 lines (-4616);fixed365-file total4915/92046 (-382 blocks/-27867 lines).40 error bodies unchanged;source-only and all delivery pending.

P248 source checkpoint: complete ATTACHINFO restoration (2200-2208),
73->72 blocks and678->669 difference lines. Cleanup has no PATTACHINFO use;
existing input alias remains outside cleanup. Original logical lines checked;
whole-file build/publication pending.

P248 source checkpoint: restored original cleanup/InsertTask declarations
3964-3977;72 blocks unchanged,669->655 difference lines. xxxSleepTask signature
remains separate. Source mapping checked;whole-file build pending.

P248 coupled correction: [initialization current-binding proof](p248-init-current-restoration.json)
disproves P244 R2623/R2626 guest-publication rationale. Original InitTask and
ShowStartGlass contracts restored locally;removed both pending rows and marked
local-awaiting-verification in main register. queue28->23 blocks,4438->4426
difference lines;userk72 blocks,655->650. No whole-file build yet.

P248 source checkpoint: original kernel-entry declaration group3179-3188
restored;userk72->71 blocks and650->640 difference lines. Existing host
PW32PROCESS alias moved to task-order owner;original opaque pointers cover
other declaration contexts. No kernel provider imported;build pending.

P248 counterreview corrected cleanup PTDB to the existing host entry (InsertTask
prototype consistency);mirror metrics unchanged. Source chains for cleanup
failure propagation and client-context exchange recorded. A33-row declaration
restoration candidate would add2 blocks while removing33 lines;not applied or
counted as restored. Backlog distinguishes metric tradeoff from necessary
retention;macro composition and remaining types are unfinished.

P248 SMS complete-unit restoration: retained original body plus existing DBG
#error,omitted only PSMS alias conflicting with real host message ABI.
71->70 blocks,640->619 difference lines;residual alias is pending,not fully
restored. Source/logical lines checked;whole-file build still pending.

P248 complete THREADINFO and two menu getters restored:70->68 blocks,
619->524 difference lines. Existing input-only host THREADINFO alias no longer
leaks into cleanup. Original prefix declaration provenance recorded;no new
kernel provider/state. Logical lines checked;whole-file build pending.

P248 PROCESSINFO declaration restored as a complete original unit (2382-2413):
userk 68->67 blocks,524->492 diff lines. Existing host pointer bindings unchanged;
compat adds only original prefix/bitmap declaration dependencies.40 errors and
prior non-original logical rows unchanged. Source-only;whole-file build pending.
See `p248-userk-review.json` process_declaration_restoration.

P248 scheduler separator restored at original4027:67 blocks unchanged,
492->491 diff lines;guard/code unchanged. WOWPROCESSINFO restoration candidate
measured but not applied:needs exitwin +1 mirror line,userk -12;owner decision
recorded in pending backlog and p248-userk-review.json. No compilation yet.

P248 finite source-chain review:startup cursor is currently an explicit no-op,
not original-equivalent presentation;idle events use task handles,profile calls
retain original FastGetProfileStringW behind machine-registry ownership,and
idle hooks use the worker callback domain. Four declaration subcases recorded
with actual caller/macro/provider evidence;no new dispatcher or source mutation.
Whole-file build and remaining audit still pending.

P248 whole-file source dispositions recorded for R2658-R2666:four original
units restored/confirmed locally,five minimized/boundary points pending owner.
Source review353/365;verified files remain352. Points2666/2720 source-reviewed;
989 delivered,6 local,52 approved/policy,1619 pending,54 unreviewed.
No build/test/publication claim;pending alternatives retain block-growth caveats.

P248 first cached build exposed duplicate RTL_BITMAP in compat/nt.h;removed
the duplicate and reused its existing original ntrtl.h include. Mirror diff
unchanged;fresh cached x86/x64 retry running,not yet passed.

P248 final cached x86/x64 passed after consolidating event declarations and
scoping kernel-only pointers/types;all four actual compiler line contexts pass.
Candidate316fac6d-a205-489b-82d8-e6c8041bd2ea published11x2,hashes verified.
Focused and Full acceptance,Git delivery remain pending;not a closure claim.

P248 userk.h and coupled queue restoration: [receipt](batch-p248-checkpoint.json);cached build,publish11x2,focused37/task-order and Full20 passed. Batch diff lines -4815,blocks +49;residual decisions and growth remain explicit pending.

P249 chartran.c: [whole-file source review](p249-chartran-review.json);original conversion unit and NLS data import restored;two residual boundaries pending,validation not yet complete.

P249 [delivery receipt](batch-p249-checkpoint.json):26->17blocks,226->149mirror diff lines;four points restored/confirmed,two residual decisions. Cached build,publish11x2,focused37/task-order and Full20 passed.

P250 draw.c: [source review](p250-draw-review.json);complete helpers restored,public USER provider boundary retained pending owner;validation pending.

P250 [delivery receipt](batch-p250-checkpoint.json):8->6blocks,1080->392mirror diff lines;three points restored/confirmed,two residual decisions. Cached build,publish11x2,focused37/task-order and Full20 passed.

P251 [drawtext investigation](p251-drawtext-investigation.json):whole-file source review complete;32->20blocks,1766->935lines;final verification pending.

P251 [delivery receipt](batch-p251-checkpoint.json):32->20blocks,1766->935mirror diff lines;five points restored/confirmed,three residual decisions. Cached,publish11x2,focused37/task-order and Full20 passed.

P252 [getset source review](p252-getset-review.json):9->8blocks,251->237mirror diff lines. Two original declarations restored;selected statements unchanged. Five owner-boundary decisions and one original-confirmed point;compiler/batch delivery pending.

P252 [help source review](p252-help-review.json):3->2blocks,46->6lines;two complete helpers restored. Batch two files -2blocks/-54lines;compiler four function lines passed,cached full running.

P252 [delivery receipt](batch-p252-checkpoint.json):two files12->10blocks,297->243mirror diff lines;three restored/confirmed,eight pending boundary points. Cached,publish11x2,focused37/task-order,Full20 and governance passed.
