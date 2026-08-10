# Pre-Governance Execution Workflow Record

> **Superseded as the current execution authority on 2026-08-09.** Use
> [rules/EXECUTION.md](rules/EXECUTION.md). This detailed record remains
> supporting evidence during the controlled documentation migration.

## One Active Task

`docs/planning/status.md` is the live task ledger. Keep one implementation or investigation task in progress at a time. Discovered follow-ups remain queued until the current task has an evidence-backed result.

## Task Packet

Before changing source, build files, or governance, create or update a task entry containing:

1. Request and desired outcome.
2. Scope and explicit boundaries.
3. Relevant source, documents, commands, and existing evidence.
4. Acceptance criteria and verification commands.
5. Known risks, especially source provenance and ABI uncertainty.

## Investigation Loop

1. Read current authorities and nearest evidence records.
2. Reproduce the smallest relevant build or runtime observation.
3. Identify the first missing file, API, ABI contract, toolchain assumption, or host boundary.
4. Choose the narrowest high-ROI probe or repair consistent with the source and coding policies.
5. Record command, artifact, output, interpretation, and next action.

## Completion And Artifact Hygiene

On completion, move the task from active to completed in the status ledger, link the evidence record, list verification results, and queue only the next concrete follow-up. Generated binaries, logs, object files, and build trees are not documentation authorities; when retained as evidence, state how they were produced and whether they can be recreated.
