# M0 T237 S1 — XMS source, ABI and failure audit

## Decision

`52:xx` is one twelve-service OpenNT XMS provider package.  The next stage
must import its source-shaped dispatcher and service files as one unit; it
must not retain, extend or route through the handwritten `bop-v1` XMS session.
The existing mantle A20 and physical-memory capabilities are mechanical
dependencies only.  They are not an XMS provider and do not change the owner
of XMS policy, register results or allocation lifecycle.

## Original source map

| Source | Original role | Current identity | S2 treatment |
| --- | --- | --- | --- |
| `base/mvdm/xms.486/xmsdisp.c`, `xms.h`, `inc/xmssvc.h` | 12-slot dispatcher and service ABI | byte-identical imported OpenNT source | Mirror as the sole bounded 52:xx route. |
| `xmsa20.c` | A20 and HIMEM-state contract | byte-identical | Mirror; replace only the old SAS operation with typed mantle A20 mechanics. |
| `xmsblock.c` | allocate/free/reallocate/move/query-free | byte-identical | Mirror; make raw VDM and virtual-memory access go through one checked memory/suballocator seam. |
| `xms.c` | package initialization and XMS-memory policy | byte-identical | Mirror; declare one session XMS-memory configuration and initialize the original suballocator policy. |
| `xmsmisc.c` | page size, memory size, INT 15 notification | byte-identical | Mirror; retain public page-size query and give INT 15 one named machine-capability outcome. |
| `xmsumb.c` | UMB list and request/release policy | byte-identical | Mirror; depend on a typed mantle UMB reservation capability, never an adapter-owned UMB allocator. |
| `suballoc/suballoc.c`, `suballoc.h` | OpenNT free-list allocation policy used by XMS (and later DPMI) | imported source | Include as a source-mirrored common dependency; its backing-memory callbacks are the compatibility seam. |
| `xms.486/i386/xmsmem86.c` | NT4 process virtual-memory commit/decommit/move callbacks | byte-identical but private-NT dependent | Do not link directly.  Replace the three callbacks with a documented mantle-backed compatibility seam; no private ntdll import. |

The historic selector caller is `softpc.new/host/src/nt_bop.c`, which reads
the service byte at guest `CS:IP` and calls `XMSDispatch`.  It mixes CCPU/SAS
host state with every selector and is reference-only.  bx-vdm must instead
perform fixed-width, bounds-checked selector/service ingress, then call the
source-shaped XMS dispatcher.  The dispatcher itself must never become a
Bochs selector recognizer.

## Full endpoint ledger

| BOP | Original function | ABI/result contract | Required mechanics | S2 disposition |
| --- | --- | --- | --- | --- |
| `52:00` | `xmsA20` | `AX=0` enable wrapping; `1` disable wrapping; `2` query; invalid input returns `AX=0`; query sets `BL=0`. | A20 query/set and HIMEM-state byte. | Direct source + typed mantle A20 seam. |
| `52:01` | `xmsMoveBlock` | Move descriptor at `SS:BP`; returns `AX=1/0`. | Checked guest descriptor read and physical guest-RAM copy. | Direct source + checked-memory seam. |
| `52:02` | `xmsAllocBlock` | requested KiB in `DX`; result base KiB or zero in `AX`. | OpenNT suballocator plus extended-RAM backing. | Direct source + suballocator/mantle backing seam. |
| `52:03` | `xmsFreeBlock` | base/size inputs; `AX=1/0`. | Same allocation identity and backing. | Direct source + suballocator/mantle backing seam. |
| `52:04` | `xmsSysPageSize` | host page size in `AX`. | Public `GetSystemInfo`. | Direct source, public Win32 API. |
| `52:05` | `xmsQueryExtMem` | configured XMS KiB in `AX`. | Declared session XMS-memory configuration. | Direct source + session configuration seam. |
| `52:06` | `xmsInitUMB` | initializes XMS UMB list and remembers `AX:BX` HIMEM A20-state address. | UMB reservation/list capability, checked far address. | Direct source + named mantle UMB seam; no adapter allocator. |
| `52:07` | `xmsRequestUMB` | `DX` paragraphs; success `AX=1`, `BX` segment, `DX` actual; source failure `BL=B0/B1`. | UMB list/reservation lifecycle. | Direct source + named mantle UMB seam. |
| `52:08` | `xmsReleaseUMB` | `DX` segment; success `AX=1`; source failure `BL=B2`. | UMB list/release lifecycle. | Direct source + named mantle UMB seam. |
| `52:09` | `xmsNotifyHookI15` | invokes `UpdateKbdInt15(CS,AX)` and returns XMS size in `CX`. | INT 15 keyboard-hook registration. | Direct source + named mantle machine/BIOS hook seam. |
| `52:0A` | `xmsQueryFreeExtMem` | free/largest KiB in `AX`/`DX` as emitted by original code. | OpenNT suballocator state. | Direct source + suballocator/mantle backing seam. |
| `52:0B` | `xmsReallocBlock` | base/old/new size; result base/zero in `CX`. | OpenNT suballocator and backing move. | Direct source + suballocator/mantle backing seam. |

