# S13 per-file evidence inventory (r1383)

The original r1383 census covered 1,056 files and all 2,112 current/original
hashes matched at that checkpoint. One unreachable header was removed in r1413;
the accompanying current CSV now covers 1,055 retained files. Each row links to its baseline, condition-arm evidence counts,
line-mapping status, prior evidence and any retained arms without observed hits.

`historical_hit_mapped` records historical probe results mapped to current
conditions; it is not a fresh instrumented build. `current_hit` records the
existing current-context probes. Zero observed hits does not establish that an
arm is unreachable. `original-identity` establishes byte equality and hence
original physical lines, not that all branches were exercised. Other line
statuses have the scopes stated in the source evidence; they are not all-context
compiler diagnostic verification. 52 differing files remain not fully
certified, and 10 retained arms in four files have no observed host hit.

This is a tracking inventory, not a completion certificate. The source branch
checklist and its evidence remain authoritative for individual arm decisions.
