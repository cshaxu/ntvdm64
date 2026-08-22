# M0 T237 S4 — manual XMS source-mirror audit

## Method

This is a manual source/route review of committed `4a711706`, not an
inference from a passing fixture.  Each `xmsdisp.c` table slot was compared to
its OpenNT source owner, the current mirror, the bx-vdm ingress condition and
the focused fixture.  The audit also searched the former v1 provider names,
the formal manifest and retained generic catalog metadata.

## Per-service result

| BOP | Original owner/body | Current route and manual result | Evidence / unresolved owner |
| --- | --- | --- | --- |
| `52:00` | `xmsa20.c:xmsA20` | Source body runs only while mantle A20 lifecycle is active; otherwise it declines before dispatch. | Real-minimal-machine A20 fixture proves off/query/on/query. |
| `52:01` | `xmsblock.c:xmsMoveBlock` | Source control point retained; x64 replaces raw VDM pointer arithmetic with checked descriptor/RAM copy. | Source fixture verifies copied bytes. |
| `52:02` | `xmsblock.c:xmsAllocBlock` | Original suballocator route and source register result. | Source allocation/free/reallocation fixture. |
| `52:03` | `xmsblock.c:xmsFreeBlock` | Original `SAFree` route and result. | Source allocation/free fixture. |
| `52:04` | `xmsmisc.c:xmsSysPageSize` | Original public Win32 page-size body. | Source fixture. |
| `52:05` | `xmsmisc.c:xmsQueryExtMem` | Original configured-XMS-size body. | Source fixture and copied generic `C4 C4 52 05` resume witness. |
| `52:06` | `xmsumb.c:xmsInitUMB` | Explicit ingress decline: original operation publishes an `AX:BX` HIMEM-state byte through a raw pointer, which is not admitted across the x64 boundary. | Requires mantle UMB reservation plus a checked durable guest-byte seam. |
| `52:07` | `xmsumb.c:xmsRequestUMB` | Original source body runs against an empty native UMB reservation input and returns original `BL=B1` failure. | Source fixture. Successful reservation remains mantle-owned. |
| `52:08` | `xmsumb.c:xmsReleaseUMB` | Original source body runs against the empty list and returns original `BL=B2` failure. | Source fixture. Successful lifecycle remains mantle-owned. |
| `52:09` | `xmsmisc.c:xmsNotifyHookI15` | Explicit ingress decline: source calls keyboard `UpdateKbdInt15` with no source error result. | Requires keyboard/BIOS/IVT owner; no no-op is retained. |
| `52:0A` | `xmsblock.c:xmsQueryFreeExtMem` | Original suballocator query body. | Source fixture. |
| `52:0B` | `xmsblock.c:xmsReallocBlock` | Original realloc body; only equal-size result initialization is documented to avoid undefined MSVC behavior. | Source fixture. |

## Divergence and ownership findings

1. All source files retain their OpenNT function/table layout.  Direct-mirror
   headers and adjacent `DIVERGENCE (T237)` comments identify every changed
   source point.
2. The sole behavior-changing x64 seam is `52:01` checked guest RAM: raw host
   pointer conversion is not allowed.  It preserves the source forward-copy
   contract.
3. `52:06` is intentionally not the same as `52:07/08`: it cannot safely
   enter the source body without a durable pointer capability; `52:07/08`
   can enter the source with an empty reservation map and preserve its native
   failure codes.
4. The retained `bop-v1` references to selector `0x52` are only generic
   inventory/catalog metadata.  The formal manifest has no `bop-v1` input;
   neither metadata path is an executable XMS provider or fallback.
5. `Test-T237S3XmsV1Retirement.ps1` confirms that all named handwritten XMS
   provider/session/mixed-classifier inputs and fixtures are absent.

## Outcome

The audit confirms the original S3 result with the above XMS-source precision.
`52:06..08` and `52:09` remain named cross-owner limits, not XMS-v1 gaps or
completed guest-machine behavior.  The separate machine-boundary defect found
by the broader sweep is corrected below.

## Machine-boundary correction

The related formal-machine sweep found two independent ownership defects that
would otherwise make the preceding conclusion misleading:

1. `bx_ntvdm_engine_run_v1` and its copied engine contract were compiled into
   `bx-mantle` while directly binding DEM and COMMAND runtime sessions.  They
   are composition code, not a mechanical Bochs lifecycle.  They are moved to
   `bx-vdm`; the mantle now receives only the prepared selector-blind machine
   stage records.
2. `bx_ntvdm_extended_memory_v1` independently implemented handles, first-fit
   allocation, reallocation, free-space accounting, and guest-address policy
   inside `bx-mantle`.  It was not used by the admitted XMS mirror, whose
   original `suballoc.c` already owns that policy.  The duplicate standalone
   implementation and its fixture are removed rather than renamed as a
   purportedly generic machine feature.

The new `Test-T237S4MachineSemanticBoundary.ps1` checks the formal manifest
and all formal core/mantle sources for these service terms.  A future UMB
implementation may request a selector-blind physical-span capability, but it
must be separately admitted with no XMS, UMB, DOS, BOP, handle or allocation
policy in `bx-core` or `bx-mantle`.
