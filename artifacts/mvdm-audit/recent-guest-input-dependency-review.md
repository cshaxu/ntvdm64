# Recent guest-input dependency path recheck

Some recent checkpoint scripts compared absolute paths against Ninja dependency
output. Ninja can emit paths relative to the build tree, so that check alone
could miss an actual dependency. The accompanying observer separately checks
both build graphs and dependency logs using normalized `/src/mvdm/` suffixes.

At this recheck all 37 excluded paths in the five named recent checkpoints had
no exact path hit in the four inspected host graphs/dependency logs. Thus no
classification in this bounded set is contradicted by this check. The result
does not establish guest behavior, source-to-image lineage, other build profiles
or correctness of every historical observer. Each original checkpoint retains
its own evidence and limitations. No product or guest was changed or run.

S10 must also inventory other observers that compare absolute paths against
Ninja dependency output and recheck any affected already-merged disposition
with normalized paths. Pending recommendations cannot be merged on the older
negative check alone. This 37-path result does not close that wider review.
