# T485 H-series audit and repair handoff

## Question and inputs

Do H001-H036 migrations meet the owner's original-first minimum-diff rule while preserving required excluded-body #error/#line structure? The frozen audit uses project HEAD 6e3e88b53bc57e7f047c9438cae741df5dd14694 and XP SP1 source commit a2f6d7c93aa4f11efc51147566d5961c59a8ef89. It covers 1,039 reconciled text/build files and 4,550 non-excluded points, 27,276 added-plus-deleted lines. The original migration inventory and this normalized audit population differ; these numbers do not replace the 159 directory batches.

## Procedure and observations

The audit re-paired 11 problematic exclusion files, removed overlapping masks, inspected retained differences, and ran bounded compilation/preprocessing and actual-provider counterexamples. [Audit result](audit-result.txt) records the findings and limits; [pairing dispositions](pairing-dispositions.txt) describes the 11 files. [Control receipts](controls.json) retain the machine-readable outcomes, including the selector0040 defect, CHAIN2 negative control, x87/OEM controls and formatting equivalence checks. Raw probes/logs remain locally under build/h-series-xp-audit-nonexcluded; these receipts are supporting evidence and cannot replace rebuilding/testing final fixes.

## Interpretation and follow-up

This is an audit handoff, not repair acceptance. All points have a classification, but open necessity/minimality evidence remains unfinished investigation. Historical `retain-*`, `candidate` and zero pending-classification counts must never be read as approval, safe bulk reversion or runtime completion. The 2,889 formatting-equivalence points prove only the tested selections. The 25-point CHAIN2 control proves cohort necessity, not individual failure for each point. No production source was modified by the audit.

The owner now assigns repair to the [ROI-ordered S plan](../../operations/m0-t485-h-repair-plan.md), ahead of N004. [Files](files.csv) carry source hashes and exclusive primary S ownership; [points](points.csv) carry every original ID, file/line anchor, necessity, minimality and evidence locator. [Assignment counts](assignment-summary.json) check coverage. Point line anchors refer to the audit snapshot; locate latest code by source identity and content rather than blindly applying old physical lines.

The audit observed concurrent dpmiint.c ownership/flag changes and reviewed that delta without changing it. New changes since the audit must be requalified when their file starts; preserve in-flight S5 P4 and unrelated work. Later exact source/receipt identities belong in repair delivery records and the current register, not rewritten into historical test results.

[S6 selector investigation](s6-selector-investigation.md) records the final repair, failed controls and S6 delivery validation.

[Repair ledger](repair-ledger.csv) overlays the frozen audit IDs with current implementation and residual-review status. [S6 setter trace](s6-setter-trace.txt) records the actual zero-limit publication counterexample.

[S7 ordered repair progress](s7-progress.md) separates local review, pending verification and delivery.

[S7 P6 mirror residuals](s7-p6-mirror-diff.json) records all 47 current mouse differences, upstream anchors, sizes and provider evidence; runtime acceptance remains separate.
