# T484 S3 P99 native control retention review

B0164 already delegates production to CMD/native CMake. Reconcile that
existing migration with an actual current cohort run and retention review,
rather than migrate the same producer twice.

Delete wow_cpd_window_normalization_probe.c and its native build/selection
entries. It implements a candidate scratch-window decoder, not production
code; the original control-source audit explicitly excludes that design.
No current product or test consumer depends on its algorithm. The historical
research remains in m0-t447-s3-control-source-audit.md and Git history.
Other native references/production contracts are retained: in particular the
style probe targets the same native masked style API used by current code.
No new success is claimed for independently selected research modes.

Runtime PID32800 exited0: all38 default control fixtures passed. These cover
original control bodies, local lifecycle, callbacks and native references;
they are not full guest acceptance. Product, mirrors and original guest are
unchanged. Removal reduces test/build source by146 net lines.

Final PID33236 exited0: full cached host build,38 stable fixture image
hashes/mtimes, x64 rejection2,11 raw host hashes and22 publication-copy
hashes. Product bytes unchanged; existing publication verified.

[Receipts](../../../artifacts/build-migration/t484/s3-control-retention/result.json)
record every current case and cache identity; raw logs remain
build/t484-s3-control-retention. Inventory196 complete/159 remaining of355.
The removed candidate is outside that inventory and is not an extra completed
entry. S3 and retrospective review remain active; S4/S5 remain pending.
