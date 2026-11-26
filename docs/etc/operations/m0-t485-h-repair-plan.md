# T485 H-series repair insertion and migration continuation

## Authority and insertion point

Owner direction, 2026-11-26: turn the H001-H036 audit findings into ROI-ordered S packets inside the current T, before resuming N004 and later migration. This plan is supporting detail; [Current](../../states/CURRENT.md) alone selects the active S.

Keep T485 S1-S4 history and S5 N001-N003 deliveries unchanged. S5 completed its P4 DPMI/NTCON repair and Full23/23. S6 completed the selector ownership repair and targeted published-package verification; S7 is now active. Its old N004-N012 remainder moves to S14. Then execute S6-S13 below, followed by S14-S20. Do not start N004 while the repair exit gate is open. This is a split of the current S5 remainder, not an S nested inside another S, and not a new queued T. No S is declared complete by this planning change.

The [audit handoff](../evidence/t485-h-repair/README.md) persists the 11-file pairing findings, all 4,550 point dispositions and 1,039 file identities. Audit disposition is not repair or unconditional approval: insufficient necessity/minimality evidence remains unfinished investigation to resolve inside its assigned S, not a question handed back to the owner.

## ROI and execution order

ROI is qualitative: prevent actual corruption first, establish trustworthy exclusion/measurement boundaries second, take low-risk whole-file reductions next, then close coupled runtime domains and acceptance. Costs include investigation, regression and delivery; point count alone is not value. S7 is deliberately ahead of the cheaper S8 because incorrect masks can hide retained behavior and falsify reductions.

