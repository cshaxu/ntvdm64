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
4. If a difference is necessary, establish its smallest possible form. Reduce
   mirror diff and autonomous/shim complexity together; moving equal
   complexity elsewhere is not simplification. Proven native Win32 ownership
   may be a valid boundary; omission alone is not evidence of a defect.
5. If the final proposed outcome preserves the diff unchanged or increases
   it, STOP at that item and ask the owner with concrete necessity evidence,
   alternatives and per-item original-relative added-plus-removed delta.
   Do not continue to later items while approval is pending. No offsets from
   independent reductions; no same-size silent retention.
6. Missing evidence is not a retain decision. Investigate the bounded actual
   call chain; record what is unknown. No blanket restoration without checking
   actual callers, compile context and source policy, and no invented original
   bug. Dependencies may be read without completing later queue items.
7. Update the item, progress and current snapshot at every delivery. A completed
   audit is not a completed repair; keep implementation/verification/delivery
   explicit. Complete a point before moving on; do not repeatedly count it.
   Apply existing ten-modified-file/final-batch cached build, regression,
   coherent publication, commit/push and net-diff approval rules.

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
