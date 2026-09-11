# Project Status

## Current Work

**Active: M0 T404 S3**

## Active Packet

### M0 T404 S3 P14 — CPU40 DOSX direct-continuation recovery (delivered; runtime coverage limited)

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T404 S3 P14, Ordinary Mode, delivered with a recorded standalone WOW/DOSX runtime-coverage limitation. |
| Admission And Approval | Owner approved on 2026-09-11: “批准修复，准入一个P提交”. |
| Candidate Proposal | [CPU40 DOSX direct-continuation recovery](../proposals/proposal-cpu40-dosx-direct-continuation-recovery-001.md); source basis: [P13 audit](../etc/evidence/m0-t404-s3-p13-cpu40-bop-entry-contract-audit-001.md); delivered [P14 evidence](../etc/evidence/m0-t404-s3-p14-cpu40-dosx-direct-continuation-recovery-001.md). |
| Reference Baseline | Selected x86 CPU40 source retains `HOST_BOP_IP_FUDGE = -2`. P13 proves the two `reset.c` inputs originate in original DOSX `enrm45`/`enrm50` continuations, not BOP stubs. |
| Applicable Rules | [Execution](../rules/EXECUTION.md), [architecture](../rules/ARCHITECTURE.md), [coding](../rules/CODING.md), [document](../rules/DOCUMENT.md), and the [source policy](../etc/operations/policy/source-policy.md). |
| Objective | Under `CPU_40_STYLE`, enter the two original DOSX continuation addresses in `reset.c` directly, without changing their historical non-CPU40 behavior; establish focused DOSX/WOW startup evidence. |
| Non-goals | Changing CCPU decoding/execution, global `HOST_BOP_IP_FUDGE`, CPU30, any actual BOP route, guest media, callback ABI, Console behavior, adapters, or broker architecture. |
| Files And ABI Surface | `src/mvdm-host/softpc.new/base/bios/reset.c`; selected x86 build/publication records; indexed P14 evidence. No ABI signature changes. |
| Verification | Fresh Win32/x86 CPU40 build under `build/M0-T404/S3/`; source inspection that CPU40 alone bypasses `-2`; bounded DOSX/WOW startup run; staging only after a non-regressive result; diff and documentation governance checks. |
| Expected Markers | Both shutdown code 9 and 0Ah select their original `enrm` target offsets exactly under CPU40; no new BOP bytes, no changed non-CPU40 expression, and a recorded startup result. |
| Asset Needs | Existing selected source, build tooling, and existing owner runtime package only; no new source, guest binary, firmware, or Microsoft component. |
| Reporting Requirements | Record original-source provenance, exact two-site diff, x86 build identity, runtime outcome/limitation, staging hash if published, and deferred global-BOP disposition. |
| Similar-Issue Sweep | Reconfirm all `HOST_BOP_IP_FUDGE` consumers and direct BOP constructors; do not alter consumers outside the two P13-proven DOSX routes. |
| Stop Conditions | A failed/regressive DOSX/WOW result, a need to touch CCPU/global BOP/CPU30, runtime instrumentation, guest modifications, or another route requires stopping and renewed admission. |
| Exit Criteria | Met with publication limitation: both CPU40 sites are direct while non-CPU40 remains unchanged; fresh x86 build and bounded WOW/DOSX result are recorded; the new executable was not published because the run did not reach DOSX and left test workers, so the accepted P12 package was restored. |
| Original Owner Request | “批准修复，准入一个P提交” |

P12's [mouse acceptance record](../etc/evidence/m0-t404-s3-p12-native-console-mouse-interaction-audit-001.md) and P13's [BOP entry audit](../etc/evidence/m0-t404-s3-p13-cpu40-bop-entry-contract-audit-001.md) are delivered. P14 delivered the two CPU40-only direct continuation selections and x86 link. Its standalone WOW launch did not reach DOSX and left bounded test workers, so the product package is restored to P12 and any lifecycle/DOSX observation follow-up requires renewed admission.

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
