# Project Status

## Current Work

**Active: M0 T412 S12.**

M0 T412 S11 P3 reached owner-directed bounded closure at pushed `3af4aa001`
for the EDIT-to-MEM
blank-gap repair. The [S11 evidence](../etc/evidence/m0-t412-s11-console-handoff.md)
records source recovery, exact products, positive tests and the known 80x5
C-VID crash. The owner explicitly transfers that crash and complete original
accessor/layout restoration to S12; it is not marked passing. T412 remains
open and the next T is not admitted. S12 is now the sole admitted packet.
No accessor candidate has been accepted or published; the retained patch is
research material to review against the original source.

## Active Packet

### M0 T412 S12 — Original CCPU/C-VID accessor and layout restoration

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T412 S12; Ordinary Mode, implementation followed by final diff review. |
| Admission And Approval | Owner explicitly closes S11 and admits S12 to investigate the CCPU-related fault, restore original OpenNT source, reduce unnecessary diffs and test the actual repair. Commit and push remain authorized. |
| Objective | Resolve the recorded prefilled 80x5 C-VID crash through source-proven original accessor/layout composition, minimize redundant mirror/overlay/autonomous code and preserve the S11 Console fix. |
| Non-goals | No assumption that CCPU instructions are wrong; no opcode, RMW/string/stack/fetch algorithm changes, alternate CPU, broker redesign, full WOW/WRITE restoration or host mutation. |
| Reference Baseline | Pushed S11 P3 `3af4aa001`; three x86 products at `O:\winnt`, APP_VERSION 0.0.412 / protocol 3. The linked S11 evidence records their hashes, passing matrix, failing 80x5 case and preserved WIP. |
| Files And ABI Surface | Original C vglob.c and generated C-VID/CCPU headers/tables; existing vector-binding overlay; build composition and complete profile fixtures. Consumer bindings only when source evidence proves necessary. No planned public/wire ABI change. |
| Applicable Rules | Execution recovery ladder and complete CPU compatibility-profile gate; architecture/coding/document rules and source-policy; preserve original owners and ordering, never copy a convenient provider blindly. |
| Verification | Audit all consumers and link-map providers; prove all 38 getter/setter pairs, latch and pointer selectors, GDP field offsets, signed/pointer ABI and scratch/ring preservation. Then fresh x86 /MT build, repeated prefilled 80x5, four-size ConPTY, live resize, mouse, COMMAND/EDIT/MEM and 17 command regressions; product hashes, governance, final diff review and Git synchronization. |
| Expected Markers | Original C accessor bodies are actually selected with the correct GDP layout; no mismatched short/full VideoVector consumers or scratch corruption; repeated adverse runs no longer crash; prior blank-gap and command tests remain passing. |
| Asset Needs | Existing original source and retained S11 logs; build/M0-T412/S12/admission/accessor-research.patch is hashed unverified research only. New build/fixture products under build/M0-T412/S12; runtime observations only `O:\winnt\logs`. |
| Reporting Requirements | Root cause and confidence, original owner/source, four-rung recovery rationale, every retained adapter/overlay justification, measured mirror/overlay/autonomous line reduction or increase, exact tests and product hashes, unresolved limits. |
| Stop Conditions | Source-proven need to change CPU instruction semantics, new architecture or unavailable historical dependency outside the admitted accessor/profile surface. Preserve evidence and obtain revised scope; never mask a crash or repair one trace leaf. |
| Exit Criteria | Complete source-defined accessor/layout profile passes focused tests; the known crash is resolved with repeated integration evidence and Console/command regressions pass; verified package published, evidence committed/pushed and worktree clean. T412 final owner acceptance remains separate. |
| Original Owner Request | “把现在的 S 任务收口，然后我们再准入一个新的 S12的任务来专门调查 CCPU 的问题…恢复原始 OpenNT 的源代码来减少更多的 diff，并且测试看看是否能够真正修复。” |
| Similar-Issue Sweep | Every full/five-slot vector consumer, GDP layout, provider selection, all external accessors, latch/pointer exceptions, initialization/reentry/thread ownership and teardown; adjacent narrow/short/live-resize Console paths. |
| Compatibility Profile | CCPU40 with C-VID external host accessors. Scalar/RMW/string/stack/fetch algorithm groups are inapplicable because this profile changes dispatch/layout only, not instruction execution; document source evidence before broadening it. |
| Working Plan | [T412 plan, S12 restoration section](../etc/operations/m0-t412-broker-restoration-plan.md). |

## Current Technical Baseline

- Formal package: run16.exe, basesrv.exe and ntvdm.exe, MSVC x86 /MT,
  original CCPU40, APP_VERSION 0.0.412 and protocol 3.
- Published at `O:\winnt` from build/M0-T412/S11/cell-grid with the exact
  SHA-256 values in the linked evidence. No experimental accessor changes
  or temporary mirror diagnostics are included.
- Four static ConPTY sizes, isolated native resize/error contracts, mouse
  delivery and 17 command cases pass. Post-publication 45x34 repeats the
  successful EDIT/MEM sequence. Actual owner Terminal/RDP visual acceptance
  remains separate from automated coverage.
- Failed prefilled 80x5 C-VID test transfers to S12, together with complete
  VideoVector/GDP layout and original C accessor composition. Live external
  resize coverage is not claimed; it remains an adjacent S12 regression check.
- Unverified accessor WIP is preserved as a hashed research patch under
  build/M0-T412/S12/admission, not in the released source or a stash.
- All runtime observations stay in `O:\winnt\logs`. Build artifacts remain
  under build/. [Queue](QUEUE.md) is unchanged.
- Original COMMAND statuses and worker lifetime are unchanged. Full
  WOW/debugger and WRITE acceptance retain their queued owner boundaries.

## Recent M0 Closures

S11 closes only the bounded blank-gap repair by owner direction. Its C-VID
failure remains within T412/S12. S10 process-loss evidence retains its bounded
conclusion. T411 C-VID/event, T410 Console, T409 DOS services, T407 DPMI/XMS
and T406 mapping do not constitute full workload acceptance.

## Recent Governance

The owner supersedes the earlier combined S11 scope with pushed S11 closure
and sole S12 admission. The [T412 plan](../etc/operations/m0-t412-broker-restoration-plan.md)
and S11 evidence preserve the transfer, limitations and exact retained WIP.
