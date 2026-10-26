# Project Status

## Current Work

**No active M/T/S packet.** M0 T460 is closed; see the
[T460 closure](../history/m0-t460-closure.md). Queue item 1, WOW32 shell and
layered Registry, is the next unadmitted candidate.

The retained T460 details below are archival closure context, not authority to
start or continue an implementation packet.

Owner requests standalone Win95 launch/path toolkits on 2026-10-25, based on
copies of win31-launch/win31-path. Admit tools/win95-launch, tools/win95-path,
independent add-on producers, arbitrary DOS-safe installation-root support,
identity-bound recoverable VMM32 consumer adaptations, driver/profile/version
installation and INI/PIF/LNK/GRP/registry relocation. Original media remains
read-only. No host/core NTIO/NTDOS change or Computer Use. Use disposable
`build/t460-s3/win95-toolkit-owner` (90 minutes, 2 GiB, 10-minute no-progress
budget); verify alternate-root install, rollback/failure safety and original
Win95 runtime before delivery. Preserve unrelated main-session work; no joint
commit from this standalone work until the owner's main session collects it.

Automatic sequential S closure/advancement is approved. Completed work follows
the read-only `ntvdm64-wow32-audit`: recover selected original behavior with
the smallest source-shaped binding and no guest modification or rebuild.

The independent Video7, packed-surface and Win95 add-on/tool work is bounded
outside S3 core closure. Their source register, published/recovery identities,
and explicit Win95 launch limits are in `src/mvdm/README.md`,
[the paging record](../etc/evidence/m0-t460-s3-win95-paging-research.md) and
[the add-on audit](../etc/evidence/m0-t460-s3-addon-tool-audit.md). Original
media remains read-only; no such record upgrades this Clipboard/PIC/keyboard S.
Clipboard callback source tracing identified twelve stray WORD arguments in
the independently authored probe's zero-argument Close/EmptyClipboard calls.
Removing them passes owner-destroy callback return and all eighteen shared
Clipboard transfer/deferred/replacement phases; the guarded version also
passes. [The S3 evidence](../etc/evidence/m0-t460-s3-clipboard-deferred-lifetime.md)
supersedes the unproved continuation-failure hypothesis. Final coherent-package
acceptance, review and joint submission remain open.
The independent add-on/tool audit was pushed as `6048075f4`.
The frozen-source candidate now passes thirteen-phase owner destruction,
seven-phase task exit and the unchanged eighteen-phase full Clipboard group,
24 PIC/RTC/mouse assertions, 46 keyboard assertions and noninteractive
regressions. Owner clarified that probe-driven interaction is allowed:
reuse the successful private-desktop DOS17/Window17 and gameplay runners,
without Computer Use. The candidate-only submission question is superseded;
complete the ordinary package gates before publication and S3 closure.
The first full run passes both seventeen-case command matrices, six RPC
groups and GUI routing, then correctly rejects the stale `0.0.451` product
identity. The shared version is advanced to admitted T460 (`0.0.460`), without
changing protocol 65. All affected components have been incrementally rebuilt
into `build/t460-s3/versioned-package`; acceptance is rerun against that exact
package. The first gameplay invocation's relative compiler path failed before
execution; both runners now resolve their compiler/observer paths before
changing directory. Neither earlier failure is counted as a pass.
R4 rebuilt the stale monitor fixture and exposed only external global-Clipboard
contention: `RadeonSoftware.exe` owned it during two fresh attempts. R5 closes
a return-to-input asymmetry by reusing the existing shared-view snapshot for
public Clipboard operands. After verified unrelated Clipboard owner
`logioptionsplus_agent.exe` was stopped, the complete 18-checkpoint shared
transfer passes (`guest_to_native`, `native_to_guest`, rejection, deferred
render and replacement all succeed). R5's five cached builds, focused suite,
complete product gate and WINMINE/SOL probes pass; its ten images are published
to `assets/release` and `O:\winnt\system32` with matching SHA-256 manifests.
S3 is closed at `5e2e8bfaf`; S4 now owns the remaining selected-format and
full lifetime acceptance, without reopening S3's completed boundaries.
## Archived T460 Packet

