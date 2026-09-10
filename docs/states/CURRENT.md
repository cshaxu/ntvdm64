# Project Status

## Current Work

**Active: Td S4 P2** — governance fixture parity. The runtime recovery
queue remains owner-held; no numeric implementation packet is admitted.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | Governance — M0 Td S4 P2. |
| Admission And Approval | Continuing owner objective: match NXVM governance level. |
| Objective | Add NXVM-equivalent isolated negative fixture validation for the governance gate. |
| Non-goals | No product/runtime/source/ABI or queue ordering change. |
| Reference Baseline | Td S4 P1 `1305d97d2` added structured closure retention; fixture-level negative validation remains. |
| Files And ABI Surface | Governance documents and verifier only. |
| Applicable Rules | Documentation and execution authorities. |
| Verification | Full positive/negative fixture self-test, governance gate, links, whitespace, review, commit and push. |
| Expected Markers | Numeric and Td identifiers, closure rows/caps, required schemas and fixture rejection match policy. |
| Asset Needs | Read-only NXVM governance reference. |
| Reporting Requirements | Record parity audit and remaining deliberate differences. |
| Stop Conditions | Required product behavior change or incompatible project policy. |
| Exit Criteria | Every identified state-machine gap has executable coverage and the closure checks pass. |
| Original Owner Request | "匹配nxvm治理水平" |
| Similar-Issue Sweep | Status, Queue, TODO, proposals, history, root pointers and all verifier branches. |

## Latest Packet Closure

[M0 T403 x64 mirror and mapping retirement](../history/m0-t403-x64-mirror-retirement.md)
removed the mapping manager, identity/token paths, and active x64-only
overlays from the sole Win32/x86 product. Formal x86 link, I386 staging,
focused Redirector validation, governance checks, and owner runtime testing
passed.

## Latest Product Closure

[M0 T401 WRITE diagnostic reconciliation](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t401-s4-write-compatible-dc-non-equivalence-001.md).

## Latest Governance Closure

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