| S | Scope and result | ROI / cost | Required acceptance |
| --- | --- | --- | --- |
| S5 retained | N001-N003 already delivered; finish existing P4 DPMI ownership, rejection-state preservation, NTCON Window retention and Full Alt+Enter round trips. | Existing in-flight correctness work; finish before changing baseline. | Existing P4 production/Full gates and coherent delivery; record that N004-N012 transfers to S14. |
| S6 inserted | Restrict selector 0040 compatibility to the proved DOSX initialization owner/profile; review the whole c_seg.c file. Preserve genuine early FSTI support without overriding ordinary guest GDT descriptors. | Highest: demonstrated CPU-state corruption / medium. | Actual valid GDT0040 negative control, positive early DOSX FSTI, DOS4GW/NESticle startup, Win3.1/Win95 guest transitions; no blanket removal of compatibility. |
| S7 inserted | Repair the 11 exclusion-pairing files plus eight adjacent structural files. Finish every retained difference in each owned file, including dpmiint stack-frame duplication, mouse/video selected bodies and WOW dispatch framing. | High: prevents unsafe deletion and establishes reliable accounting / high. | Original guards/order and diagnostic lines, all supported selections, unsupported-arm negative checks, non-overlapping masks; owner-specific runtime gates for substantive changes. Preserve the latest S5 P4 fixes. |
| S8 inserted | Restore complete files whose remaining differences are format/reference metadata or already evidenced declaration restoration; reconcile zero-point/exact/newline-only entries. | High: broad reduction with low behavioral risk / low-medium. | Exact upstream bytes where normalized exact; equivalent selected preprocessing for subset restorations; real compile/link where declarations/build selections change. Preserve exclusions. Do not claim 852 files required edits. |
| S9 inserted | Remaining CPU/DPMI/BIOS/PIC/LIM/XMS, debug and related headers: retain proved x87/permission/width/BOP fixes, minimize state projections and ABI seams. | High: core execution correctness and duplication reduction / medium-high. | Whole compatibility profile and scalar/RMW-string/stack/fetch groups as applicable, positive/failure paths, DOS4GW and affected guest transitions. Protect original owner and original failure semantics. |
| S10 inserted | Remaining C-VID, keyboard, mouse host bridge and video publication: minimize adaptations while preserving CHAIN2 layout, read/EOI handshake, origin-tagged typeahead and coherent fonts/cursor/palette. | High: existing input/display regression risk / high. | Actual plane oracle and odd/even moves, both read/EOI orderings, key handoff/reset, downloaded fonts, Win3.1 exit and repeated CAF/Alt+Enter in both start routes. No hardware-fullscreen or kernel-monitor reintroduction. |
| S11 inserted | Remaining WOW32/header ABI, callback/resource ownership, public USER/GDI bridges, task lifecycle and profile/environment behavior. Resolve broad _X86_ switching and unconsumed local additions with real consumer evidence. | High: large semantic surface, costly regressions / high. | Build/export/caller ABI; reentrant and failed callbacks, DDE/clipboard allocation rollback, native/guest procedure identity, task cleanup and real Win16 applications. No duplicate state authority. |
| S12 inserted | Remaining COMMAND/DEM/OEM, redirector/VDD, host configuration, packaging and lifecycle boundaries. Resolve unused declaration deletions, cpu_createthread signature consistency, blanket registry error suppression and path/optional-provider fallback. | Medium-high: ABI and failure semantics / medium. | Real caller/provider traces; positive and missing/access-denied cases, OEM/DBCS capacities, captured launch inputs, async cancellation/cleanup, package-root isolation. No writes to native host boot files. |
| S13 inserted | Reconcile every assigned file/point and retained decision against final code; complete final regression and delivery audit. | Mandatory prevention of missed/false closure / low-medium after preceding repairs. | No unreviewed or unfinished points, correct qualified retains, both diff metrics, published final package Full and required guest witnesses, hashes/commits/pushes; then release N004 gate. |
| S14 continuation | N004-N012, the uncompleted remainder of old S5. | Resume original migration order. | Original batch-level source/ABI/guest obligations and delivery cadence. |
| S15 continuation | N013-N026, formerly planned S6. | Original scope retained. | Private/public declarations and USER actual consumers. |
| S16 continuation | G001-G033, formerly planned S7. | Original scope retained. | DOS tools, COMMAND/devices/kernel/headers/redirector and immutable guest qualification. |
| S17 continuation | G034-G048, formerly planned S8. | Original scope retained. | DOSX/SIM16/firmware/guest VDD/utilities and matching host handoffs. |
| S18 continuation | G049-G075, formerly planned S9. | Original scope retained. | First WOW16 group through multimedia, including actual audio call paths. |
| S19 continuation | G076-G097, formerly planned S10. | Original scope retained. | Remaining WOW16 packages; reconcile all 159 original migration batches. |
| S20 continuation | Package/capability closure, formerly planned S11. | Original scope retained. | Audio, LFN, Win95 localized PIF creation and separate restart-to-DOS lifecycle, coherent package, regression and rollback. |

Repair S packets do not increment the 159-directory migration counter: it stays 39/159 until N004 delivers. N004 becomes 40/159 and still triggers the original Full cadence. S13 additionally requires Full irrespective of that counter. Existing G-series strict original-first policy remains unchanged.

## Exclusive whole-file ownership

[files.csv](../evidence/t485-h-repair/files.csv) assigns every audited file once; [points.csv](../evidence/t485-h-repair/points.csv) assigns every snapshot point to the same owner. S13 independently verifies closure, without double-counting file completion. Within an S use relative-path alphabetical order and original point order, after necessary coupled dependencies.

| Primary owner | Audited file population | Snapshot points |
| --- | ---: | ---: |
| S6 | 1 | 1 |
| S7 | 19 | 1,055 |
| S8 | 852 | 900 |
| S9 | 41 | 437 |
| S10 | 30 | 1,060 |
| S11 | 48 | 452 |
| S12 | 48 | 645 |
| Total | 1,039 | 4,550 |

These are coverage assignments, not counts of edits, approved retains, completed files or repaired points. S8 includes byte-exact and zero-point files needing reconciliation only. The 2,889 format-restoration evidence points are spread across mixed semantic files too: they stay with each file's primary S, not automatically in S8.