### M0 T460 S4 — Clipboard complete selected-format and lifetime acceptance

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T460 S4, Ordinary Mode; single-agent implementation and independent final review. |
| Admission And Approval | Automatic continuation under the admitted Queue W10 Clipboard proposal, after S3 closure at `5e2e8bfaf`. |
| Objective | Prove all selected Clipboard formats and their normal, rejection, replacement, deferred-render and task-loss lifetimes through production callers; hand only proved reusable boundary facts to the later DDE package. |
| Non-goals | No guest change/rebuild; no Clipboard manager/cache/listener/DDE substitute; no unrelated Win95/video/device expansion; no claim that a selected-format acceptance closes DDE. |
| Reference Baseline | [S1 owner matrix](../etc/evidence/m0-t460-s1-clipboard-owner-matrix.md), [Clipboard proposal](../proposals/proposal-wow32-clipboard-001.md), [S3 lifetime evidence](../etc/evidence/m0-t460-s3-clipboard-deferred-lifetime.md), and WOW32 minimum-diff governance. |
| Files And ABI Surface | Original `wuclip.c` format/ownership callers, original guest/native conversion and USER public boundary only; immutable guest/native fixtures. |
| Applicable Rules | Applicable execution, architecture, coding, documentation, source-policy and WOW32 minimum-diff rules. |
| Verification | Repeated real guest/native selected-format exchanges; open/set/get/close, rejected/abandoned data, owner replacement, deferred rendering and task loss; targeted error/cleanup evidence and complete package regression for a production change. |
| Expected Markers | Each selected format has one original conversion/ownership owner; native and guest results agree; callbacks retire once; dead/replaced owners neither leak buffers nor receive callbacks. |
| Asset Needs | Existing immutable guest media, mirrors and fixtures only; no Win95 image or new guest artifact. |
| Reporting Requirements | Per-format source/ownership matrix, failure/lifetime disposition, production diff and package hashes or explicit no-change result, and precise DDE handoff facts. |
| Stop Conditions | Guest change; Clipboard substitute; a new cache/listener/manager; DDE implementation; source-less conversion; or a failed/rejected/retired path silently counted as a transfer success. |
| Exit Criteria | All selected formats and their stated lifetime/error paths are verified through production callers, or each remaining bounded non-pass is retained explicitly rather than transferred to generic integration. |
| Original Owner Request | 单人双角色模式复通 WOW32；源码追溯优先、最少 diff、禁止 guest 修改。 |
| Similar-Issue Sweep | All selected formats, ownership, callbacks, rejection, replacement, deferred render, task loss and public USER operands/results. |
| Candidate Proposal | [WOW32 clipboard](../proposals/proposal-wow32-clipboard-001.md). |

S4 inherits S3's original-handle projection and narrow public-USER adaptation;
the shared-view snapshot remains an identity projection, not a second Clipboard
state owner. [S3 lifetime evidence](../etc/evidence/m0-t460-s3-clipboard-deferred-lifetime.md)
and [the PIC plan](../etc/evidence/m0-t460-s3-pic-eoi-reset-plan.md) are
closed inputs, not S4 expansion authority.
The selected-format matrix now passes on the shared Clipboard for `CF_TEXT`
and `CF_DIB`, including normal/rejection/replacement/deferred/owner-destroy,
callback-return and task-loss paths.  [S4 evidence](../etc/evidence/m0-t460-s4-clipboard-selected-format-acceptance.md)
records the original-owner audit, cooperative DIB Yield boundary and runner
compatibility correction; no product image or guest media changed.

## S1 Closure Record

T460 S1 closed the original Clipboard source/owner inventory and callback ABI
audit. The [S1 owner matrix](../etc/evidence/m0-t460-s1-clipboard-owner-matrix.md)
records the selected original conversion/cache owners, no autonomous
Clipboard substitute, and finite S2 transfer/S3 delayed-render handoffs.
This is source/composition evidence, not a runtime transfer claim.

## S2 Closure Record

S2 repaired the bounded host-generation invalidation and rejected-handoff
ownership seam while retaining the selected original `wuclip.c` conversion
owners.  Two real guest/native transfer rounds, the original callback ABI and
formal x86 graph, cached component build, all retained WOW frontiers and the
Console-17/Window-17 product gates pass against one published ten-image
package.  [The S2 transfer acceptance](../etc/evidence/m0-t460-s2-clipboard-transfer-acceptance.md)
records the hashes, modern-host exceptions and finite S3 handoff.  Guest media
is unchanged.

## S3 Closure Record

S3 closes at `5e2e8bfaf`. [Its lifetime evidence](../etc/evidence/m0-t460-s3-clipboard-deferred-lifetime.md)
records the selected original `wuclip.c` owners, the live public-USER identity
seam, deferred rendering, owner replacement and task-loss proof. It also
delivers source-proven PIC ICW1 device cancellation and the keyboard
early-EOI/read ordering repair. Five cached build stages, 24 PIC/RTC/mouse and
46 keyboard assertions, focused Clipboard/graph/ABI routes, the complete
product gate and isolated WINMINE/SOL probes pass. Ten matching images are
published; guest media is unchanged. S4 retains selected-format acceptance and
DDE handoff only.

