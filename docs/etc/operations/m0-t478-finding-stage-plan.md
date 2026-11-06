# T478: prior-finding S work plan

This plan maps the [69 findings](../evidence/m0-t478-s1-classified-findings.md)
to sequential proposed S packets. The active packet in CURRENT.md alone controls admission; these
numbers are a plan, not simultaneous admission to implementation. The owner requests one S
per unresolved problem, ordered by: original code replacing duplicate local
implementation; missing original integration without an equivalent; then
known bugs, risks and incomplete fixes. S1's broader inventory reconciliation
is still in progress: this mapping is not the complete T478 inventory. Add
further evidenced findings without silently dropping or conflating them.

Scope is mvdm-host and necessary host connections. Guest source, media, tools
and packaging are not work items; existing immutable guest inputs may provide
read-only evidence for a concrete host capability. A candidate is not a proven
defect or an instruction to restore an incompatible `#error` branch. Each S
first establishes current host ownership, reachability, original source and
product requirement, then implements only a demonstrated appropriate repair;
otherwise record the evidence and disposition without inventing a change.
Previously accepted exclusions and limitations remain in force. Host-adjacent
issues #15 and #30 have an explicit applicability gate, not permission to
change guest or unrelated components.

No item in the 69-row classified report currently proves both an unconnected
original owner and a replaceable duplicate autonomous implementation. Thus
the first priority has no falsely asserted item. If S1 or a later S establishes
one, insert it before the missing-without-equivalent group, renumber the
unadmitted future packets, restore the original owner where appropriate, and
remove the displaced local implementation in the same delivery. Generic thunk
is a missing-integration candidate, not a proven duplicate replacement.

## Priority 2: missing original host integration

| Planned S | Finding | Objective and disposition gate |
| --- | --- | --- |
| S2 | 33 — WOW generic thunk | Establish caller/ABI and original assembly helper composition; restore a narrowly selected host binding where compatible, with calling-convention, negative and lifecycle tests. Do not globally enable i386 branches. |
| S3 | 20 — ForceDOS host flag | Trace actual host entry and CREATE_FORCEDOS propagation; connect the original host path only if a supported caller requires it. Independent guest utilities are excluded. |
| S4 | 26 — remote RAP provider | Trace selected host calls, supported feature and private transport boundary; connect a legitimate provider if viable and needed, otherwise document the precise unavailable contract. |
| S5 | 09 — directory reset | Verify the existing restart/fallback; finish only demonstrated remaining original-semantics gaps if substantially implemented, otherwise prefer explicit failure and remove unnecessary code. Unavailable modern-Windows kernel indexed support may remain a registered accepted gap; no added capability is required. |

## Priority 3: known problems and partially completed findings

Each row owns only its outstanding host behavior. Recheck the existing fix and
its test before making changes; no real-world failure is asserted merely from
missing coverage. The finding IDs link to the classification and its per-row
source evidence.

| Planned S | Finding | Objective |
| --- | --- | --- |
| S6 | 04 — SAS vector | Reconcile prior successful tests and current selected consumers of unsupported SAS tail slots; repair only a demonstrated remaining original-semantics host defect. Unused slots do not justify new capability or speculative ABI cleanup. |
| S7 | 10 — XMS block boundaries | Reproduce zero-size/equal-size paths and correct any reachable uninitialized state. |
| S8 | 11 — suballocator rollback | Verify callback failure bitmap consistency and repair confirmed rollback defects. |
| S9 | 13 — floppy/FDC ownership | Verify cross-thread state and actual host device lifetime; repair demonstrated ownership defects. |
| S10 | 15 — keyboard layout | Determine whether a modifiable host boundary actually exposes the length risk; guest COMMAND changes are excluded. |
| S11 | 22 — XMS dispatch | Check release dispatch bounds and supported invalid inputs; repair confirmed host indexing risk. |
| S12 | 24 — redirector loader | Exercise loading failure/cleanup and correct module-handle ownership if confirmed. |
| S13 | 28 — input scan tables | Verify exact bounds and repair the proven one-past access without changing mapping semantics. |
| S14 | 29 — ROM/resource load | Verify non-ROM oversize failure and close the leaked descriptor where reachable. |
| S15 | 30 — codepage transition | Trace host responsibility for the observed codepage mismatch; change only an identified necessary mvdm-host connection, otherwise record the separate component owner. |
| S16 | 34 — WOW file-cache flush | Exercise tail flush, handle reuse and teardown; repair the confirmed skipped node. |
| S17 | 42 — WOW virtual memory | Verify allocator flags and failure rollback for selected callers; fix demonstrated defects. |
| S18 | 43 — parameter-map lifecycle | Establish cross-task borrowing and retirement semantics; fix only a reachable lifetime violation. |
| S19 | 47 — EDIT tab-stop return | Bind previous fallthrough evidence to current product and repair confirmed unthunk behavior. |
| S20 | 49 — DIB registration failure | Test selector rollback and bitmap ownership on failure; repair confirmed cleanup defects. |
| S21 | 50 — DIB retirement | Test selector callback failure/reentrancy and safe mapping retirement; repair confirmed defects. |
| S22 | 51 — font query output | Verify short/error results and kerning bounds; repair confirmed host/guest-buffer handling. |
| S23 | 55 — palette conversion | Reproduce short-return conversion and limit copies to actual valid output. |
| S24 | 56 — embedding/profile enumeration | Verify provider error termination and bounded host output; repair a demonstrated loop or boundary issue. |
| S25 | 57 — fax failure | Establish selected host provider and allocation failure behavior; repair confirmed null/cleanup paths. |
| S26 | 58 — printer/spool failure | Test failed ClosePrinter and object ownership; repair confirmed lifetime errors without reopening accepted raw-provider limitations. |
| S27 | 59 — DLC event creation | Exercise second-event failure and repair the incorrect host event check if selected. |
| S28 | 62 — redirector asynchronous completion | Exercise allocation failure and teardown/drain; repair a demonstrated host interrupt or lifetime defect. |
| S29 | 65 — Winsock asynchronous database requests | Test task-exit cancellation failure and buffer lifetime; fix a demonstrated race while preserving T467 fixes. |
| S30 | 01 — CPU diagnostics | Check uncovered host nested/exception diagnostic paths and repair demonstrated gaps; preserve delivered diagnostics. |
| S31 | 07 — DLL entry lifecycle | Verify host explicit unload and failure cleanup, using guest execution only as evidence. |
| S32 | 08 — export/profile consumers | Check remaining supported host consumers and host task lifecycle; repair evidenced gaps. |
| S33 | 12 — DPMI buffer/save state | Reconcile remaining original host buffer/save candidates with T430 lease fix and repair confirmed defects. |
| S34 | 21 — BIOS integrations | Verify host EMS dispatch and device IRQ lifecycle; repair host gaps established by tests. |
| S35 | 27 — BIOS RTC | Verify host RTC BOP 70 and IRQ/EOI handling; repair demonstrated gaps. |
| S36 | 35 — WOW shell/hooks | Verify host callback nesting and task exit; repair demonstrated lifecycle defects. |
| S37 | 40 — DDEML/WINMM | Establish supported host-side async callback and DDE ownership; guest modules themselves remain outside scope. |
| S38 | 48 — clipboard/caret | Verify remaining supported host format and caret paths beyond T460; repair demonstrated gaps. |
| S39 | 52 — keyboard thunks | Verify host input, DBCS, failure and exit against original ownership; repair demonstrated defects. |
| S40 | 54 — multimedia/fax | Verify supported host wave/MIDI/MCI/fax callbacks and teardown beyond T466; do not revive excluded dead conversion code by assumption. |
| S41 | 61 — VDD/optical host chain | Verify RegisterVDD loader and host optical calls only; guest VCDEX placement, installation and packaging are excluded. |