S7's 11 primary pairing files are dpmi32/{dpmi32.c,dpmiint.c,modesw.c}, inc/wowusr.h, softpc.new/base/{keymouse/mouse_io.c,video/video.c}, softpc.new/host/{inc/host_def.h,src/copy_fnc.c,src/nt_fulsc.c}, softpc.new/obj.vdm/ntvdm.c and wow32/wkgthunk.c. The eight adjacent files are dpmi32/{dpmi32.h,dpmiselr.c,i386/dpmi386.c}, softpc.new/base/bios/reset.c, softpc.new/host/src/{nt_sb.c,nt_sbfm.c,sim32.c}, and wow32/wow32.c. They are not scheduled again as fresh whole-file work in S9-S12.

A required coupled header/provider moves to the earliest dependent S with its entire unfinished file scope and explicit ledger reassignment; do not split formatting from semantic closure, count it twice, or delay a required ABI counterpart to its nominal later S. Record lead delivery and recheck dependent callers at their own turn. New differences or files discovered after the fixed snapshot are appended with source identity and an owner, never hidden by renumbering or dropping original IDs.

## Implementation and evidence requirements

1. Use the existing audit, pinned XP SP1 tree and runtime counterexamples; do not restart a general H-series census. Revalidate evidence only for changed source/configuration or an unresolved claim. Existing control results are supporting evidence, not final candidate acceptance.
2. For every file finish original restoration and residual adaptation minimization together. Unused/unbuilt retained code stays original. Proved excluded bodies keep original effective condition, #error-only body and correct #line. New guards and unconditional omissions are independently accounted differences.
3. Minimize both original-relative point/block count and added-plus-deleted lines. Keep original algorithms and required CCPU behavior. No moving equal complexity into compat, renamed private overlays, duplicate stores or synthetic callbacks merely to improve mirror statistics.
4. c_seg.c profile work must establish owner predicate before implementation and cover all affected mechanics. Existing selector evidence does not authorize an opcode-only workaround. dpmiint.c must retain S5's latest ownership/flag fixes; parameterize the original stack-frame owner rather than retaining a second algorithm where equivalence is proved.
5. Register retained/new differences in the main mirror register and pending decision tracker. A `retain-*` audit label establishes neither approval nor minimum implementation. Resolve unknowns in the assigned S; preserve legitimate fixes while investigating smaller forms. Apply existing net-increase approval thresholds; this plan does not approve arbitrary new diff.
6. Whole-file readiness applies before diagnostic/negative-control builds too. Use the existing supported CCPU40 production/test profiles, stable caches and owned bounded background runners. No production Computer Use is authorized by this plan; use existing probes/fixtures for investigation.
7. Each substantive delivery follows build, publish to both destinations and verify hashes, test the published package, commit, push. Source review, local experiments, publication and runtime acceptance are separate states. Keep one active S; publish a bounded S brief in Current before each transition.
8. Each S retains a separate review/closure checkpoint. S13 rechecks the complete result independently of per-S closure claims: actual final source diffs, registers, fixture selection, package identity and real guest outcomes. It may reopen a failed owner S; it cannot silently waive it to resume migration.

## N004 restart gate

Before admitting S14, every ledger row must have a truthful final state: restored-and-delivered, minimized-and-delivered with residual registration, or evidenced necessary retain. No unreviewed, unfinished-investigation, unverified local candidate or unresolved exclusion mapping may be relabelled complete. Record exact before/after measurements against both pinned XP and the frozen audit baseline, with excluded-body normalization and diagnostic-line changes separate.

Required final regression covers DOS4GW/NESticle startup, DOS/WOW startup/exit, keyboard/typeahead and repeated presentation switching, Win3.1/Win95 DOS-window transitions, CHAIN2/palette/font restoration, OEM paths and affected callback/resource failure cases. Final Full result must identify the published source/artifact/guest identities. Preserve source/media provenance, immutable guest requirements and every later migration/capability obligation. If an external limitation prevents required proof, keep the gate open and record the limitation; only explicit owner scope change can waive it.
