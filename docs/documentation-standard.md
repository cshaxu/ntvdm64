# Pre-Governance Documentation Standard Record

> **Superseded as the current documentation authority on 2026-08-09.** Use
> [rules/DOCUMENT.md](rules/DOCUMENT.md). This detailed record remains
> supporting evidence during the controlled documentation migration.

## Purpose

Documentation must make research decisions reproducible without creating several competing versions of the same truth.

## Authority Model

Each topic has one current authority. A document may cite evidence, but it must not create a second current policy or status page.

| Topic | Current authority |
| --- | --- |
| Project purpose and scope | `RESEARCH-PROJECT.md` |
| Technical direction and milestones | `ROADMAP.md` |
| System boundaries and component ownership | `docs/architecture/overview.md` |
| Historical source and reference use | `docs/source-policy.md` |
| Source-change discipline | `docs/coding-standard.md` |
| Working procedure | `docs/execution-workflow.md` |
| Active task and known blockers | `docs/planning/status.md` |
| Document classification and migration | `docs/governance/document-map.md` |

When a new document overlaps an authority, extend that authority or state explicitly that the new file is evidence, a proposal, or a superseded record.

## Placement And Naming

- Put new policy, architecture, planning, and governance documents in the corresponding `docs/` subdirectory.
- Use lowercase, hyphenated names for new governance documents, such as `abi-probe-notes.md` or `toolchain-decision-2026-08-07.md`.
- Keep existing uppercase file names where they already carry historical links; do not rename them merely for style.
- Root Markdown is reserved for `README.md`, `RESEARCH-PROJECT.md`, `ROADMAP.md`, and existing legacy milestone records.
- Add a short `Status`, `Scope`, or `Decision` section near the top of a record whose conclusion could otherwise be mistaken for current policy.

## Evidence Records

A research, build, ABI, or compatibility record should include:

1. Question or hypothesis.
2. Inputs: source tree, snapshot, toolchain, host, and relevant paths.
3. Exact command or procedure.
4. Observed output, artifacts, or failure signature.
5. Interpretation and confidence level.
6. Follow-up, owner, or explicit completion statement.

Use links to source paths and retained logs where possible. Do not replace an observed result with a retrospective summary only.

## Current State Versus History

`docs/planning/status.md` is the only current task ledger. A completed task moves out of the active section and leaves a link to its evidence record. Historical documents remain valuable provenance, but their opening section must not imply that they override a newer authority.

## Retirement And Link Discipline

Do not delete or rewrite a material conclusion solely because it became stale. Mark it superseded, identify the successor, and retain enough context to explain why the decision changed. Use repository-relative links and verify every link introduced by a documentation change.
