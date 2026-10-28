# Shard 1: original accelerator aliases and task cleanup

Proposed **partial** for the pending original `wow32/waccel.c` file. The
[observer](wow-accelerator-observer.py) and [receipt](wow-accelerator-observations.json)
recheck source against both original editions and both audit catalogs, the
selected object and valid dependency, and its eight mapped function bodies in
the released WOW32 image. Object bytes are compared outside relocation words;
this does not prove behavior. Original source text is 100% retained; a
complete behavioral-restoration percentage cannot be measured here.

The previously image-bound original USER ordinal 533 reaches `WU32NotifyWow`.
The current observer independently resolves its accelerator branch to original
`WU32LoadAccelerators`, then `CreateAccel32` and `SetupAccelAlias`; the latter
calls the selected original small-heap allocator. The original
`WowOriginalFreeTask` call resolves to `DestroyAccelAlias`. These five released
machine-code targets establish selected call edges, not that a guest actually
sent the relevant notification or that all branches executed.

The original accelerator layer owns the 16/32-bit alias list, conversion and
per-task cleanup; Windows owns native accelerator handles, and the current
WOW task context supplies guest task identity. `DestroyAccelAlias` receives a
task argument but uses `CURRENTPTD()->htask16` to select list entries. The
task cleanup caller passes its task's handle; whether the current task always
matches that argument during teardown remains to be verified rather than
assumed. The source also creates a native table before attempting alias setup;
investigate allocation failure, handle ownership and cleanup with a real
fault/retirement test before treating a leak as demonstrated.

Outstanding coverage includes guest ordinal routing and signed/size limits,
duplicate handles, malformed 16-bit accelerator records, allocation and
native API failures, aliases across simultaneous tasks, nested callbacks,
explicit destruction and cleanup after task/worker exit. No guest, runtime
image, installed file or shared ledger was modified. Coordinator approval of
the source-role evidence is needed before updating its classification.
