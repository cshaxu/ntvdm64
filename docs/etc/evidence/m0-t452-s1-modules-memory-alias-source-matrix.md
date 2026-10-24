# M0 T452 S1 — modules, memory and aliases source matrix

## Purpose and evidence boundary

This is the bounded source-to-production review required before W03 changes
runtime code. It consumes the content-pinned external audit named in the
[working plan](../operations/t452-modules-memory-aliases-plan.md); it does not
turn a static audit result into runtime evidence. All observations below were
made against product source at `058121557` and the pinned OpenNT comparison
edition. No guest input, external audit input, image, build output, or runtime
process was changed.

The selected x86 provider graph has no `i386` preprocessor definition. Its
effective flags include `CPU_40_STYLE`, `CCPU`, `SPC386` and `V7VGA`; this is
the CCPU40 software-machine branch, not the historical kernel-V86 branch.

## Finite W03 matrix

| Original seam and guest entry | Selected source path and current build route | Sole state authority | S1 disposition and S2/S3 proof |
| --- | --- | --- | --- |
| `SEAM:ef05926cf522c1c97f6b`, KERNEL k49 `GETMODULEFILENAME` | `wktbl2.h` binds `WK32GetModuleFileName`; `wkernel.c` maps either a real HMODULE or an HTASK alias and maps the caller's output range with `ALLOCVDMPTR`. `wkernel.c` differs from pinned OpenNT only in unrelated, registered profile-shadow lines. | Real modules remain modern loader modules; task aliases remain the original `wole2.c` table. The guest range is caller-owned; no host pointer becomes guest state. | Keep the original dispatch and identity selection. Repair only bounded output: zero capacity returns zero without mapping/writing; a nonzero capacity is the sole flush bound; normal, exact, truncation, conversion/query failure and retired alias all return a bounded result. S2 adds target-specific tests. |
| `SEAM:42571d704af29168b45b`, KERNEL k47 `GETMODULEHANDLE` fallback | `wktbl2.h` binds original `WK32WowGetModuleHandle` in `wkman.c`. It is called only after the Win16 kernel's own lookup fails. The same `PTD::pWOAList` is made by original `WK32LoadModule32`, associated by original `W32Thread`, consumed by k47 and retired by `CleanupWOAList`/`W32FreeTask`. | Original per-task `PTD::pWOAList`, with its native child-process handle. No new module or task registry is allowed. | Retain direct original body. S3 validates local-hit bypass, matching/nonmatching child, null association, child wait/reap, parent-task retirement and concurrent lookup/retirement. Parent teardown must close only its owned handle if evidence proves that it survives record disposal; it must never terminate a live child merely to clean a list node. |
| `SEAM:d8f111b47450e5cbf412`, KERNEL k233 `VIRTUALALLOC` | `wktbl2.h` binds verbatim original `wkmem.c`. Effective non-`i386` branch calls `VdmAllocateVirtualMemory(address,size,TRUE)` and falls back to host `VirtualAlloc` only for `STATUS_NOT_IMPLEMENTED`. `nt_mem.c` is the selected SoftPC allocator and returns page-aligned Intel linear addresses. | `nt_mem.c` section-header/Intel-memory allocator. It owns reserve, commit, rounded size and release. | Direct original reuse; **zero runtime source diff expected**. S2 proves the selected branch, page-aligned guest-linear result, null-on-non-success and `STATUS_NOT_IMPLEMENTED` fallback without adding a WOW heap, registry or raw-host-pointer projection. The historical i386 `\0WOW` fill is not selected by this image and is not copied into CCPU40. |
| `SEAM:d558e9391f6aafcab7c7`, KERNEL k234 `VIRTUALFREE` | Adjacent original `wkmem.c` body calls `VdmFreeVirtualMemory((ULONG)address)` on the same non-`i386` branch; only `STATUS_NOT_IMPLEMENTED` falls back to host `VirtualFree`. The selected Win16 arena caller passes `MEM_RELEASE` and size zero, then keeps its original bookkeeping rule of not branching on the BOOL. | The same `nt_mem.c` allocation record as k233. Guest arena bookkeeping remains guest/original code. | Direct original reuse; **zero runtime source diff expected**. S2 proves same-address release, invalid/repeated release false projection, selected fallback and no invented rollback/retry or DPMI/non-WOW pairing. |
| OLE HTASK aliases used by k49 and `GetWindowWord` | Verbatim original `wole2.c` owns `Add/Find/Get/RemoveHtaskAlias`; `walias.c` selects aliases for foreign thread IDs and `wuwind.c` consumes them. The source validates a live thread by ID, process ID and creation time before exposing it. | The original 32-slot alias table is the sole alias owner. Its entries contain only thread/process IDs and creation time; no pointer-sized handle crosses the guest boundary. | S3 preserves the original selector encoding and 32-slot full-table oldest-entry replacement rule as a documented compatibility capacity boundary. It may add a narrow lock/initialization wrapper around this existing table, not a second registry. Tests cover repeated live lookup, retired thread removal, selector reuse, query failure and concurrent add/find/remove. |
| Alias-to-image-name conversion | Original `GetHtaskAliasProcessName` currently calls `NtQuerySystemInformation` into a fixed 16 KiB stack buffer, converts a found Unicode image name and then uses `strcpy`; meanwhile k49 flushes `strlen(output)+1` rather than caller capacity. | Alias record remains `wole2.c`; the host query is only a transient observation. Output storage remains the guest caller's bounded range. | S2 makes the smallest source-shaped repair: retain the original system-process enumeration and Unicode-to-ANSI conversion, but obtain one required query length, allocate a transient bounded buffer, make one populated query, and copy/flush at most the declared caller capacity. A changed process list or any query/conversion/allocation failure returns zero and leaves no host buffer/alias resource. No polling, retry loop, permanent cache, process-handle table or new adapter ownership is admitted. |

