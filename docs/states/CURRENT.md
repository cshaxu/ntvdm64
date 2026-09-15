# Project Status

## Current Work

**Active: M0 T412 S11.**

Owner directs workspace cleanup and a new S to repair the Console regression
with a correct, compact architecture. S10 reaches an owner-directed bounded
handoff at pushed `157267256`, not full T acceptance. Its
[broker verification](../etc/evidence/m0-t412-s10-final-verification.md) remains
valid within its recorded matrix; EDIT-to-MEM display acceptance failed in
the owner's narrow Windows Terminal session. The
[investigation and handoff](../etc/evidence/m0-t412-s11-console-handoff.md)
preserves reproduction evidence, limitations and retained research inputs.
T412 remains open; no next T is admitted.

S11's first inactive-buffer candidate compiled and passed focused binding
checks but failed the real COMMAND/EDIT/MEM ConPTY sequence. It is withdrawn;
published products are restored unchanged. The linked handoff records the
negative result and retained candidate identity. Resize suppression alone is
not a sufficient root-cause repair. Stream/frame copy and native-child handoff
remain under investigation; S11 is not closed and no owner test is requested.

## Active Packet

### M0 T412 S11 — Console output geometry and lifecycle repair

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T412 S11; Ordinary Mode, implementer followed by final diff review. |
| Admission And Approval | Owner authorizes cleanup and sequential S admission to solve the EDIT-to-MEM display bug through correct, compact architecture; commit and push authorized. |
| Objective | COMMAND -> EDIT -> exit EDIT -> repeated MEM displays correct text, cursor and scrolling in native conhost and narrow ConPTY/Terminal, with correct host restoration. |
| Non-goals | No graphical replacement, guest/CPU algorithm changes, broker redesign, forced stream-mode workaround, worker idle reaper, full WRITE recovery or bulk SoftPC-lib import. |
| Reference Baseline | Pushed 157267256; S10 process-loss products at `O:\winnt`, APP_VERSION 0.0.412 / protocol 3; linked handoff preserves exact hashes and adverse results. |
| Files And ABI Surface | Existing win32 Console adapter and its declarations, neutral session resource storage only as needed, minimal registered mirror bindings in nt_graph.c/nt_fulsc.c, focused observation tests and evidence. No planned wire ABI change. |
| Applicable Rules | Execution/source recovery ladder, architecture/coding/document rules; preserve original owner, call shape, ordering and failure behavior. |
| Verification | Original-source boundary review; automated conhost and ConPTY before/after EDIT matrix, repeated MEM, resize/scroll/cursor/mouse, clipping and API-failure negatives, cleanup and host restoration; fresh x86 /MT CCPU40 build, existing broker regressions, product hashes, governance and Git synchronization. |
| Expected Markers | Actual viewport/buffer geometry and returned paint rectangles agree with the selected mapping; no unexplained blank gaps or clipped success; same mapping for paint/cursor/mouse; no runtime crash. |
| Asset Needs | Intentionally retained manifested S10 investigation sources; new products below build/M0-T412/S11; existing O:\winnt media; observations only O:\winnt\logs. |
| Reporting Requirements | Source-first design decision, before/after reproduction, mirror/overlay/autonomous line accounting, rejected workarounds, exact product identities and remaining owner acceptance. |
| Stop Conditions | Source-proven need for CPU semantics, new component or other material boundary expansion; document and seek revised scope, never mask crashes or count VT replay as real Terminal acceptance. |
| Exit Criteria | Repair and focused/regression tests pass; verified three-program package published, evidence committed/pushed and worktree clean; report for owner Terminal/RDP confirmation. T closure still requires final acceptance. |
| Original Owner Request | “清理干净当前工作区 处理好未提交文档和代码 然后准入一个新的s任务 要求能用正确和精简的合理架构解决这个bug”. |
| Similar-Issue Sweep | Stream/frame transitions, active screen-buffer lifetime, actual versus requested dimensions, scrollback, palette-triggered resize, paint clipping, cursor/mouse coordinates, nested/native child and exit restoration; separately triage recorded short-window C-VID fault. |
| Working Plan | [T412 S plan](../etc/operations/m0-t412-broker-restoration-plan.md). |

## Current Technical Baseline

- Package: run16.exe, basesrv.exe, ntvdm.exe; MSVC x86 /MT, original CCPU40.
  APP_VERSION 0.0.412; RPC protocol 3. This admission changes no products.
- Public entry: run16.exe <binary> [arguments]. Original COMMAND exit/reentry
  semantics and worker lifetime remain unchanged.
- S7 nested execution, S8 original exit semantics and S9 version agreement
  retain their indexed bounded conclusions. S10 process-loss tests do not
  establish narrow-Terminal rendering correctness.
- Startup can start a missing broker; runtime broker/worker loss fails without
  restart/replay. Launcher failure preserves claimed and unrelated workers.
- Owner confirms DOSX/HIMEM/MSCDEX startup loading, not full WRITE execution.
  Full WOW/debugger and WRITE recovery retain their queued owners.
- Build outputs stay below build/; formal publication is `O:\winnt` and
  runtime observations are `O:\winnt\logs`. Previous status chronology is in
  the [snapshot](../etc/evidence/m0-t412-s10-status-snapshot.md).
- [Queue](QUEUE.md) is unchanged. S11 owns the current display regression and
  its explicit adverse-test disposition; it is not silently deferred to release.

## Recent M0 Closures

T411 C-VID/event, T410 native Console selection, T409 DOS file services,
T407 bounded DPMI/XMS and T406 mapping restoration retain their indexed
bounded conclusions. They are not full workload compatibility acceptance.

## Recent Governance

This delivery preserves S10 investigation evidence, removes disposable probe
binaries and objects, and admits S11 without changing the candidate queue.
Earlier governance chronology remains in the indexed status snapshot.
