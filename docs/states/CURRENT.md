# Project Status

## Current Work

**Active: M0 T404 S4**

## Active Packet

### M0 T404 S4 — productized single-process WOW bootstrap recovery

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T404 S4, Ordinary Mode. S3's mouse-usability packet is closed; this is the next sequential bounded recovery packet. |
| Admission And Approval | Owner directed on 2026-09-11: “你将S3收口。当前T任务是跑通write.exe，启动链任务交给S4”. |
| Candidate Proposal | S4 follows the delivered [P15 positional NE-to-WOW recovery](../proposals/proposal-positional-ne-wow-entry-recovery-001.md) and its [entry evidence](../etc/evidence/m0-t404-s3-p15-positional-ne-wow-entry-recovery-001.md). The newly audited A200 baseline is evidence only, not a source import or binary dependency. |
| Reference Baseline | P15 preserves a positional NE as a copied WOW record (`state=0102`, owner `2`) but cannot provide original `GetWowKernelCmdLine` with a real `-a` process token in the present one-process composition. A200 demonstrates the valid alternate single-process closure: internal `-w/-a` selects WOW and a package-root-limited fallback supplies `KRNL386.EXE`; its record is implicit v2 metadata and its public `--command` CLI is retired. |
| Applicable Rules | [Execution](../rules/EXECUTION.md), [architecture](../rules/ARCHITECTURE.md), [coding](../rules/CODING.md), [document](../rules/DOCUMENT.md), and the [source policy](../etc/operations/policy/source-policy.md). |
| Objective | Productize the single-process Win16 bootstrap behind only `ntvdm32.exe <binary>`, retaining P15's explicit WOW record and recovering the original `KRNL386.EXE → DOSX.EXE → WOW32.DLL` path toward WRITE. |
| Non-goals | Public worker/bootstrap flags; a new process, BaseSrv executable, cross-process broker, WOW pooling, CCPU/BOP/guest-media/Console changes, a new WOW/DOSX loader, or using A200 binaries/source snapshot as a production dependency. |
| Files And ABI Surface | Expected narrow surfaces: `src/app/launch_declaration.[ch]`, `src/mvdm-host/dos/command/cmdmisc.c`, a bounded existing/new `adapter-mvdm-host-out` declaration provider, focused fixtures, build graph and indexed evidence. P15 broker record v3 remains fixed-width and explicit; no guest ABI changes. |
| Verification | Fresh Win32/x86 CPU40 build under `build/M0-T404/S4/`; focused tests for real `-a` precedence, session-bound fallback success and unavailable fallback failure; DOS positional non-regression; formal x86 link; public positional WRITE run with default-off DEM-open/module observers; governance and diff review. |
| Expected Markers | No public `-w`, `-a`, `-o` or `--command` is accepted; a classified NE receives an explicit WOW record, a verified package `KRNL386.EXE` carrier, empty WOW PIF/default bootstrap, then runtime opens `KRNL386.EXE`, `DOSX.EXE`, loads `WOW32.DLL`, and advances WRITE beyond the current entry frontier. |
| Asset Needs | Existing selected OpenNT/MVDM source, packaged KRNL386/DOSX/WRITE media, build tooling and owner runtime package only; no new source, guest binary, firmware or Microsoft component. |
| Reporting Requirements | Record the four-rung recovery audit; exact original parser/fallback ordering; session/path lifetime and failure result; DOS/WOW profile disposition; focused test/build identities; public runtime observations; staged hash only if non-regressive; and remaining WRITE frontier. |
| Current Evidence | [Run 001](../etc/evidence/m0-t404-s4-positional-wow-bootstrap-run-001.md) proves positional WRITE reaches the original DOSX bootstrap and then repeats at original WOW BOP `50:01`, without an illegal-instruction or host-exception report. The parent formal graph has no same-version WOW32 provider target; current deployed `WOW32.DLL` is an independent stale artifact. |
| Similar-Issue Sweep | Audit all direct `GetCommandLine` WOW bootstrap consumers, every app-created initial PIF profile, every `command_owner` serialization/consumer, and every public option parser; preserve DOS and native-image behavior. |
| Stop Conditions | Need for a cross-process service/worker, a new historical source import, CCPU/BOP/guest-media/Console changes, a non-session-bound path search, a guest-visible host pointer, or DOS positional regression pauses work for renewed admission. |
| Exit Criteria | The sole public positional WRITE command demonstrably reaches `KRNL386.EXE`, `DOSX.EXE`, and `WOW32.DLL` through the retained original owners, with focused checks and formal x86 build passing. A WRITE UI/document/lifecycle claim requires its own direct evidence. |
| Original Owner Request | “当前T任务是跑通write.exe，启动链任务交给S4”. |

## S3 Closure Record

S3 is closed as the native Console mouse-usability packet. Its delivered acceptance is the owner-verified [P12 mouse record](../etc/evidence/m0-t404-s3-p12-native-console-mouse-interaction-audit-001.md): host mouse movement and button transitions reach the original guest mouse path without the prior false-click/help-modal behavior. P13--P15 are retained startup-chain investigations and entry recovery evidence, not part of S3's mouse closure. They establish S4's baseline only; they do not claim KRNL386, DOSX, WOW32 or WRITE execution.

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
