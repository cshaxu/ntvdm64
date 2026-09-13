# M0 T407 S1 DPMI/XMS current-state audit

## Question and boundary

Which current source differences implement D35–D37 and their explicitly
assigned U03, U04 and shared U07-M dependencies; which duplicate an available
OpenNT algorithm; and what is the smallest safe restoration sequence?

This is an admission-stage source/build audit, not runtime-failure diagnosis
or repair authorization.  It covers the admitted [DPMI/XMS proposal](../../proposals/proposal-dpmi-xms-contract-restoration-001.md), not every
changed line in the broad DPMI32 package.

## Inputs and procedure

- Frozen local baseline: `14d937ca3` (T406 closure).
- Read-only original owner: `O:\repos.external\OpenNT\base\mvdm`, with
  matching paths `dpmi32/{modesw.c,dpmimemr.c,dpmi32.c,dpmiselr.c,data.c,i386/dpmi386.c}`,
  `xms.486/{xms.c,xms.h,xmsblock.c,xmsmemr.c,i386/xmsmem86.c}`, and CCPU
  `softpc.new/base/ccpu386/{c_seg.c,c_intr.c}`.
- Compared every matching local file with `git diff --no-index --numstat` and
  read the full diffs for `modesw.c`, `dpmimemr.c`, `xmsblock.c`, `xms.c`,
  `xms.h`, `dpmi32.c`, `dpmiselr.c`, `data.c`, `c_seg.c`, and `c_intr.c`.
- Searched definitions/callers and the selected
  `build/governance-x86-target-check/build.ninja`.  It compiles
  `modesw.c`, `dpmimemr.c`, `xmsblock.c`, `dpmi32.c`, `dpmiint.c`,
  `dpmiselr.c`, `data.c`, `c_seg.c`, `c_intr.c`, the XMS adapter, the
  effective-address adapter, and the shared guest-memory adapter.
- Searched `mvdm-host-overlay` and `mvdm-softpc-patch` for this family.  No
  DPMI/XMS-private overlay is selected.  `mvdm-host-overlay/dos/dem/demfastio.c`
  is a shared-U07 lease consumer; it is not a D35–D37 implementation and
  remains owned by the queued DOS file-service package.

