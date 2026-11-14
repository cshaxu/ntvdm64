# R1336-R1370 initial evidence

This is an in-progress whole-file review, not a disposition or delivery.
The next ordered file is `src/mvdm-host/wow32/wcntl32.c`, 35 points.
Baseline is P137 commit `4dbd3a0e7251a8ce225632fb3eb59296a23de8d7`.
No mirror source has changed during this evidence collection.

## Source and measurement

Pinned OpenNT revision `5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`,
`base/mvdm/wow32/wcntl32.c`, was read directly through Git.
The current original-relative line metric is +54/-32, total86, excluding
standalone `#line` directives, with no exclusions in this file.
`build/t483-s5-p138/compare-controls.py` compares all14 control converter
bodies used in the historical construction fixture with that pinned original.
All14 match exactly after CRLF normalization; result is
`build/t483-s5-p138/control-source-proof.json`.

## Actual diagnostic run

The maintained `test/observation/verify-wow-message-construction.ps1`
runner was launched hidden through the desktop-capable modern pwsh host,
using the actual x86 compiler driver and `-Transaction`. Parent PID10776
terminated with a script assertion: its ChooseFont source check did not allow
the existing `#line 770` between `stack.completed = TRUE` and `return(FALSE)`.
Both executable statements are present. The test now removes line directives
from this selected check input only; it retains the required assignment,
return, negative baseline and all runtime fixture assertions.

Retry parent PID26100 completed successfully. Its owned child records,
commands, timestamps and exit codes are in
`build/t483-s5-p138/transaction-before-r2/steps.json`.
The historical fixture exited1 as expected, reporting72 failures, including
heap/control exception cleanup. The current fixture exited0 with167 scenarios,
19 heap families, five modes per family, nested/global-lock/stack/CBT checks.
Result: `build/t483-s5-p138/transaction-before-r2/result.json`.
This is exact-body testing with host service doubles, not guest runtime proof.
The72 failures describe the whole fixture, not72 defects in this file.

## Findings and remaining review

The original selected control bodies release heap buffers in their normal
unthunk phase. Constructor/copyback exceptions bypass that release.
The current owner finalizer calls `wow_message_unwind32`; captured allocation
keys permit release without replaying output conversion. The14 tested original
bodies confirm this is not merely comparison with an invented old adapter.
These facts support a cleanup responsibility, not automatic necessity of every
current wrapper or state field.

Next compare reuse of original lParam versus captured keys, the LB_DIR local
allocation-before-lParam assignment, and CB_DIR map-reference versus heap
ownership across callback and repeated cleanup. Review all producer/caller
initialization before deciding whether its explicit resets are redundant.
Only then finalize R1336 and subsequent ordered point dispositions, apply the
whole-file reductions, and run final-identity delivery gates. Main register,
pending decisions and completed counters remain unchanged; no35-point bulk
retention or whole-file completion is claimed.

## Whole-file follow-up

The source review and reductions are now recorded in
[the P138 checkpoint](batch-p138-checkpoint.json); that record supersedes the
initial review status above. The14 control converters and the separate CB_DIR
baseline body match the pinned OpenNT originals. The final source passes the
transaction fixture (167 scenarios) and parameter-map fixture (54 scenarios),
with the expected historical failures retained.

The first final-source line comparison paired the original inner closing brace
at470 with the surviving outer brace at471. Original CB_DIR has nested
conditionals; the selected adaptation removes the inner DeleteParamMap test.
The outer closing brace remains explicitly mapped to its original471.
The comparison now records that structural disambiguation, without changing
product #line to satisfy a greedy textual match. Actual preprocessing finds540
shared selected lines at the original positions and no unresolved mismatch.

Whole-file review identified duplicate initialization, not a replacement
ownership mechanism: remove the two CB_DIR resets, initialize the existing
consumed-state snapshot at entry, and remove14 standalone duplicate comments.
No original guest or autonomous product code changed. Mirror metric86 to69,
net-17. Final full-build/publication/regression/Git status belongs to the
checkpoint, not this preliminary diagnostic receipt.
