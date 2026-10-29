# S10 DOS DEM and DPMI32 host C/H follow-up

Scope is the current `host-candidate` rows under `dos/dem/` and `dpmi32/`;
only two were still marked pending in the recorded inventory. The frozen
reconciliation associates each with one DOS audit row and two WOW audit rows;
those source identities are inventory evidence, not proof that guest behavior
is complete. No shared ledger, product, guest, or build cache was changed.

## Per-file review

| Path | Source, selected owner and bounded disposition | Functional obligation |
| --- | --- | --- |
| `dos/dem/demmsg.c` | Current, OpenNT and OpenNT-4.5 SHA-256 `1414099A0A5AD421440F101BBC6C669285141AB53BD9403B9EAD4A6E4D9DB2C8`. The entire formatter and message array are inside `#if DBG`; `dem.h` compiles calls to an empty macro when DBG is absent. The current selected graph builds `obj/dem/demmsg.obj`, but that object's SHA-256 `870CCB698029573AB6737A31EE56D813A7B2FF80960F3D1A6D208F6E41CC1B2B` and symbol inspection show no formatter or message code. Recommend **excluded physical DBG formatter in the selected non-DBG production profile**. This is not a whole-product, other-profile or ordinary DOS error-path exclusion. | S12 must retain real DEM error propagation and teardown. S17 owns whether a separate DBG diagnostics profile is needed. `fShowSVCMsg` itself is present in the current executable map from `demdisp.obj`; its existence does not prove this formatter is present. |
| `dpmi32/i386/dpmimem.c` | Current and both original identities `69695AB6DCB6C977318A4981B6B255FFCBB643B072549DC1C22B89C564EC2DE4`. Original i386 recipe selects this native-VDM virtual-memory provider; current CPU40 instead selects `dpmimemr.c` for the same named interfaces and `xmem.c` for task allocation tracking. The native allocator and shared XMS pool are different ownership systems. **Pending as a global original-provider/equivalence decision**; a physical-file exclusion is supportable only when explicitly limited to the current CPU40 profile and does not close the memory-query semantic question. | S11/S12 must verify actual 0500 reporting, allocation, resize/copy/free, low-memory fragmentation and owner teardown with immutable guest inputs, and compare a real original i386 environment when available. Do not introduce two competing allocators to make a source row appear restored. |

Neither pending path currently differs from the original source; neither can
be promoted to functional completion merely from its 100% byte identity. The
two selected CPU40 provider files do have registered differences: DIV-224
chooses the shared XMS pool and DIV-272 handles the reallocation BOOL. Their
separate partial rows remain subject to behavior and lifecycle validation.

## Current actual-object and initialization follow-up

The current cached worker graph SHA-256 is
`835D8D25248FFBE2C4B4B147C056A43EF7C2F3624A85E09F141258F45FA0410C`.
The current `ntvdm.exe.map` SHA-256 is
`FC7D83F803AC353401364DE7A4F0E873F7F9FBD198BC63FDFF5703352A2229B2`.
It maps all four `Dpmi{Allocate,Free,Reallocate}VirtualMemory` and
`DpmiGetMemoryInfo` symbols to `original-mvdm-dpmi32:dpmimemr.obj`, whose
SHA-256 is `D3F07F4BD83EBEDD830593EF979E3CD5059B336B5A468AAD3F8B9923734E7FF4`.
This is an observation of the current cache and map, not a binding to the
last published or guest-tested image. The existing
[selected-object test](host-dpmi/dpmi-query-selected-object-review.md) has
the same object hash and proves a controlled query call/result, not real pool
fragmentation or native-i386 equivalence.

Further source tracing narrows a failure assumption: the selected
`suballoc.c::SAQueryFree` implementation walks the supplied allocator bitmap,
writes both outputs and returns TRUE unconditionally on its valid path; the
fixture's hypothetical FALSE is not proof of a reachable production result.
`XMSInit` creates `ExtMemSA` and returns FALSE if initialization fails;
`scs_init` calls `host_error(...ERR_QUIT...)` and `TerminateVDM` on that
failure. Actual startup failure, null/invalid allocator, and retry/reentry
remain runtime obligations rather than inferred success. Current source hashes:
`suballoc.c` `B06496E16D551F2BBAC37D896B61857C6569EDA8C98FB3D26DBF574E594148E0`,
`nt_mem.c` `6ABEA408AEF77B1B45C87065F14D56FF046D87C0B52CBFA2534011DB0270D1B1`,
`xms.c` `592F4A063AD84C21890CDC15E56D4900E6669224C7C34C829C09D25DFF9BED75`.

CPU40's `DpmiGetMemoryInfo` still invokes the selected
`VdmQueryFreeVirtualMemory`, whose implementation linearly scans the VDM
allocation-header list, before unconditionally overwriting its outputs via
`SAQueryFree`. The resulting 0500 fields use the shared pool, not the first
query. The existing actual-object fixture confirms both calls under controlled
seams. This is a source-level redundant-query observation, not a measured
performance issue or authorization to remove an original call: its assertion,
initialization ordering and any call-side effects must be resolved before any
change. No new runtime or failure injection was performed for this review.

Detailed prior evidence: [DEM profile](host-dem/demmsg-profile-role-review.md),
[DPMI provider difference](host-dpmi/dpmi-query-provider-difference-review.md),
[DPMI allocator source reconciliation](nine-way/part-2/dpmi-i386-allocator-semantic-reconciliation.md),
and [selected XMEM branches](host-ch-scope/dpmi-xmem-selected-review.md).
