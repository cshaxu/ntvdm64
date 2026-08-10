# Execution Rules

## Admission

`STATUS.md` is the only active task packet. Before source, build, or
governance changes, record objective, scope, non-goals, evidence, verification,
risks, stop conditions, and exit criteria there. `QUEUE.md` candidates receive
no task identifier or active status until approved.

Task identifiers use `T` followed by a zero-padded serial. The historical
crosswalk in `history/task-index.md` reserves `T001`--`T093`; the next active
or approved task uses the next unused serial. Legacy identifiers remain
evidence aliases only.

## Evidence And Closure

Every investigation records exact inputs, commands/procedure, observed output,
interpretation, confidence, and follow-up in `etc/` or a numbered `history/`
record. A closure updates `STATUS.md`, links its evidence, and moves only the
next concrete candidate to the queue.

## Verification

Run focused checks proportionate to the changed boundary. Every documentation
closure runs `tools/Verify-DocumentationGovernance.ps1`; run `git diff --check`
when Git metadata is available. A failed or unavailable verification is a
recorded limitation, not a passing result.
