# S10 shard 1: WOW cursor and icon ownership

The [observer](wow-cursor-observer.py) and [evidence](wow-cursor-evidence.json)
recheck three pending shard-1 original sources against both frozen catalogs,
both original editions, the selected WOW build and the released/installed DLL.
All three retain 100% original source bytes. The selected cached DLL and the
released DLL have different whole-file hashes but identical executable
sections; all object/image code comparisons are scoped to those sections.

The released USER dispatch table initially maps ordinals 173 and 174 to the
original `WU32LoadCursor` thunk and ordinal 533 to `WU32NotifyWow`. Their
actual linked call relocations reach `SetupResCursorIconAlias` and
`W32CheckIfAlreadyLoaded` respectively in the original `wcuricon.obj`. The
original lookup and task alias list retain ownership of guest resource
identity and locks. The private USER output slot is assigned by the project
registration bridge to `wow_private_user_load_create_cursor_icon`; its linked
object imports public `CreateIconFromResourceEx` and `LoadImageW` under the
registered `ADAPTER-WOW-016` boundary. The static assignment does not prove a
live registration or every resource format, error, or alias lifetime.

Task cleanup needs a precise qualification to the earlier
[cursor source review](cursor-cache-trace.md): the actual image first calls
project `W32FreeTask` in `wow_task_cleanup_entry.obj`, which invokes the USER
retirement preparation and then `WowOriginalFreeTask` in original `wkman.obj`.
The latter's linked call reaches original `FreeCursorIconAlias`. The observer
binds both call edges; it does not prove that every task-exit route reaches
them or that reentrant/failed retirement is correct.

The historical `wcurcash.c` owner has a separate `pCurIconFirst` list and is
absent from both original WOW source selection and the selected link graph.
The searched static references to its three named APIs occur only in its own
source/header. The selected original alias lookup is a different state owner;
do not introduce a parallel cache solely to recover the unselected file.
Equivalence of zero resource IDs, case folding, task ownership and teardown
is not established, so file-only exclusion remains a candidate, not a
completed disposition.

| File | Recommendation | Remaining boundaries |
| --- | --- | --- |
| `wucursor.c` | Partial candidate | Exercise real guest resource and stock loads, string/integer IDs, encoding, missing/invalid resources and failure cleanup through the registered adapter. |
| `wcuricon.c` | Partial candidate | Verify task/module/handle reuse, shared aliases, lock and replacement, reentrant failure, teardown and cross-task behavior with the deployed DLL. |
| `wcurcash.c` | Pending; bounded historical-cache exclusion candidate | Establish whether any applicable original path needs its distinct behavior, including numeric ID zero; an absent static reference alone is insufficient. The guest cursor/icon capabilities remain in scope regardless. |

Nine mapped functions and eight pertinent relocation edges were checked; the
remainder of their relocations and behavior are not claimed by this audit.
Restored-behavior percentages are unmeasurable from static image evidence.
No guest, product image, desktop or shared ledger was modified or exercised.
