# Pre-Governance Document Map

> **Superseded as the current classification authority on 2026-08-09.** Use
> [../README.md](../README.md), [../rules/DOCUMENT.md](../rules/DOCUMENT.md),
> and [../etc/operations/governance-migration.md](../etc/operations/governance-migration.md).
> This file remains retained migration evidence.

## Current Authorities

When an older evidence or work-order document conflicts with the current
SoftPC route, `docs/SOFTPC-RECONSTRUCTION-GOVERNANCE.md` controls source role,
implementation ownership, and acceptance. In particular, no document may turn
an external core, mantle, emulator, or CPU backend into a prerequisite or
substitute for OpenNT SoftPC reconstruction.

| Topic | Authority |
| --- | --- |
| Purpose, scope, and research framing | `RESEARCH-PROJECT.md` |
| Approved technical sequence | `ROADMAP.md` |
| System ownership and integration boundary | `docs/architecture/overview.md` |
| Historical inputs and provenance rules | `docs/source-policy.md` |
| Documentation rules | `docs/documentation-standard.md` |
| Source modification discipline | `docs/coding-standard.md` |
| Task lifecycle | `docs/execution-workflow.md` |
| Live work state | `docs/planning/status.md` |
| This classification | `docs/governance/document-map.md` |

The active execution queue is only the ordered SoftPC reconstruction sequence
in `ROADMAP.md`, interpreted through
`docs/SOFTPC-RECONSTRUCTION-GOVERNANCE.md`. A document that discusses core,
mantle, an external wrapper, a replacement CPU, or an external emulator cannot
create a current task, implementation prerequisite, build dependency, or
acceptance condition.

## Existing Evidence Families

The following remain evidence or work-order records, not competing current policy:

| Family | Examples | Classification |
| --- | --- | --- |
| Source acquisition and gates | `HISTORICAL-SOURCE-INPUT-GATES.md`, `OPENNT-BUILD-BOOTSTRAP-AUDIT.md` | Provenance and bootstrap evidence |
| Toolchain and artifact research | `HISTORICAL-TOOLCHAIN-PROVENANCE-MATRIX.md`, `OPENNT-ARTIFACT-MATRIX.md` | Build evidence |
| ABI, API, and subsystem probes | `ABI-*`, `API-*`, `MVDM-*`, `NTDOS-*` records | Compatibility evidence |
| Milestone work | `artifacts/reports/legacy-root-milestones/M11-*.md` and related records | Retained legacy milestone history |
| Directional analysis | `ROI-DRIVEN-EXECUTION-ROADMAP.md` and related reports | Supporting analysis only; `ROADMAP.md` remains the current queue |
| External core/mantle/backend proposals | `BACKEND-ADOPTION-READINESS.md`, `CORE-*`, `MANTLE-*`, `OWNED-MONITOR-*`, `CPU-EXECUTION-BACKEND-DECISION.md`, `CCPU-STANDALONE-BACKEND-ASSESSMENT.md`, `CCPU-SEMANTIC-RECONSTRUCTION-RECOVERY-MATRIX.md`, and `NTVDM64-*` records | Archived cross-project observations. They cannot schedule, gate, link, or replace OpenNT SoftPC reconstruction; `SOFTPC-RECONSTRUCTION-GOVERNANCE.md` controls any conflict. |
| Build and validation notes | `BUILD-*`, `VALIDATION-*`, `TEST-*` records | Operational evidence |

## Migration Rules

- Do not rename or move existing evidence during routine work.
- When a new record supersedes an older conclusion, add a successor link in the newer record and, when practical, a short supersession note in the older one.
- New policy belongs in named authority documents. New observations belong in evidence records below `docs/`.
- New root documents require a reason in `planning/status.md`; the default is `docs/`.

## Known Governance Gaps

1. The evidence catalog is broad but not fully cross-linked by subsystem.
2. Some root milestone files may duplicate conclusions later recorded in `docs/`; preserve them until a focused comparison identifies a successor.
3. Generated build outputs and transient logs are governed by
   `directory-reorganization.md` and `toolchain-probe-inventory.md`; retained
   Microsoft binary comparison artifacts additionally follow
   `source-policy.md`'s Research Binary Artifacts rule. They are evidence, not
   documentation authorities.
