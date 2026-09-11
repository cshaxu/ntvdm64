# Project Status

## Current Work

**Active: M0 T404 S3**

## Active Packet

### M0 T404 S3 P15 — positional NE-to-WOW record-entry recovery (delivered; actual-worker limitation)

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T404 S3 P15, Ordinary Mode, delivered with an explicit actual-worker command-line limitation. |
| Admission And Approval | Owner approved on 2026-09-11: “批准！”. |
| Candidate Proposal | [Positional NE-to-WOW entry recovery](../proposals/proposal-positional-ne-wow-entry-recovery-001.md); source basis: OpenNT [`BaseSrvCheckVDM`/`BaseSrvCheckWOW`](../../src/opennt-host/base/win32/server/srvvdm.c), original MVDM [`GetWowKernelCmdLine`](../../src/mvdm-host/dos/command/cmdmisc.c), and P14's [limited-coverage evidence](../etc/evidence/m0-t404-s3-p14-cpu40-dosx-direct-continuation-recovery-001.md). |
| Reference Baseline | Positional `ntvdm32.exe system32\\WRITE.EXE` is correctly classified as Win16/NE, but the current app publishes it as a DOS-only BaseVDM record and starts no WOW worker bootstrap. Original `BaseSrvCheckVDM` directs Win16 to `BaseSrvCheckWOW`; original `GetWowKernelCmdLine` reads the worker's `-a <KRNL386.EXE>` bootstrap path. |
| Applicable Rules | [Execution](../rules/EXECUTION.md), [architecture](../rules/ARCHITECTURE.md), [coding](../rules/CODING.md), [document](../rules/DOCUMENT.md), and the [source policy](../etc/operations/policy/source-policy.md). |
| Objective | Recover the positional NE-to-WOW copied-record selection and derive the original worker bootstrap argv, while determining whether the current one-process composition can satisfy original `GetWowKernelCmdLine`. |
| Non-goals | Changing CCPU execution, BOP bytes or CPU profiles; changing guest media; accepting user-supplied worker bootstrap parameters; a cross-process broker split; shared-WOW pooling; Console behavior; or implementing a new WOW/DOSX loader. |
| Files And ABI Surface | Expected: `src/app/entry.c`, `src/app/launch_declaration.[ch]`, `src/broker/base_vdm_record.[ch]`, `src/adapter-mvdm-host-out/basesrv/source/base_vdm_broker.c`, focused fixture(s), and indexed evidence. Broker's fixed-width copied record version may advance; no guest ABI changes. |
| Verification | Fresh Win32/x86 CPU40 build under `build/M0-T404/S3/`; focused classification/broker tests proving DOS and WOW selection; source recovery ledger; formal x86 link; positional `WRITE.EXE` run with default-off BaseVDM and DEM-open observers; documentation governance and diff review. |
| Expected Markers | `WRITE.EXE` produces a WOW-owned request, original `GetWowKernelCmdLine` receives a package-relative `KRNL386.EXE` path, and runtime observers show `KRNL386.EXE` then `DOSX.EXE` before any claimed WRITE result. |
| Asset Needs | Existing selected OpenNT and MVDM source, existing package `system32\\KRNL386.EXE`, build tooling and owner runtime package only; no new source, guest binary, firmware, or Microsoft component. |
| Reporting Requirements | Record four-rung recovery audit, exact source paths/ordering, changed wire version and compatibility boundary, focused positive/negative test results, x86 build identity, positional runtime result, staged hash if eligible, and any remaining frontier. |
| Similar-Issue Sweep | Audit every local broker rejection/serialization of `command_owner`, every app command-classification caller, and all current positional Win16 handling; preserve DOS and native-image behavior. |
| Stop Conditions | Need for CCPU/BOP/guest-media/Console changes, shared-WOW reuse, a new broker process, a new historical source import, a worker bootstrap path not expressible through the current bounded declaration, or a regression of DOS positional launch requires pausing and renewed admission. |
| Exit Criteria | Met with publication limitation: positional NE derives the source-shaped worker argv and matching copied WOW record; DOS remains DOS; focused tests and formal x86 build pass; positional runtime proves the WOW record. Original `GetCommandLine()` still cannot observe the in-memory argv, so KRNL386/DOSX is not reached and the candidate is not published. |
| Original Owner Request | “批准！” following the identified app/broker NE-to-WOW recovery boundary. |

P12's [mouse acceptance record](../etc/evidence/m0-t404-s3-p12-native-console-mouse-interaction-audit-001.md), P13's [BOP entry audit](../etc/evidence/m0-t404-s3-p13-cpu40-bop-entry-contract-audit-001.md), and P14's direct CPU40 continuation repair are delivered. P14's test result did not reach DOSX; after the owner approved this renewed, bounded app/broker recovery, P15 is the sole active packet. The package remains restored to P12 until P15 proves a non-regressive positional launch.

P15's [entry-recovery evidence](../etc/evidence/m0-t404-s3-p15-positional-ne-wow-entry-recovery-001.md) proves the positional NE request now reaches the WOW record (`state=0102`, owner `2`) and records focused x86 build/fixture success. It also reaches P15's stop condition: original `GetWowKernelCmdLine` reads the real worker `GetCommandLine()`, while the current one-process composition has only an in-memory SoftPC argv. No KRNL386/DOSX result was reached; P12 remains staged and any actual-worker/bootstrap bridge needs renewed admission.

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
