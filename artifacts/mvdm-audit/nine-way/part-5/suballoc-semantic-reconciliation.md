# Part 5: XMS/DPMI suballocator declarations and failure boundary

The three owned `suballoc` files are individually hash-identified in the part-5 host-input observer. `suballcp.h` is the original private header included by selected `suballoc.c`; exact valid Ninja dependency records tie it to `worker/obj/suballoc/suballoc.obj`. It describes one-kilobyte units, allocation bitmap and callbacks; the selected original implementation owns suballocation state. `xms.486/xms.c:59-64` creates `ExtMemSA`, and `xmsblock.c` plus `dpmi32/dpmimemr.c` call SAAllocate/SAFree/SAReallocate against that shared owner. Host CPU40 SAS owns actual backing; no independent competing address allocator is implied.

| Owned path | Evidence-supported file-role disposition / boundary |
| --- | --- |
| `suballoc/suballcp.h` | Original private allocator declarations, exact current selected-object dependency; individual code paths and current release execution are not established by dependency. |
| `suballoc/suballoc.txt` | Historical design/source-description text, not the implementation; retain for evidence, no claim it exactly describes every selected provider. |
| `suballoc/foo.bar` | Original private-header-like alternate text, differs from `suballcp.h` in 11 diff lines; no observed actual consumer, provenance or purpose proved. Keep pending, do not call it unused or delete it. |

The chronological T420 S36 record reports two generic original allocator failure limitations with fallible callbacks: partially completed commits lack rollback, and a failed decommit can expose a changed bitmap. Its final ledger `m0-t420-s36-suballoc-capability-review.md:390-417,510-526` supersedes earlier uncertainty for that candidate: original CPU40 SAS preallocated backing callbacks avoided those specific fallible behaviors, real direct/nested XMS relocation and DPMI lifecycle witnesses passed, and a finite mapped-mover binding remained. This is versioned evidence, not proof every later callback provider or current release still has the same reachability. Do not claim the generic defects fixed.

S11/S12 should trace current formal graph and exact callback provider, then repeat real XMS/DPMI allocation, resize/move, data retention, release and task-loss using the immutable guest; if a fallible provider is reachable, reproduce rollback and define its bounded owner. S19 binds the candidate test and exact release. Keep original suballocator state as sole owner and do not graft another bitmap or patch guest. No tests, product, guest, or shared-ledger modification in this review.

Replay `rg -n "SAInitialize|SAAllocate|SAFree|SAReallocate" src/mvdm/xms.486 src/mvdm/dpmi32 src/mvdm/suballoc`, inspect `suballcp.h` and T420 S36 final ledger, and rerun the part-5 host-input observer. `git diff --no-index --stat -- src/mvdm/suballoc/suballcp.h src/mvdm/suballoc/foo.bar` is a read-only comparison.
