# M0 T405 source-diff audit and owner-directed handoff

## Decision and request ledger

On 2026-09-12 the owner requested: “好的，现在可以收口当前T任务清理工作区吗”.
After the approved candidate planning through 8f7f6ec3d, S3 and T405 close
as completed static audit with restoration transferred to the receivers below.
This is an owner-directed scope change, not satisfaction of the original
implementation/runtime exit criteria. No next numeric packet is admitted.

The [original proposal](m0-t405-source-diff-audit-proposal.md) required audit
followed by repair. The subsequent request “你审计完成了，就停下来，告诉我结论，当前所有diff和overlay和原始opennt有重复的点位” required discussion before repairs,
including overlays and successful COMMAND/EDIT paths. Candidate planning and
this closure transfer, rather than cancel or declare completed, those repairs.

## Brief, results and evidence

One agent executed then reviewed sequentially. Audit scope covered MVDM and
non-MVDM mirrors, overlays, SoftPC patches, adapters, app/session/broker and
declarations. Source baseline: 3d127962c82133cb6adad50f52733250bac5f11a,
including unaccepted 1daff0ace WIP. External sources remained read-only.
Original owners, selected providers, ordering/layout/failure semantics and
separate footprint counts were required. Source changes and runtime repair
were excluded from S1-S3. Ambiguous evidence remained unresolved; source,
architecture, execution and documentation rules remained applicable.

| Requirement / stage | Result and evidence |
| --- | --- |
| S1 baseline | [Inventory](../etc/evidence/m0-t405-s1-audit-baseline-001.md): 5,043 tracked inputs, identities and counting rules. |
| S2 MVDM/overlay coverage | [Combined audit](../etc/evidence/opennt-replacement-audit-20260912.md): 174 MVDM text-different paths, seven overlay source/header carriers and five patch carriers with dispositions. |
| S3 cross-root consolidation | Same audit: D01-D37 functional-overlap discussion rows, separately eight unresolved families; original owners and selected graphs joined without double counting. |
| Stop for owner discussion | No S4/S5 restoration or S6 integrated recovery performed; planned outcomes transfer below. |
| Preservation and delivery | Audit through dce6b1e01 and governance through 8f7f6ec3d retained; archive the proposal and preserve pending U02 planning without deletion or stash. |

The [S plan](../etc/operations/m0-t405-audit-restoration-plan.md) is historical.
37 rows are not 37 proven bugs, independent implementations or safe deletions.
The eight unresolved families are not proved duplicates. Graph selection does
not prove runtime use; static coverage is not a guarantee of every semantic bug.

## Restoration receivers

Queue owns current order; these closure receivers are unnumbered candidates.
They own implementation plus verification, not a repeat report-only audit.

| Findings | Receiver |
| --- | --- |
| U02 | [Physical mapping](../proposals/proposal-physical-memory-mapping-restoration-001.md). |
| D35-D37; U03/U04; U07-M | [DPMI/XMS](../proposals/proposal-dpmi-xms-contract-restoration-001.md). |
| D11-D16, D26; U07-E | [RTL/encoding](../proposals/proposal-rtl-character-conversion-restoration-001.md). |
| D08, D31-D33; U07-F | [DOS file services](../proposals/proposal-dos-file-service-restoration-001.md). |
| D09, D23-D25, D28-D30, D34; U05/U08-C | [Console/resources](../proposals/proposal-console-input-display-resource-restoration-001.md). |
| U01; U07-T/U08-W | [CCPU40 C-VID/events](../proposals/proposal-ccpu40-cvid-event-path-restoration-001.md). |
| D01-D07, D10; U07-B/U08-B | [Broker/Base lifecycle](../proposals/proposal-cross-process-broker-closure-001.md). |
| D17-D22, D27; U06/U07-C/U08-S | [WOW/debugger](../proposals/proposal-wow-debugger-original-owner-restoration-001.md). |
| Integrated WRITE/release proof | [WOW16 workload](../proposals/proposal-wow16-single-process-lifecycle-recovery-001.md), then [release matrix](../proposals/proposal-multiprocess-release-matrix-001.md). |

Each D identifier has one receiver group. U07/U08 subitems have unique owners
and consumer regressions. Other thin bindings, diagnostics, inactive carriers
and unresolved subcases remain with the reached original-owner candidate;
this closure does not blanket-approve their retention. A proven prerequisite
must be explicitly promoted; a mandatory residual needs a receiver and owner
disposition, never silent transfer to WRITE. Broker cannot acquire worker logic.

## Workspace and runtime

The sole pre-existing pending change was 72 added lines of U02 EMS/DIB
research direction and proposed tests. It is preserved as candidate planning,
including conditional reachability and bounded-search limitations. Closure
reviews its scope/consistency, not independently re-proving that research or
approving its implementation. No source, media, ignored build, runtime log,
stash or other worktree is deleted. Git cleanliness means no tracked/untracked
changes and no unpushed commits, not an empty build tree.

The existing deployed x86 ntvdm32.exe remains SHA-256
27F8D7B5BB074838E9484877954A38DCBCFA0BE64282A4A2D05C776867655107,
from 2026-09-12 09:15. No new build/test/deployment occurs in this closure.
Historical mouse acceptance remains preserved; complete WRITE remains unproved.

## Verification and review

Inspect Git status/diff, reconcile all D/U receivers, archive the proposal,
repair links and stale active-state prose, run documentation governance with
relative-link/anchor checks and git diff --check, review the staged scope,
commit/push, then verify remote identity and empty status. These are document
checks, not runtime acceptance. A failed push remains pending delivery.

Documentation governance (including relative links/anchors) and git diff
--check passed before delivery. Review found no unassigned named D/U family
and no product change; remote identity and clean status are checked after push.

The closure review distinguishes inventory, compilation selection, runtime
acceptance and repair ownership. Candidate exit clauses prevent a passing
fixture or WRITE launch from silently closing unrelated contracts. Original
audit evidence and unaccepted WIP remain available to the next admitted task.
