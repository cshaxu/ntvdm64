# T478 S17 — original WOW virtual-memory boundaries

Baseline: c5dece1a7. Finding 42 is closed as connected original behavior with
retained original limitations, not as a repaired failure-recovery guarantee.
No product source or release bytes change. A stale test-only link boundary is
repaired so the existing original-memory fixture runs again.

## Current owner and contract

Selected wkmem.c alloc/free thunks call VdmAllocateVirtualMemory(commit TRUE)
and VdmFreeVirtualMemory; only STATUS_NOT_IMPLEMENTED selects native fallback.
Other allocation failures return NULL. Current worker binds nt_mem.obj and its
VirtualAlloc/VirtualFree imports to kernel32; commit/decommit can fail. This
must not be described as an unreachable failure path or a missing host provider.

Five release bodies (allocate, free, reallocate, commit, decommit) match both
original families after balanced DEBUG_MEM-block removal, #line/comment removal
and token comparison. No project allocator replacement was found here.
Allocation sets/splits headers before host commit; on failed commit it returns
STATUS_NOT_COMMITTED before totalFree update and without undoing those headers.
That is an inherited state-recovery limitation, not tested as safe. Adding a
transactional rollback guarantee is outside the owner's original-semantics
ceiling, as in S8. Decommit failure is also preserved; CCPU's overwrite-memory
slot is the already-connected original no-op, not observed destructive rollback.

The two flags & HDR_VALID_ALLOC != HDR_VALID_ALLOC tests have the original
precedence defect. Correct normal free callers supply an allocated chunk start;
no valid caller requiring stale/arbitrary address acceptance was demonstrated.
This stage does not add new malformed-address tolerance or declare repeated
free safe. Normal original guest arena ownership was established in the prior
read-only review; no guest code is changed or audited anew. Project page-domain
owners also call this allocator and retain their own allocation records; no new
double-free symptom is asserted from the ineffective check alone.

The old report's absent-reallocate-export statement is stale: current product
provider DEF exports query/reallocate at 270/271. The direct fixture verifies
its own name/ordinal providers; that alone is not live guest invocation. CPU40
DPMI reallocation still selects SAReallocate before the VDM call. There is no
basis to restore an alternate DPMI allocator or duplicate the existing exports.

## Verification and test maintenance

[Receipt](../../../artifacts/mvdm-audit/shard-3/virtual-memory-s17-results.json).
The existing original-external-memory-test target initially failed to link:
worker_lifecycle now references mvdm_standalone_worker_close_frontends, absent
from its test-owned entry boundary. Add the real header and an asserted-unreached
seam alongside existing broker boundaries in ccpu_host_fixture_seams.c. It exits
ERROR_CALL_NOT_IMPLEMENTED if reached; it does not invent successful shutdown or
stub the allocator under test. Production entry/link inputs are untouched.

Rebuilt with the current graph, the existing fixture exits zero and verifies
original allocation grow/shrink/content/free, external mapping retirement,
normal backing restoration, EMS/SAS cross-window copies and real bidirectional
DIB/GDI sharing. Existing duplicate-boundary /FORCE linker warnings are retained;
this fixture is not a full product entry test. No artificial commit failure,
repeated invalid free or real guest memory-pressure failure was run or claimed.

All ten worktree release hashes match S16 package
b4a13bec-806d-48d3-ad50-728178335129, whose 16/16 Full result remains applicable
to the unchanged bytes. No new Full run or republication is claimed for this
test-only repair. No O:/winnt writes, push or original repository modification.
