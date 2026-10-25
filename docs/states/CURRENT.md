# Project Status

## Current Work

**Active: M0 T456 S1**

The owner directed automatic sequential S closure and advancement. T455 W07
is closed; T456 admits Queue W08, WOW32 fonts, text and metafiles. The task
follows the read-only `ntvdm64-wow32-audit`: recover maximum selected original
behavior through the smallest source-shaped binding, with no guest modification
or rebuild.

## Active Packet

### M0 T456 S1 — font/text/metafile owner and composition audit

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T456 S1, Ordinary Mode; single-agent implementation and independent final review. |
| Admission And Approval | Owner's standing WOW32 objective and automatic sequential S directive; T455 W07 closure. |
| Objective | Establish the exact original owner/build/lifetime boundary for WOW font conversion, text drawing and metafile operations, including T443 H08 font discovery transfer, before any repair. |
| Non-goals | No guest changes/rebuild; no generic text renderer, second font registry, host-font-to-guest lookup merger, metafile replacement, control/resource conversion, or speculative source edit. |
| Reference Baseline | [T455 closure](../history/m0-t455-closure.md), [W08 proposal](../proposals/proposal-wow32-fonts-text-metafiles-001.md), T443 H08 transfer, W04/W07 evidence and WOW32 minimum-diff governance. |
| Files And ABI Surface | Original WOW32 font/text/metafile units and existing selected providers; no product file is presumed changed. |
| Applicable Rules | Execution, architecture, coding, documentation, source policy and WOW32 minimum-diff admission governance. |
| Verification | Trace dispatch tables and actual build selection; classify original/facade/replacement rungs; inspect lifetime/failure edges and execute only existing non-mutating focused probes needed to distinguish routes. |
| Expected Markers | One authority each for host-font discovery, guest-root lookup, font handles/text conversion and metafile lifetime; every missing dependency has a finite source-shaped boundary. |
| Asset Needs | Existing OpenNT MVDM mirror, formal x86 graph, immutable guest media, T443/W04/W07 evidence and current release package; no new source import or replacement media. |
| Reporting Requirements | Produce an owner/build/lifetime ledger, exact source/build conditions, tests and finite S2 repair handoff; record no-runtime-proof limits precisely. |
| Stop Conditions | Need for guest change, broad USER/GDI server import, second font/metafile authority, source-less renderer, or semantic concession beyond existing original behavior. |
| Exit Criteria | Every reachable selected font/text/metafile family is classified with an original owner or bounded missing dependency; S2 scope is finite and does not hide a provider replacement. |
| Original Owner Request | 单人双角色模式复通 WOW32；优先原版代码追溯与最少 diff，整批接通后再统一验证；每个 S 自动收口并继续。 |
| Similar-Issue Sweep | `WG32CreateFont`/selection/delete, ANSI/OEM/Unicode conversion, text extent/output, host versus guest font search, metafile create/play/delete, task teardown and failure rollback. |
| Candidate Proposal | [Fonts, text and metafiles](../proposals/proposal-wow32-fonts-text-metafiles-001.md). |

## S1 Closure Record

S1 selected the original `walias`/`wgdi`/`wreldc`/`wdib` providers from the
formal x86 graph and proved that one bounded worker alias carrier is the only
necessary substitute for the inaccessible NT4 `GdiQueryTable`. The source,
ownership, lifetime and handoff record is [the S1 GDI owner
matrix](../etc/evidence/m0-t455-s1-gdi-owner-matrix.md). It makes no runtime
acceptance claim and admits the bounded S2 DC/drawing binding batch.

## S2 Closure Record

S2 proved the original `W32Dispatch` → `wowtbl` → `wgtbl2` → `WG32*` route,
including intentionally different immediate-delete and cached-ReleaseDC
retirement semantics. Its cached component build and bounded carrier fixture
found no binding defect; no speculative product change or release was made.
[The S2 DC/drawing evidence](../etc/evidence/m0-t455-s2-dc-dispatch-lifetime.md)
admits S3 DIB/palette work.

## S3 Closure Record

S3 found no product-source defect.  The selected original `wdib.c`/`wgpal.c`
route has one DIB information owner and original callback cleanup; the formal
x86 DIB fixture proved bidirectional native/guest storage sharing, selection
and removal.  [The S3 DIB/palette boundary](../etc/evidence/m0-t455-s3-dib-palette-boundary.md)
records the precise scope and admits only real graphics acceptance in S4.

## S4 and T455 Closure Record

S4 found no GDI/DC/DIB/palette source defect. The physical private-desktop
WINMINE content/lifetime probe passed; WRITE retained all required visible
startup windows over four samples. SOL startup and normal close passed, while
full gameplay is explicitly unverified because the test desktop's maximum
tracking geometry prevents its 585-pixel client width. The full matrix,
test-only x64 type correction and W07 handoff are in [the S4 graphics
acceptance record](../etc/evidence/m0-t455-s4-graphics-acceptance.md) and
[T455 closure](../history/m0-t455-closure.md).

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

T454 S3 `5ede6c0e4` is the current coherent ten-image delivery: it publishes
the bounded WOW32 free/rollback correction with rebuilt dependent product
images. Its release identity and formal evidence are indexed in
`docs/etc/README.md`; guest media remains unchanged. T452 S2 `0f8487f04`
remains the prior W03 allocation/alias foundation delivery.

## Successor Boundary

W05 and W07 are closed. W08 font/text/metafiles is active at S1.
Resource-specific menu/dialog conversion remains a later owner.

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

T455 S2 may use only `build/t455-s2` for disposable evidence. Stable `.ninja`
and `.ccache` remain reusable caches and are never task evidence roots.

T455 S3 may use only `build/t455-s3` for disposable evidence. Stable `.ninja`
and `.ccache` remain reusable caches and are never task evidence roots.

T455 S4 may use only `build/t455-s4` for disposable evidence. Stable `.ninja`
and `.ccache` remain reusable caches and are never task evidence roots.

T456 S1 may use only `build/t456-s1` for disposable evidence. Stable `.ninja`
and `.ccache` remain reusable caches and are never task evidence roots.

## Recent M0 Closures

- [T454](../history/m0-t454-closure.md): WOW32 resource discovery and loading.
- [T453](../history/m0-t453-closure.md): files, directories, environment and OEM.
- T452: modules, memory and aliases.
- [T451](../history/m0-t451-closure.md): USER objects/shared view and CUI handoff.
- [T450](../history/m0-t450-closure.md): style removal and Run16 entry.
- [T449](../history/m0-t449-closure.md): WOW32 execution continuation.
- [T448](../history/m0-t448-closure.md): lifecycle.

## Recent Governance

T454 S4 added only acceptance evidence and closure records. Its S3 product
delivery remains the current verified release; S4 changed no product source,
guest media or release asset.
