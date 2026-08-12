# Project Status

## Current Work

**Active: M0 T95 S7 — contained read-only guest-file capability.**

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | New S within the existing `M0 T95` package; Ordinary Mode. |
| Admission And Approval | Owner approved the contained CLI-first NTVDM route, immutable BYOB identity admission, no ambient host exposure, and a holistic capability rather than per-BOP hacks. |
| Objective | Materialize one coherent, read-only guest-file capability for the first contained profile: explicit verified `COMMAND.COM` placement, CONFIG.SYS/AUTOEXEC.BAT materialization, and canonical DEM Open/Seek/Read/Close semantics. |
| Non-goals | Ambient PIF/SystemRoot/temp/registry behavior; BYOB-root-as-DOS-root; host mutation; arbitrary host reopen; general DOS filesystem; search/redirector/WOW; and Bochs CPU, device, FPU, PIC, or VGA semantics. |
| Reference Baseline | `design/ADAPTER-BULK-RESULT.md`, `design/HOST-NAMESPACE-CAPABILITY.md`, and the retained [M0 T95 status ledger](etc/evidence/m0-t95-status-ledger-20260811.md). |
| Files And ABI Surface | Versioned BYOB profile/selection extension; adapter-local immutable placement and read-token state; typed CPU/bulk result records; focused CLI/adapter tests. Bochs receives no path, handle, selector, or configuration term. |
| Applicable Rules | `rules/EXECUTION.md`, `rules/ARCHITECTURE.md`, `rules/CODING.md`, `rules/DOCUMENT.md`, and `etc/operations/task-identifier-governance.md`. |
| Verification | Run profile admission, namespace, adapter runtime, read-token lifecycle, CONFIG/AUTOEXEC materialization, boundary, documentation-governance, and diff checks named by the current S7 evidence. |
| Expected Markers | Placement is rejected before guest execution when invalid; immutable `\COMMAND.COM` reachability; bounded source-shaped service outcomes; and no DOS/file vocabulary across the Bochs seam. |
| Asset Needs | Existing admitted NTIO/NTDOS/COMMAND profile inputs and retained OpenNT source only. |
| Reporting Requirements | Record requirement-to-evidence mapping, changed files, verification results, outside-scope disposition, and any material scope/evidence/risk objection before a P delivery. |
| Stop Conditions | Stop if this needs ambient NT state, a Bochs semantic/device change, unbounded guest-string reading, host mutation, raw pointer/handle crossing, or a special COMMAND/configuration fast path. |
| Exit Criteria | Explicit reproducible COMMAND placement; paired CONFIG/AUTOEXEC selection; bounded Open/Seek/Read/Close contract and tests; adapter-owned host-resource lifetime; no Bochs/guest semantic leakage; and a fresh trace stating exactly what executed. |
| Original Owner Request | Continue a non-invasive CLI NTVDM with Bochs as the minimal SoftPC replacement, an adapter as the sole boundary, explicit BOP observation, source-shaped rehosting, and no unprincipled growth. |
| Similar-Issue Sweep | Review profile admission, runner handoff, namespace roots, bulk-result lifecycle, COMMAND/configuration callers, DEM error/retry behavior, BOP listener/dispatch separation, and the Bochs intrusion registry together. |

## Current Technical Baseline

- `M0 T95` is open; `S7` is the sole active subtask. `S1`--`S6` and their
  `r` observations are retained evidence, not concurrent packets.
- Bochs 2.6 owns machine mechanics only; OpenNT owns guest DOS/WOW/BOP/DEM/
  COMMAND semantics; the typed adapter bridge is the only connection.
- The host namespace is immutable, profile-selected, and adapter-owned. No
  arbitrary host path, handle, or guest-triggered enumeration is admitted.
- Detailed historical status, completed increments, and run observations are
  preserved in [the retained ledger](etc/evidence/m0-t95-status-ledger-20260811.md).

## Recent Progress

| Scope | Compact result |
| --- | --- |
| T95 S1--S6 | Historical Bochs/adapter evidence and run records retained in the status ledger; they do not allocate P values retroactively. |
| T95 S7 | Profile-owned COMMAND/CONFIG/AUTOEXEC placement and bounded adapter read capability are evidenced; real runtime trace closure remains in the active S packet. |
| M0 Td S1 P1--P3 | Documentation governance now uses one active S in Status, a T-only Queue, indexed supporting evidence, and a hash-verified full-document inventory. |
