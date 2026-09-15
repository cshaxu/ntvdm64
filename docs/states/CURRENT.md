# Project Status

## Current Work

**Active: M0 T412 S10.**

S7 is [closed by owner direction](../etc/evidence/m0-t412-s7-interactive-child-repair.md#s7-owner-directed-closure-and-s8-handoff)
at pushed `8335f38bd`: nested COMMAND/MEM and EDIT return are delivered.
S8 is [closed by owner acceptance](../etc/evidence/m0-t412-s8-command-exit-provenance.md#owner-accepted-s8-closure)
at delivered `67ee292cb`: preserve original COMMAND exit semantics unchanged.
All 15 Console cases pass; direct DOS/native result 7 propagates correctly.
S9 [implementation and verification](../etc/evidence/m0-t412-s9-version-agreement.md)
deliver shared APP_VERSION 0.0.412 and protocol 2, mutual rejection and
incompatible-worker rollback at pushed df2467de0. Owner now authorizes the
final closure packet and confirms DOSX/HIMEM/MSCDEX load at startup.
This advances the delivered S9; it is not full device or WRITE acceptance.

The [S6 VDM TIB repair](../etc/evidence/m0-t412-s6-vdm-tib-storage-repair.md)
retains its full-storage link gate. Current products are at `O:\winnt`;
full T412 matrix and owner acceptance remain open.

S10 P1 delivers the [final verification and accounting](../etc/evidence/m0-t412-s10-final-verification.md):
17 Console rows, original-owner/RPC/version tests and controlled empty-drain
race pass. The separately controlled admitted-broker-loss test fails with a
20-second timeout after typed EXIT. S10/T412 therefore remain open; no closure
or next T admission is claimed. Deliver the verified changes and wait for owner
direction on the recorded remaining fault contract.

## Active Packet

### M0 T412 S10 — Final broker package verification and closure

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T412 S10; Ordinary Mode, one implementer then reviewer. |
| Admission And Approval | Owner admits final T closure work, build/test/commit/push then wait. S9 delivery is the predecessor. |
| Objective | Verify final same-source three-program composition and settle D01-D07/D10, code accounting and explicit residual boundaries. |
| Non-goals | No new T, full WRITE recovery, guest changes, worker reaper or invented service policy. |
| Reference Baseline | df2467de0 delivery; whole-T comparison starts at S1 input 08b33351b1c341bb433f27ab517ee9a4e0bc3e0c. |
| Files And ABI Surface | Final verification tools/tests, scoped source registers and governance/evidence; existing protocol 2 and APP_VERSION 0.0.412 unchanged. |
| Applicable Rules | All current execution, source, architecture, coding and document authorities; source-first minimal diff. |
| Verification | Fresh x86 /MT build; original-owner fixtures, actual RPC/security/resources, Console matrix, WOW coordination and broker lifecycle/races; hashes, governance and Git synchronization. |
| Expected Markers | Every mandatory broker contract passes on identified final inputs; separate historical proof, current proof and out-of-scope workload limits. |
| Asset Needs | build/M0-T412/S10/final; existing MSVC/Node and O:\winnt media; all runtime logs in O:\winnt\logs. |
| Reporting Requirements | Measured mirror/overlay/project deltas, retained seam rationale, exact products and tests; stop after delivery. |
| Stop Conditions | Unresolved mandatory contract, runtime regression or material owner-boundary change; record failure, never declare unsupported proof passed. |
| Exit Criteria | Final scoped matrix and accounting complete, verified products published, closure evidence committed/pushed, worktree clean; await owner. |
| Original Owner Request | “请你准入一个s任务来执行收口工作 完成了以后 编译测试提交推送等我”. |
| Similar-Issue Sweep | Final artifact lineage, obsolete local providers/entries, startup/drain/disconnect, version rejection and original DOS/WOW separation. |
| Working Plan | [T412 S plan](../etc/operations/m0-t412-broker-restoration-plan.md). |

## Closure work

S1-S8 reached their recorded bounded conclusions; S9 delivery is advanced by
owner authorization of S10. The indexed [working plan](../etc/operations/m0-t412-broker-restoration-plan.md)
and [supporting records](../etc/README.md) retain the complete requirement chain.
Earlier status chronology is preserved in the [snapshot](../etc/evidence/m0-t412-s10-status-snapshot.md),
not repeated as live admission state.

S10 found and repaired a dormant Check scalar encoder's uninitialized reserved
fields, obsolete fixture capacities/media paths, and UNKNOWN_IF misclassification
when an empty broker drains. The existing pending-worker query is relocated
unchanged into the mirror's private overlay and explicitly registered.
Final same-source positive verification and accounting are recorded; admitted
broker loss remains a blocking negative. No T closure is claimed.

## Current Technical Baseline

- Product package: run16.exe, basesrv.exe and ntvdm.exe; MSVC x86 /MT,
  original CCPU40. APP_VERSION 0.0.412 and RPC protocol 2.
- Public entry: run16.exe <binary> [arguments]. Original COMMAND exit/reentry
  semantics are retained; no worker idle timer or reaper.
- Formal outputs and fixtures stay under build/. Published products are at
  `O:\winnt`; observations belong only below `O:\winnt\logs`.
- Owner confirms DOSX, HIMEM and MSCDEX load at startup. This is not complete
  device validation, full DPMI compatibility or successful WRITE execution.
- Full WRITE/WOW provider recovery remains with the existing queued owners.
  Broker-side WOW acquisition/completion tests do not prove GUI execution.
- [Queue](QUEUE.md) owns the unchanged next-candidate order; [TODO](TODO.md)
  owns unplanned debt. No next T is admitted by this closure packet.

## Recent M0 Closures

| Task | Compact result |
| --- | --- |
| T411 | CCPU40 C-VID/event bounded closure; its RESET evidence correction was completed in T412 S1. |
| T410 | Retired project-owned presentation-window path; native Console remains selected. |
| T409 | Original DOS file-service ownership and guest fast-I/O fallback restored. |
| T407 | Original DPMI/XMS source package and bounded DOSX transition restored; full WRITE remains outside that closure. |
| T406 | Original mapping contracts and bounded EMS lease restored; x86-width cohorts cleaned. |

## Recent Governance

- **M0 Td S10 P1:** 8f7f6ec3d assigned U03-U08 to named proposal owners.
- Earlier governance deliveries and closure details remain in the indexed
  [status snapshot](../etc/evidence/m0-t412-s10-status-snapshot.md).