Partial rows #32, #38, #44 and #46 do not create additional S packets: #32
has no identified host defect after the 70 checks; #38 and #44 concern guest
image identity with no established host gap; #46's remaining raw-provider
limitation was accepted and its separate host failure issue is #58/S26. If
new evidence identifies a distinct mvdm-host problem in any of these, create
its own S rather than silently reopening guest work or an accepted limit.
All other resolved, explicitly excluded and guest-only rows remain in the
source ledger, not in the implementation queue.

## Execution and evidence

Sequential S admission occurs only after the preceding active S reaches its
bounded conclusion and the owner admits the next. For every production repair,
record original/current source identities, ownership, effective build/link and
dispatch, focused normal/failure/teardown tests and applicable full regression;
build, local release to both configured destinations, hash verification and
delivery follow the active packet and owner instructions. An evidence-only
disposition must document why no production change is justified. Preserve
existing regression and immutable guest boundaries. S1's complete inventory
and inherited handoffs remain open, so this plan cannot by itself close T478.

## S3 disposition

Finding 20's missing host flag producer and Run16 dispatch are resolved by
[S3](../evidence/m0-t478-s3-forcedos.md). The existing original broker/COMMAND
implementation is reused. Independent utilities, guest modifications and
unproved PIF issues remain outside this bounded repair. S4 is the next proposed
packet; this record does not admit it. Publication follows the owner's current
worktree-assets-only, local-commit, no-push instruction.

## S4 disposition

[S4](../evidence/m0-t478-s4-remote-rap.md) corrects finding 26's overbroad
unavailable classification: original server enumeration has a reachable public
NetServerEnum branch, now restored with original conversion. Actual private
RAP transport, password/session-key and private Browser boundaries retain
explicit unavailable contracts. This per-boundary disposition closes S4;
S5/finding 09 remains proposed, not admitted.

## Owner scope clarification

The owner explicitly rejects adding capability beyond original OpenNT.
For S5, an unavailable NT4 kernel indexed directory path may remain a clearly
registered gap; it is not a mandatory recovery target. Substantially completed
existing restart support may be finished, but a speculative or fragmentary
replacement should instead fail explicitly and lose unnecessary scaffolding.
Do not expand deletion/rename tolerance or performance guarantees to create
new closure obligations. Preserve verified useful existing behavior.

## S5 disposition

[S5](../evidence/m0-t478-s5-directory-restart.md) verifies the already implemented
bounded restart and its explicit failure cases. No additional production repair
is justified. The missing indexed kernel path is an owner-accepted registered
gap, not a mandatory implementation item. S6/finding 04 remains proposed.

## S6 disposition

[S6](../evidence/m0-t478-s6-sas-vector.md) closes finding 04 as no demonstrated
applicable repair under the original-capability ceiling. Current source, archive,
direct-object and PE checks support preserving the coherent layout and unsupported
tails. No product implementation changes; no universal non-reachability claim.
Reopen only with a concrete supported caller/original contract or failure evidence.
S7/finding 10 is next proposed, not admitted.
