# Project Status

## Current Work

**Active: M0 T413 S6** — Terminal resize and parent-CMD exit handoff.

The owner explicitly corrects the requested repair to Windows Terminal:
run16 COMMAND, maximize while running, EXIT, parent CMD DIR fills the screen,
then typed input jumps about three rows above the visible prompt. S5 remains
closed; this owner-approved extension does not reopen exception repair.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T413 S6, Ordinary Mode. |
| Candidate Proposal | [Accessor and Console regression context](../proposals/proposal-ccpu40-cvid-accessor-layout-restoration-001.md). |
| Admission And Approval | Owner requests a new S to fix Terminal, then compile/test/commit/push; clarification overrides the mistakenly selected exception quotation. |
| Objective | Preserve coherent Console geometry, modes and cursor ownership when a resized Terminal returns from VDM to native CMD and begins cooked input after scrolling. |
| Non-goals | No CCPU, guest cursor arithmetic, exception repair, broker protocol or Terminal settings changes. No fixed three-row cursor compensation. |
| Reference Baseline | 58c535e2e, S5 published three-program package, existing Console grid-resize and handoff code. |
| Files And ABI Surface | Original nt_hosts/nt_event/nt_fulsc/nt_graph consumers; existing Console adapter; only source-proven affected boundary changes, related regression observers and evidence. |
| Applicable Rules | Execution, architecture, coding, document and source policies; source-first finite Console boundary. |
| Verification | Reproduce native CMD before/after child VDM and live ConPTY resize; observe buffer/window/cursor/modes and partial cooked input; contrast old/new exact builds; retain existing COMMAND/EDIT/MEM, resize and mouse regression checks; governance/link and git diff checks. |
| Expected Markers | Parent CMD prompt and typed characters stay on one logical line after scroll; current Terminal dimensions remain coherent; original failure reproduced and corrected without guest changes. |
| Asset Needs | Existing OpenNT mirrors and selected sources; MSVC x86 /MT; build/M0-T413/S6/{formal,integration}; runtime media O:\winnt and observations only O:\winnt\logs. |
| Reporting Requirements | Exact root cause, changed owner, source rationale, negative control, test limits, published hashes and clean pushed revision. |
| Stop Conditions | Conflicting concurrent edits, inability to reproduce or a required new platform boundary; report rather than invent a repair. Never terminate an unrelated live session. |
| Exit Criteria | Demonstrated minimal repair, three formal EXEs, focused and integration tests, tracked evidence, commit/push and clean worktree. |
| Original Owner Request | “我就是要你处理terminal的问题啊”; new S, compile/test/commit/push as requested immediately beforehand. |
| Similar-Issue Sweep | No resize, grow/maximize, shrink/restore, scroll then partial cooked input; ordinary conhost versus ConPTY and child exit versus app transition. |

## S6 Investigation Progress

[Initial evidence](../etc/evidence/m0-t413-s6-terminal-handoff-investigation.md):
four system-ConPTY parent-CMD handoff runs do not reproduce the owner's visible
row mismatch. Native buffer/window/cursor records remain consistent. Actual
Windows Terminal frontend/runtime observation is still required; screenshots
requested. S6 pauses at its reproduction gate, not closed. No product source,
build or published EXE change; admission/test evidence only in this delivery.

## S1 Closure Record

[S1 audit](../etc/evidence/m0-t413-s1-accessor-layout-audit.md), 23a4fa83c.

## S2 Closure Record

[S2 restoration](../etc/evidence/m0-t413-s2-accessor-restoration.md), 3cc7851e5.

## S3 Closure Record

[S3 integration](../etc/evidence/m0-t413-s3-integrated-verification.md), d9d814b00.

## S4 Closure Record

[S4 cleanup](../etc/evidence/m0-t413-s4-binding-cleanup.md), eb0c4fa33.

## S5 Closure Record

[S5 exception repair](../etc/evidence/m0-t413-s5-exception-scope.md), 58c535e2e.

M0 T413 S5 concludes the owner-approved five
NTVDMx64 exception-hook scope corrections. The
[closure audit](../history/m0-t413-accessor-restoration-closure.md#s5-exception-scope-closure)
and [S5 evidence](../etc/evidence/m0-t413-s5-exception-scope.md) retain scope,
provenance, tests and explicit compatibility limits. No next T is admitted.

## Current Technical Baseline

- O:\winnt contains fresh formal x86 /MT run16.exe, basesrv.exe and ntvdm.exe,
  original CCPU40, APP_VERSION 0.0.413 and protocol 3. Hashes match S5 evidence.
- Original vglob.c supplies all 38 getter/setter pairs through original typed
  declarations. The registered seven-line header seam retains original full
  vector layout; autonomous field rebinding and 20 conversion wrappers remain
  removed. Original complete USE16 IP arithmetic remains restored.
- Five c_xcptn.c scope corrections keep immediate continuation/flag cleanup
  inside the handled protected-mode hook branch. Unhandled and real-mode faults
  reach original delivery. No new runtime function, adapter or overlay.
- 27 actual-source exceptional-control cases pass; uncorrected source fails
  101 assertions. All 168 IP-expression cases and 12 real DOS instruction cases
  pass, including divide-by-zero guest-handler entry and saved fault-IP check.
- Full original C-VID owner-library fixture, 17 command cases, six ConPTY
  profiles, native resize contracts and five prefilled short-window EDIT runs
  pass. The latter include 20 post-EDIT MEM completions.
- Protected-mode tests prove repaired branch behavior and delivery arguments,
  not complete IDT/TSS or WOW/WRITE runtime compatibility. No such claim is made.
- Mirror diff distance is 437 versus pre-T413 506; overlay cohort stays 125
  lines. S5 adds only ten braces and five attribution comments across existing
  condition lines (+15/-5); no executable statement is added.
- Build roots: build/M0-T413/S5/formal and integration. Prior products remain
  recoverable under integration/prior-products; runtime logs only O:\winnt\logs.
  Owner Terminal/RDP visual acceptance remains separate.

## Delivery References

| Delivery | Evidence |
| --- | --- |
| S1, 23a4fa83c | [Source audit](../etc/evidence/m0-t413-s1-accessor-layout-audit.md) |
| S2, 3cc7851e5 | [Accessor restoration](../etc/evidence/m0-t413-s2-accessor-restoration.md) |
| S3, d9d814b00 and fbb121d72 | [Integrated verification](../etc/evidence/m0-t413-s3-integrated-verification.md) |
| S4, eb0c4fa33 | [Binding cleanup and IP repair](../etc/evidence/m0-t413-s4-binding-cleanup.md) |
| S5, admission 03847f34b; P2 accompanies this record | [Exception scope repair](../etc/evidence/m0-t413-s5-exception-scope.md) |

## Next Work

[Queue](QUEUE.md) remains the sole ordering authority. Existing x87 host-layout
restoration is first. The owner-requested
[remaining NTVDMx64 corrections proposal](../proposals/proposal-ntvdmx64-source-corrections-001.md)
is second, before WOW/debugger. It requires hunk-level applicability proof,
accounts for existing equivalent fixes such as DIV-221, and rejects blanket
optimization/hack imports. Neither candidate is admitted by S5 closure.

## Recent M0 Closures

T413 S5 supersedes the previous real-mode divide timeout disposition, without
reopening T412 Broker or claiming full WRITE recovery. T411/T410/T409/T407/T406
retain their recorded scopes and limitations.

## Recent Governance

S5 P2 delivers the source repair, upstream hunk evidence and separate queued
proposal in one reviewed commit. The current packet is closed without admitting
the next candidate; no document inventory or parallel status is introduced.