## T459 Closure Record

W18 is closed in [the T459 closure](../history/m0-t459-closure.md). It
preserves original common-dialog and OLE alias authorities, passes real guest
modeless-dialog retirement plus exact alias lifecycle evidence, and records
guest OLE client/server as an explicit immutable stopping boundary. No product
image or guest input changed.

## Closed predecessor evidence

- [T457 closure](../history/m0-t457-closure.md): dialog/input/caret and hook/timer recovery; per-S evidence is indexed there.
- [T456 closure](../history/m0-t456-closure.md): font/text/metafile recovery and retained acceptance limits.
- [T455 closure](../history/m0-t455-closure.md): original GDI/DC/DIB/palette recovery and application observations.
- [T453 closure](../history/m0-t453-closure.md): files, environment and OEM recovery.

Detailed closed-stage chronology belongs to those retained records; the active
packet above remains the only current S authority.

## Current Technical Baseline

T460 S3 `5e2e8bfaf` is the current coherent ten-image delivery: it publishes
the Clipboard deferred-lifetime boundary plus PIC/keyboard device repairs with
rebuilt dependent images. Its release identity is `assets/release/release-manifest.json`;
guest media remains unchanged. T454 S3 `5ede6c0e4` remains the prior bounded
WOW32 free/rollback delivery.

## Successor Boundary

W05 and W07 are closed. W08 font/text/metafiles is active at S1.
Resource-specific menu/dialog conversion remains a later owner.

## Temporary Evidence

T458 S1 may use only `build/t458-s1` for disposable source-audit and focused
resource-fixture evidence, with a 30-minute phase, 2-GiB size and 10-minute
no-progress budget. Stable `.ninja` and `.ccache` remain reusable caches and
are never task evidence roots.

T458 S2 may use only `build/t458-s2` for retained conversion and guest
resource evidence, with a 45-minute phase, 2-GiB size and 10-minute no-progress
budget. Stable `.ninja` and `.ccache` remain reusable caches and are never task
evidence roots.

T458 S4 may use only `build/t458-s4` for disposable consumer acceptance, with
a 45-minute phase, 2-GiB size and 10-minute no-progress budget. Stable
`.ninja` and `.ccache` remain reusable caches and are never task evidence roots.

T459 S1--S4 used `build/t459-s1` through `build/t459-s4` only for disposable
evidence. Their retained results are indexed from the closure; stable `.ninja`
and `.ccache` remain reusable caches and are never task evidence roots.

T460 S3 may use only `build/t460-s3` for disposable delayed-render/loss
evidence, with a 45-minute phase, 2-GiB size and 10-minute no-progress budget. Stable
`.ninja` and `.ccache` remain reusable caches and are never task evidence roots.

T457 S1 may use only `build/t457-s1` for disposable source-audit and focused
fixture evidence. Stable `.ninja` and `.ccache` remain reusable caches and
are never task evidence roots.

T457 S2 may use only `build/t457-s2` for disposable guest-dialog/input/caret
evidence. Stable `.ninja` and `.ccache` remain reusable caches and are never
task evidence roots.

T457 S3 may use only `build/t457-s3` for disposable hook/timer evidence.
Stable `.ninja` and `.ccache` remain reusable caches and are never task
evidence roots.

T457 S4 may use only `build/t457-s4` for disposable integrated acceptance
evidence. Stable `.ninja` and `.ccache` remain reusable caches and are never
task evidence roots.

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

T456 S2 may use only `build/t456-s2` for disposable evidence. Stable `.ninja`
and `.ccache` remain reusable caches and are never task evidence roots.

## Recent M0 Closures

- [T460](../history/m0-t460-closure.md): WOW32 Clipboard selected-format and
  lifetime acceptance, with project-owned PIC and keyboard device repairs.
- [T454](../history/m0-t454-closure.md): WOW32 resource discovery and loading.
- [T453](../history/m0-t453-closure.md): files, directories, environment and OEM.
- T452: modules, memory and aliases.
- [T451](../history/m0-t451-closure.md): USER objects/shared view and CUI handoff.
- [T450](../history/m0-t450-closure.md): style removal and Run16 entry.
- [T449](../history/m0-t449-closure.md): WOW32 execution continuation.
- [T448](../history/m0-t448-closure.md): lifecycle.

## Recent Governance

T460 closure records a current-package metadata comparison and a sealed
source-identical Clipboard witness. Its S4 acceptance changed no product image
or guest media; the later independent package/tool audit is `c1729f5a9`.
