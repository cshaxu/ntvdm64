# T478 S8 — original suballocator failure-boundary disposition

## Owner standard and conclusion

Baseline 7d49b5d2f. The owner clarifies that S8 and all remaining S packets must
conform to original semantics, without unlimited tolerance or new capabilities.
This supersedes an interpretation of prior audit recommendations as requiring
new transactional failure recovery. Existing implemented behavior is not redone.

S8 closes as **original-equivalent known limitations retained**, not as a repair
of rollback or proof that failure handling is safe in every host condition.
No project-introduced allocation algorithm or missing original connection was
found. No production, guest or existing test source changes are made. The two
known inherited limitations remain explicit in TODO; they are not silently
converted into passing failure-recovery tests.

## Current original-equivalence evidence

Using the actual worker graph's global cflags plus its release /O2 setting,
MSVC /EP preprocesses current suballoc.c, pinned OpenNT/base/mvdm/suballoc/suballoc.c
and OpenNT-4.5/nt/private/mvdm/suballoc/suballoc.c. After removing blank lines and
trimming only line-edge whitespace, all three outputs have 935809 characters and
SHA-256 58766C748BC9EE437F0B62C0B997CCD2848E1DB9834FECD2CB6CC94E5D206D00.
Internal whitespace and string contents are preserved. This compares the selected
translation unit, including declarations; excluded #error branches are not enabled.
There is no effective suballocator-code difference for the selected profile.
Original trees remain read-only comparison inputs, never product link inputs.

The [recheck runner](../../../artifacts/mvdm-audit/shard-2/suballoc-s8-recheck.ps1)
records the exact preprocessing, fixture compilation and execution. The
[compact results](../../../artifacts/mvdm-audit/shard-2/suballoc-s8-results.json)
retain source/archive/map hashes, normalization method, outputs and supervised
run timing. Normalization can be reproduced with Python by joining the nonempty
`line.strip()` values from each UTF-8 .i file with newline and hashing UTF-8 bytes.
An initial graph parser expected a per-object cflags override; it was corrected
to use the actual inherited global flags before any result was claimed.

Current xms.obj references VdmAllocateVirtualMemory, VdmCommitVirtualMemory and
VdmDeCommitVirtualMemory as well as the alternate fallback pair. XMSInit selects
the normal VDM callbacks after successful reservation; STATUS_NOT_IMPLEMENTED
alone selects xmsCommitBlock/xmsDecommitBlock. The examined reservation function
has no such return. The current product map binds the real VDM functions to
original-softpc-host-roots:nt_mem.obj and SAAllocate/SAFree/SAReallocate to
original-mvdm-host-suballoc:suballoc.obj. The normal callbacks can return failure;
the fallback's always-success CPU40 stub is not evidence that failure is impossible.

The CCPU Sas_overwrite_memory slot is already connected to the original
c_sas_overwrite_memory no-op, explicitly documented as doing nothing for CCPU.
Thus the broader audit caveat about compiled-code invalidation must not be
misrepresented as an observed destructive side effect in this selected profile.
Actual successful VirtualFree decommit can still discard page contents.

## Reused tests and observed limitations

The unchanged suballoc_lifecycle_fixture.c is rebuilt with the selected flags
and linked to the actual cached original-mvdm-host-suballoc.lib. Normal mode
passes rounding/tail growth/shrink, preceding overlapping relocation with data,
elsewhere relocation, fragmentation/exhaustion/failed growth/coalescing, and
first-commit-failure recovery/reuse. This is existing behavior, not new S8 capability.

The unchanged --failure-audit mode reproduces both recorded limitations:

| Case | Result | Interpretation |
| --- | --- | --- |
| Second page commit fails | Allocation reports failure, bitmap remains free, no decommit callback undoes the first commit. | Original partial-commit cleanup limitation; no new allocated guest block is delivered. |
| First decommit fails while freeing a 4096-byte range beyond the fixed struct | Free reports failure, but the bitmap exposes 4096 free bytes. | Original failed-free bookkeeping defect; potential later ownership conflict remains a risk. |

The callback fixture intentionally supplies failures. It is not evidence of a
naturally occurring VirtualAlloc/VirtualFree failure or guest corruption. Its
exit zero means these original observations were reproduced, not that rollback
passed. No S8 native-boundary failure injection or new actual-guest run is claimed.

## Why no new rollback implementation

Original CommitChunk itself says to undo earlier commits in a BUGBUG comment,
and SAFree explicitly marks decommit error handling as unfinished. FreeChunk
copies a fixed struct even though Allocated is a trailing variable-sized bitmap;
restoring that struct does not restore the complete bitmap. These are inherited
limitations already present in both original sources, not missing project wiring.

A blanket SET of every freed bit is not an established general correction:
SAFree expressly permits releasing a different size from the original allocation,
and its accepted range may contain previously free subranges. A correct general
snapshot/rollback also needs to account for successful earlier decommits and
lost page contents; restoring allocation bits cannot restore those contents.
Adding a transactional snapshot, recommit/data restoration, retry or new failure
policy would require a stronger contract than this task admits. S8 therefore
preserves the original implementation and records the limitations rather than
adding a partial apparent fix or inventing a recovery guarantee.

This disposition does not declare either limitation harmless or fixed. Reopen
only for evidence of project-introduced drift against original semantics, or an
explicitly admitted requirement to correct/strengthen the original failure
contract. It does not block unrelated remaining S work or authorize repeated
identical audits. S7's two boundary corrections remain unchanged.

## Delivery

This is a research/documentation-only local P. All ten assets/release hashes
still match package 25974683-9d30-4f02-a62c-32e8ca3df424; cached NTVDM also matches
that manifest's input hash. Application 0.0.478/protocol 67 remains unchanged.
[S7](m0-t478-s7-xms-boundaries.md)'s real XMS and 16 selected Full gates remain
prior evidence for these unchanged images, not new S8 test runs. No product
rebuild or publication is needed; only the existing native fixture was rebuilt.
No O:/winnt write, push, other-repository modification or guest change.
Documentation governance and diff checks gate the local commit. S9/finding 13
is next proposed, not admitted; broader T478 inventory qualification remains open.