## Direct comparison and exclusions

`wkmem.c` and `wole2.c` are byte-identical to the pinned OpenNT comparison
edition. `wkernel.c` and `wkman.c` contain pre-existing registered product
divergences outside these W03 leaves (profile shadow and lifecycle/USER
bindings respectively). This task neither deletes nor broadens those changes.

The existing `mvdm_softpc_wow_page_domain.c` uses
`VdmAllocateVirtualMemory` for the approved USER-layout projection. It is not
a general WOW allocator and is not an alternative authority for K233/K234;
W03 must not merge, replace, or call it for these KERNEL entries.

Guest LOCAL allocation, resources, menu conversion, DIB mappings, GDI, files
and OLE behavior remain owned by their named packets. This review does not
make their same-named APIs a W03 dependency.

## Planned implementation delta

S2 has two original-mirror repair loci (`wkernel.c`, `wole2.c`) and no new
runtime source owner. The expected production delta is one bounded k49 output
path and one source-shaped transient process-query path; exact lines are
measured only after review of the complete patch. `wkmem.c`, `nt_mem.c`, the
WOW dispatch table and the guest image are expected to remain untouched.

S3 may add only synchronization around the existing alias table and, if the
targeted handle-lifetime evidence proves it necessary, close an owned
WinOldAp process handle during node retirement. It cannot create a new alias,
module, memory or task manager. Any change to the original 32-slot eviction
contract would be a new observable compatibility decision and is not admitted
by this plan.

## Exact verification plan

S2 tests are one coherent batch, not ad hoc application retries:

1. k49 real module and alias lookup with zero, one, exact, truncated and
   inaccessible guest output ranges; validate return length and only the
   declared bytes change.
2. Alias image lookup after process/thread retirement and after query,
   conversion or allocation failure: return zero, bounded untouched output
   and no leaked transient allocation.
3. k233/k234 selected CCPU40 allocation/free pair: guest-linear alignment,
   use, release, invalid second release and null-on-failure; prove the
   allocation never goes through the USER page-domain or a raw host pointer.

S3 tests hold the existing list/table owner under repeated and concurrent
operations: normal WinOldAp fallback, no match, local module bypass,
association-before-use, child exit/reap, parent task retirement, live alias
lookup, retired alias withdrawal and legacy capacity replacement. A runtime
test may not claim a handle leak from source inspection alone.

Only after these grouped paths pass do S2/S3 run the applicable package
regression, cached ten-component product build, publication and independent
application frontiers required by the active packet.

## Uncovered boundaries

No runtime proof exists yet for any row. In particular, process-list growth
between the size query and populated query is deliberately an explicit zero
result rather than a retry policy; the original fixed-stack path had no valid
result in that case. Cross-session/process access denied by the host query is
also an explicit zero result. The original alias table's 32-live-entry
replacement is retained as source behavior, not reclassified as a host bug.