The prior broad audit is supporting evidence, not a substitute for these
fresh comparisons: [XMS direct-diff coverage](opennt-replacement-audit-20260912.md#t405-xms-direct-diff-and-replacement-coverage) and
[DPMI platform follow-up](opennt-replacement-audit-20260912.md#t405-dpmi-transition-ownership-follow-up).

## Observed diff ledger

The following are gross original/current line counts, not promised net code
removal.  They include comments and declarations so a later repair must use
the actual hunk, not this table, for deletion accounting.

| Area | Current locations and gross diff | Original owner / current reachability | Four-rung disposition and cleanup decision |
| --- | --- | --- | --- |
| D36 block service | `xms.486/xmsblock.c` `+9/-30`; `adapter-mvdm-host-out/softpc/mvdm_xms_memory.c` 98 authored lines | Original `xmsMoveBlock` parses the `SS:BP` frame and performs a forward `RtlCopyMemory`; both mirror and adapter are compiled. | **Duplicated algorithm.** The adapter independently parses the same 12-byte frame and supplies its own chunk-copy/overlap policy. Restore the original frame parsing and `setAX(1)` path in `xmsblock.c`; retain only one same-shaped numeric range/lease transfer beneath the original call. Delete the adapter parser and its XMS-specific cancellation policy once the focused partial-failure contract is proved. |
| D36 backend callbacks | Adapter `xmsCommitBlock`, `xmsDecommitBlock`, `xmsMoveMemory`; `xms.c` `+8/-4`; `xms.h` `+5/-1` | `i386/xmsmem86.c` is exact but unselected; it calls NT VM APIs on direct host aliases. `xmsmemr.c` is exact but unselected RISC/SAS variant. | **Necessary finite binding, not direct-composable source.** The selected CCPU40 guest address cannot be a durable NT process pointer. Preserve original callback names/signature/order; replace zero-on-commit/decommit and chunked move policy only after selecting the exact CCPU40 observable contract. No generic lease manager or direct pointer backend may be introduced. |
| D37 allocation owner | `dpmi32/dpmimemr.c` `+30/-4` | Original function already contains `VdmAllocate/Free/Reallocate/Query` then `STATUS_NOT_IMPLEMENTED` SA fallback. It is compiled and reached by `xmem.c`, DPMI initialization, and the local task carrier. | **Original algorithm with changed provider selection.** Remove the CPU40 early SA returns only if the original Vdm provider can be restored through a finite OpenNT-shaped binding and its result means the same thing as XMS ownership. Until then retain this narrow divergence, but make allocation/free/reallocate/query use one proved owner and prove no double allocation. This is not a new allocator to delete wholesale. |
| D35 frame carrier | `dpmi32/modesw.c` `+317/-6`; declarations in `dpmidata.h` `+28`; related BOP selection | Original `dpmi32/i386/dpmi386.c` supplies `switch_to_protected_mode`/`switch_to_real_mode`, but also writes fixed kernel-VDM state and assumes passive `CONTEXT` setters. Local frame consumers are actively called by BOP FD and DPMI paths. | **Source-derived rehost plus necessary CCPU seam.** Move the two original frame algorithms to the foreground: preserve frame offsets, visible registers, `PE/CPL` and failure direction, while one small CCPU cache-order binding performs each active-setter operation. Delete frame parsing/reordering that has no source counterpart; do not copy kernel RI/RM writes or claim them implemented. |
| U04 platform projections | `dpmi32.c` `+78/-1`, `dpmiselr.c` `+11/-1`, `data.c` `+19`, parts of `modesw.c` (TSS/IDT helpers), `dpmiint.c` CPU40 additions | 53:00 source publication is original; no audited original owner has yet supplied the local TSS slots `01f0/01f8`, inferred IDT base, shared GDT/LDT backing, FastWOW TEB prefix, or projected PM stack. All are selected in the formal graph. | **Unresolved autonomous platform state.** Do not delete or normalize it as part of D35. First make one row per projection with original owner, consumer, initialization/reset/teardown and negative behavior. A directly reusable kernel semantic carrier may enter `opennt-host` only after the package/finite-ABI audit; otherwise retain the smallest explicit compatibility seam with a receiver. |
| U03 effective address and selector cache | `c_seg.c` `+48`; `adapter-mvdm-host-out/softpc/mvdm_softpc_effective_address.c` 90 authored lines | Original CCPU descriptor walker is reused, but adapter chooses the first matching live CS/SS/DS/ES/FS/GS cache before descriptor memory. | **Unresolved selection policy.** The adapter is not a second descriptor decoder, but selector equality does not prove cache equivalence. Keep the numeric facade temporarily; audit every caller and prove when active cache versus descriptor memory is intended. Remove the `0040h` cache bypass only when its descriptor is established by a proved owner; no CCPU instruction change is authorized. |
| U07-M shared lease | `mvdm_softpc_guest_memory.c` 123 authored lines; XMS adapter uses it; `nt_emm.c` and `demfastio.c` are other selected/reached consumers | Original XMS used direct aliases. The current lease is bounded to a thread-bound session and delegates byte transport to original CCPU SAS loads/stores. | **Necessary shared finite binding with duplicate-policy risk.** Do not create a second XMS lease or make this a generic provider. S2 must test all consumers for access direction, span, overlap, partial write, failed release and teardown. XMS may keep only a thin use of this shared primitive; its current descriptor parsing, zero policy and cancellation are not justified by the lease itself. |

Outside the table, DPMI32 has 13 text-different matching files totaling
`+607/-33`; XMS has four totaling `+22/-36`.  Those package-wide figures are
an upper bound, not T407's cleanup total: diagnostics, public-header
compatibility and unrelated interrupt observations are not silently assigned
to D35–D37.  The directly observed D36 duplication is the 98-line adapter
plus its `xmsblock.c` replacement; D35/U04 is larger but cannot honestly be
counted as deletable until original owners for the projection group are found.

## Proposed restoration groups

1. **XMS contract group (D36/D37):** pair `xmsblock.c`, callback binding,
   `xms.c`, `xms.h`, and `dpmimemr.c`.  Prove one allocator ownership and
   preserve original copy-versus-overlap behavior.  This group is the only
   current candidate for immediate deletion of duplicated policy: parser,
   chunk-copy and XMS-specific cancellation after the thin lease call exists.
2. **D35 frame group:** rehost `dpmi386.c` frame order through one bounded
   active-CCPU setter seam.  Test both directions, malformed frames and BOP
   instruction advancement.  Keep U04 mechanisms out of this deletion patch.
3. **U04/U03 disposition group:** resolve platform-projection owners and the
   effective-address cache-choice rule before any rollback.  Each retained
   exceptional seam must state reset/teardown and a future receiver; an
   unresolved kernel dependency stops removal rather than licensing a new
   implementation.
4. **U07 integration group:** run positive and negative lease tests across
   XMS, EMS and DEM consumers.  Only then may D36 remove duplicate adapter
   mechanics.  COMMAND/EDIT are regressions; DOSX/WOW bootstrap is boundary
   evidence, not proof that the whole platform is correct.

## Confidence, limitation and follow-up

Confidence is high for the selected-build membership, exact line counts, the
duplicated D36 parser/copy policy, and the existence of the original
allocation/frame owners.  It is deliberately low for a net deletion number
for U03/U04: existing TSS/IDT/GDT/LDT/TEB state has no identified complete
original standalone owner.  The next S must not repair it until that source
and finite-boundary audit is complete.

S1 therefore recommends the four groups above, with D36/D37 first.  It does
not assert an executable improvement, build result, or real-program hit.
