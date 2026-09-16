# M0 T412 — Broker architecture closure

## Decision and boundary

The owner explicitly accepts closing the Broker task and transferring its
unimplemented S12 CCPU/C-VID work into a separate proposal at the queue head,
then admitting that proposal. This closes T412's Broker architecture scope,
not all worker compatibility. S12 is transferred, not reported as repaired.

Delivered baseline: S10 process-loss `157267256`, S11 Console repair
`3af4aa001`, and former S12 admission `fb6d2d537`. The runtime package is
the S11 three-program build, APP_VERSION 0.0.412 / protocol 3.

## Requirement-to-evidence audit

| Broker requirement | Evidence and conclusion |
| --- | --- |
| Original BaseSrv/BaseClient lifecycle, D01-D07/D10 | [S10 verification](../etc/evidence/m0-t412-s10-final-verification.md) maps command selection, PIF, first-VDM, directories, completion, reentry, capture/retry and classification to selected original bodies; original-owner and classifier fixtures pass. |
| Three programs and original-shaped worker entry | S10 formal x86 links, provider maps and VDM_TIB layout gate; real COMMAND/MEM/EDIT runs. run16 owns classification/creation, basesrv owns coordination, ntvdm owns execution. |
| Versioned authenticated IPC and resources | S10 P3 actual RPC/resource/rundown tests and five negative peers against launcher and worker; copied contracts, stale/forged refusal and wait-only broker-process capability. |
| Commands, nested execution, streams and exits | S11 final 17-case matrix passes, including nested COMMAND/MEM, native streams/EOF and original guest exit conventions. |
| Singleton, residence and empty shutdown | S10 P3 lifetime/drain evidence: concurrent startup, original worker lifetime, empty-broker stop/restart and arrival race. No worker timeout/reaper. |
| Process-loss behavior | S10 P3 broker/worker/launcher loss and controlled startup cut points pass; no runtime replay, rollback before claim, claimed-worker preservation and unrelated-worker isolation. |
| Broker-side WOW coordination | Real worker reaches original shared-WOW PIF acquisition; original service fixtures cover completion. WRITE execution still times out and is explicitly not Broker application acceptance. |
| Source accounting and retirement | S10 whole-T accounting distinguishes imported originals, finite transport and autonomous changes; no claim of zero autonomous code. P3 source total +8533/-2071 across 98 paths is historical S10 accounting, not a count of the later Console repair. |
| Console regression follow-up | [S11 evidence](../etc/evidence/m0-t412-s11-console-handoff.md): original fixed-cell resize facade, four ConPTY sizes, native contract and mouse tests pass. S11 adds 109 production lines in the adapter; zero mirror/overlay edits. |

The closure audit rechecked the retained P3 lifetime report and final S11
17-case summary, publication hashes and clean synchronized Git state.
`git diff 157267256..fb6d2d537` has no changes in app, broker, session,
opennt-host or the BaseSrv adapter. Broker fault evidence is therefore retained
same-source evidence, not mislabelled as a fresh same-EXE fault run on S11.
Exact S11 product hashes and final-build observations remain in its evidence.

## Explicit receivers and limitations

- The prefilled 80x5 C-VID crash, complete accessor/VideoVector/GDP audit,
  source restoration and adjacent live-resize regression transfer to the
  [new accessor proposal](../proposals/proposal-ccpu40-cvid-accessor-layout-restoration-001.md).
  This is an unresolved defect, not an accepted passing test or a Broker fix.
- Former S12's unverified patch remains at
  `build/M0-T412/S12/admission/accessor-research.patch`, SHA-256
  79ACAD335642107F72FC8D8B4605C60B492DB18A9452C8095565A67D57A7A78F.
  Its three candidate files are absent from the S11 product diff. No work is lost.
- Owner Terminal/RDP visual confirmation remains outstanding; automated
  Console/ConPTY evidence is not a claim of that manual acceptance.
- Full WOW/debugger and WRITE workload restoration retain their separate
  queued owners. The x87 layout candidate remains distinct from video accessors.

The [Broker proposal](../proposals/proposal-cross-process-broker-closure-001.md)
and [working plan](../etc/operations/m0-t412-broker-restoration-plan.md) are
retained historical requirements/evidence pointers, not an active T packet.
