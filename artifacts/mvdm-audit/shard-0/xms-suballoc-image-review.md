# XMS and suballocator selected image bodies

The original XMS dispatcher and suballocator objects are selected in the worker
graph. This observer binds `XMSDispatch`, `SAAllocate` and `SAInitialize` to
the released NTVDM code image; the [source review](../shard-2/xms-suballoc-roles-review.md)
separately records original hashes, graph and map entries.

These files remain partial. The dispatch index guard is debug-conditional;
whether an invalid release request is reachable is not established. The
previous [suballocation failure investigation](../shard-2/suballoc-xms-callback-failure-review.md)
retains partial-commit and decommit questions. Actual immutable guest calls,
allocation failure, rollback, nested use and teardown need independent proof.
No guest or product was modified or executed by this observation.