## Ownership and failure conclusions

1. **A20, RAM copies and allocation backing stay in bx-mantle.**  Existing
   `bx_ntvdm_a20_capability_v1` and `bx_ntvdm_extended_memory_v1` show that
   this mechanical boundary exists, but their fixed slots, results and
   handwritten allocation bookkeeping are not XMS semantics.  S2 may adapt
   their mechanics behind source-shaped callbacks; it may not make them the
   product XMS provider.
2. **UMB and INT 15 cannot be fabricated by bx-vdm.**  The original `nt_umb.c`
   depends on old SAS/VDD/private-NT composition and is not eligible for direct
   linkage.  S2 must obtain an explicit bx-mantle capability contract or give
   the relevant service the original, observable unavailable/failure outcome.
   It must not create an adapter UMB list or BIOS hook merely to mark the
   endpoint successful.
3. **The old XMS/DPMI classifier is shared historical material.**  XMS v1
   files and their tests must be deleted only after S2 moves all XMS ownership
   out of them and preserves DPMI's still-unrecovered classification as
   DPMI-owned historical evidence.  S1 therefore forbids a blind deletion of
   `bx_ntvdm_xms_dpmi_plane_v1.*`.
4. **No private NT4 ABI is admitted.**  `NtAllocateVirtualMemory`,
   `NtFreeVirtualMemory`, SAS pointer conversion and the old CCPU caller are
   source evidence, not current link inputs.  S2's seams must use public
   Win32 where OpenNT did, or fixed typed mantle/adapter ABI where the old
   product used private state.

## S2 admitted implementation boundary

S2 may create `src/bx-vdm/bop/opennt/xms/` for the OpenNT-shaped XMS files and
`src/bx-vdm/bop/shim/` for the explicitly named pointer, backing-memory, UMB
and INT-15 seams.  Every divergence in a mirrored source file must have an
adjacent reason comment.  It may add no BOP outside `52:00..0B`, no Bochs
selector knowledge, no bx-vdm allocator, and no private NT import.

## Dual-role review

The implementation review found an attractive but invalid shortcut: promote
the old `bop-v1` fixed-slot allocator into the provider.  The independent
review rejects it because it omits `52:06..09`, changes allocation identity
and leaves the original free-list/lifecycle unrepresented.  The approved
route is the full source-shaped package above.

## S1 exit

All twelve endpoints now have an original implementation, ABI, required
mechanics, owner and S2 disposition.  The only unresolved prerequisites
(UMB and INT 15) have named native owners and explicit non-fabrication rules.
T237 S2 may now be admitted as the complete package recovery, followed by
v1 retirement and regression in S3.
