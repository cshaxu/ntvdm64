# S13 per-file evidence inventory (r1383)

The accompanying CSV covers all 1,056 files in the fixed S13 census. Current
and original SHA-256 values were rechecked against the existing file audit; all
2,112 matched. Each row links to its baseline, condition-arm evidence counts,
line-mapping status, prior evidence and any retained arms without observed hits.

`historical_hit_mapped` records historical probe results mapped to current
conditions; it is not a fresh instrumented build. `current_hit` records the
existing current-context probes. Zero observed hits does not establish that an
arm is unreachable. `original-identity` establishes byte equality and hence
original physical lines, not that all branches were exercised. Other line
statuses have the scopes stated in the source evidence; they are not all-context
compiler diagnostic verification. 58 differing files remain not fully
certified, and 11 retained arms in five files have no observed host hit.

This is a tracking inventory, not a completion certificate. The source branch
checklist and its evidence remain authoritative for individual arm decisions.
