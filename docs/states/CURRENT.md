# Project Status

## Current Work

**Active: M0 T404 S3**

## Active Packet

### M0 T404 S3 P2 — presentation-window mouse input recovery

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T404 S3 P2, Ordinary Mode. P1 (`510d74b76`) delivered the bounded mouse binding; P2 verifies the text-mode display-transfer entry. |
| Admission And Approval | Owner approved admission and direct, bounded reuse of the independently implemented mouse support. |
| Candidate Proposal | [Presentation-window mouse input recovery](../proposals/proposal-presentation-window-mouse-input-recovery-001.md) |
| Reference Baseline | The selected x86 `ntvdm32.exe` and its unchanged original `nt_event.c` → `nt_mouse.c` → `base/keymouse/mouse_io.c` route. |
| Applicable Rules | [Execution](../rules/EXECUTION.md), [architecture](../rules/ARCHITECTURE.md), [coding](../rules/CODING.md), [document](../rules/DOCUMENT.md), and the [source policy](../etc/operations/policy/source-policy.md). |
| Objective | Recover text-mode DOS mouse input through the existing Console-to-SoftPC-to-INT 33h path. |
| Non-goals | Guest mouse driver/media, mouse BOP, SoftPC/OpenNT mouse algorithm changes, graphics input, generic input broker, or cross-process broker work. |
| Files And ABI Surface | `src/app/presentation_window.[ch]`, its focused fixture/build selection, and indexed evidence only. The sole new behavior is Win32 mouse messages becoming bounded `MOUSE_EVENT_RECORD`s on the existing `CONIN$` handle. |
| Verification | Source-path audit; focused x86 build/link; pixel/grid and button/capture tests; owner `EDIT.COM` presentation-window run; governance and diff checks. |
| Expected Markers | Bounded `MOUSE_EVENT_RECORD`s on `CONIN$`, unchanged original mouse sources, focused fixture exit zero, and the staged x86 executable. |
| Asset Needs | Existing text-mode `EDIT.COM` media only; no new guest binary, driver, firmware, or Microsoft component. |
| Reporting Requirements | Record source boundary, focused build/fixture result, staged executable identity, owner-runtime limitation, and Queue disposition in indexed evidence. |
| Similar-Issue Sweep | Inspect only the existing presentation-window keyboard input bridge and sibling mouse handoff as design references; do not sweep or alter original mouse providers. |
| Stop Conditions | Any need to modify original mouse code, guest media, graphics coordinate semantics, or a source file outside the named boundary requires renewed admission. |
| Exit Criteria | The selected text-mode route has focused and owner-runtime evidence; the reviewed result is committed and pushed. The proposal leaves Queue on admission because Queue contains candidates only. |
| Original Owner Request | “在该T任务中准入一个月S任务支持鼠标；这个鼠标支持已经在另一个工作树中可直接导入；完成提交推送后，可清除队列中相应的鼠标任务。” |

## S1 Closure Record

[T404 S1 WOW hard-error recovery evidence](../etc/evidence/m0-t404-s1-wow-hard-error-recovery-001.md).

## S2 Closure Record

[T404 S2 DOSX BOP FD contract audit](../etc/evidence/m0-t404-s2-dosx-bop-fd-contract-audit-001.md).

[M0 T403 x64 mirror and mapping retirement](../history/m0-t403-x64-mirror-retirement.md)
removed the mapping manager, identity/token paths, and active x64-only
overlays from the sole Win32/x86 product. Formal x86 link, I386 staging,
focused Redirector validation, governance checks, and owner runtime testing
passed.

## Latest Product Closure

[M0 T401 WRITE diagnostic reconciliation](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t401-s4-write-compatible-dc-non-equivalence-001.md).

## Latest Governance Closure

M0 Td S5 P1 clarified the unadmitted cross-process-broker proposal: `run16`
is the standalone BaseClient/CLI entry, `basesrv` owns broker selection and
cleanup, and `ntvdm` returns to the worker role.  It records a one-minute
idle-worker policy as a future product policy, not as an OpenNT fact.  No
product source, runtime, ABI, Queue order, or candidate admission changed.

M0 Td S2 P1 strengthened the governance gate with fixed-topology, compact
state, supporting-index, encoding, relative-link and anchor checks; it also
reconciled the public README, product UX, closed-evidence lifecycle and a
historical encoding defect. NXVM governance materials were read-only quality
references; no product, ABI, build, runtime or queue behavior changed.

M0 Td S3 P1 added governance self-test and CI execution, explicit one-candidate
one-proposal Queue rows, and mechanical Queue/TODO schema checks. No product,
ABI, build, runtime or queue ordering changed.

## Recent M0 Closures

| Task | Compact result |
| --- | --- |
| T403 | x64 mapping and overlay residue retired; formal x86 build and owner test passed. |

## Recent Governance

- **M0 Td S5 P1:** clarified the candidate-2 standalone broker split and proposed cleanup policy; no product behavior changed.
- **M0 Td S2 P1:** fixed topology, links, index, encoding, and compact status controls.
- **M0 Td S3 P1:** CI, self-test, and one-candidate/one-proposal queue controls.
- **M0 Td S3 P2:** identifier, authority schema, path allowlist, and queue/TODO checks.

## Current Technical Baseline

- The sole product and acceptance executable is the Win32/x86
  `ntvdm32.exe`, using the original SoftPC CCPU40 profile. Native x64 is not a
  product, build, runtime, or acceptance target.
- M0 T395 (CPU40 DPMI32/bootstrap), M0 T396 (WOW32 first-dispatch frontier),
  M0 T397 (first WOW16 UI/service boundary), M0 T400 (WRITE observation
  boundary), M0 T401, M0 T402, and M0 T403 are closed with their linked
  evidence and history.
- Candidate packages are authoritative only in [QUEUE.md](QUEUE.md). No
  candidate is a numeric task or active packet until owner admission here.

## Historical Status Ledger

The complete pre-compaction status ledger is retained unchanged in
[history/status-ledger-snapshot-20260910.md](../../artifacts/documentation-archive/20260910/etc/evidence/status-ledgers/status-ledger-snapshot-20260910.md).
Use it only for historical lookup; it does not define current work.

## Status Hygiene

This page contains only current packet state, compact baseline, and links to
closed records. Detailed run chronology belongs in `docs/etc/evidence/`;
closed-task facts belong in `docs/history/`.
