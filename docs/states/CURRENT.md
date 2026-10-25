# Project Status

## Current Work

**Active: M0 T454 S1**

The owner has directed automatic sequential closure and advancement. T454
admits Queue row W05, WOW32 resource discovery and loading foundation, after
T453 W04 closure. This packet follows the read-only `ntvdm64-wow32-audit`:
recover maximum selected original behavior through the smallest source-shaped
binding, with no guest modification or rebuild.

## Active Packet

### M0 T454 S1 — resource lookup boundary

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T454 S1, Ordinary Mode; single-agent implementation and independent final review. |
| Admission And Approval | Owner approved the Queue's first WOW32 package and later directed that every S automatically close and advance. T452 W03 supplies the selected module, allocation and alias boundary; T453 is closed. |
| Objective | Produce the finite original-owner matrix for FindResource, LoadResource, LockResource, UnlockResource and FreeResource: guest names/IDs, module association, bounds, normal/failure/retirement ownership and selected real consumers. |
| Non-goals | No guest changes/rebuild; no resource-specific drawing, dialog/menu implementation, second resource manager, new USER state, generic memory manager, PE-resource substitution, or production repair before the matrix proves a bounded edge. |
| Reference Baseline | T452 closure; W05 [proposal](../proposals/proposal-wow32-resource-loading-001.md); [T454 plan](../etc/operations/t454-resource-loading-plan.md); the read-only audit's `free-resource-direction-contract.md`; selected original `wow16/kernel31/resaux.asm`, `wow16/kernel31/wow16cal.asm`, `wow32/wres16.c` and `wow32/wkernel.c`. |
| Files And ABI Surface | Expected review surface is the original WOW16 callback ABI, `wres16.c` RES list and W03 alias/module providers; S1 may add only plan/evidence documentation and test inventory. Guest ABI and product source remain unchanged. |
| Applicable Rules | Execution, architecture, coding, documentation, source policy and WOW32 minimum-diff admission governance. |
| Verification | Static caller/source/ABI review; compare the two pinned original editions where needed; identify existing test entrypoints and real guest consumers. No build/test is claimed as runtime proof in this review-only S. |
| Expected Markers | One authority per guest resource count, RES record, converted/aliased data pointer and native cursor/icon identity; every changed future edge has a normal, failure and retirement route. |
| Asset Needs | Pinned OpenNT/OpenNT-4.5 audit, immutable guest media and existing W03/W05 test sources; no imported source or replacement media. |
| Reporting Requirements | Separate source facts from unproved runtime behavior; record exact original owner/build condition/ABI, selected consumers, unresolved bounded edges, mirror-diff budget and successor ownership. |
| Stop Conditions | Need for guest change, unbounded original import, replacement resource/memory/task owner, required new scheduler/USER authority, or a semantic concession not already approved. |
| Exit Criteria | A finite matrix defines the S2 load/lock closure and S3 free/rollback closure with exact tests and no unowned shared state. |
| Original Owner Request | 准入队列第一条；按照 ntvdm64-wow32-audit 审计结果执行最小化 diff 和最大化复通。随后要求每个 S 自动收口并继续。 |
| Similar-Issue Sweep | NULL/numeric/string names and types; same-module and cross-module association; failed/partial load; aliases versus converted buffers; duplicate loads; guest/global/module/task retirement; cursor/icon callback direction and return normalization. |
| Candidate Proposal | [Resource discovery and loading foundation](../proposals/proposal-wow32-resource-loading-001.md). |

S1 is intentionally review-only. Its matrix must establish the bounded
implementation group before S2 changes production source. The full four-S
sequence is in the [T454 plan](../etc/operations/t454-resource-loading-plan.md).

## T453 S1 Closure Record

Static source, caller, authority and test-entrypoint closure is recorded in
[the T453 S1 owner matrix](../etc/evidence/m0-t453-s1-files-environment-oem-owner-matrix.md).
It admits S2's finite consumer verification/repair boundary; it does not claim
runtime package completion.

## T453 S2 Closure Record

S2's source-shaped `WK32FileSetAttributes` cleanup repair, product/provider
fixtures, full component build, product regression, coherent ten-image publication
and post-publication OEM witness are recorded in [the S2 runtime
witness](../etc/evidence/m0-t453-s2-oem-runtime-witness.md). The delivery is
`9bf66f914dceac0ffb75c004835e1c990ef05a3d`; it leaves the remaining S3 consumer
and fallback paths intentionally unclaimed.

## T453 S3 Closure Record

S3 found no product-source defect. It repaired only the owned fixture closure
needed to exercise the current session-aware root provider, then passed the
entire bounded non-ASCII/font/PIF group. [The S3 OEM/font fallback
witness](../etc/evidence/m0-t453-s3-oem-font-fallback-witness.md) records the
original ownership and evidence. It admits S4 package acceptance; it does not
publish a new product image.

## Current Technical Baseline

T452 S2 `0f8487f04` remains the W03 coherent ten-image delivery. T452 S3/S4
made no product-source or deployment change; their current production-object
and formal evidence is indexed in `docs/etc/README.md`.

## Successor Boundary

W05 is active. W07 GDI/DC/drawing remains the next unadmitted Queue candidate.
Resource-specific menu/dialog conversion and drawing are later owners, not S1
scope.

## Temporary Evidence

T454 may use only declared `build/t454-s1`, `build/t454-s2`, `build/t454-s3`
and `build/t454-s4` roots for disposable evidence; stable `.ninja` and
`.ccache` remain reusable caches. Retain durable results before exact-target
cleanup; never use a task root as a build cache.

`build/M0-T451`, `build/t451-s3`, `build/t451-s4` and `build/t451-s5` remain
temporary evidence, not reusable caches. Process inventory access was denied
during closure; deletion requires verified quiescence and the existing
exact-target ownership checks. Durable results are linked from the closure.
Stable `.ninja` and `.ccache` caches remain untouched.

## Recent M0 Closures

- [T453](../history/m0-t453-closure.md): files, directories, environment and OEM.
- T452: modules, memory and aliases.
- [T451](../history/m0-t451-closure.md): USER objects/shared view and CUI handoff.
- [T450](../history/m0-t450-closure.md): style removal and Run16 entry.
- [T449](../history/m0-t449-closure.md): WOW32 execution continuation.
- [T448](../history/m0-t448-closure.md): lifecycle.

## Recent Governance

T453 closes W04 without reordering the successor queue. Its S2 product
delivery remains the current verified release; S3/S4 changed no product source,
guest media or release asset.
