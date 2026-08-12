# Legacy Identifier Index

## Status

The former `T001`--`T093` crosswalk was an invalid renumbering introduced
during the initial documentation migration. It is retired. `M0`--`M92`,
`G0.4`, and `R1`--`R5` remain historical record identifiers only; they are not
available `T` task numbers and do not define the current task sequence.

## Current Numbering Authority

The current task sequence is governed by
[task-identifier-governance.md](../etc/operations/task-identifier-governance.md).
Its active implementation packet is **M0 T95**. New task numbers continue
strictly from the last closed numeric `T`; they are never inferred from legacy
`M`, `G`, or `R` labels.

Lowercase `r<n>` build and fixture labels are also legacy run evidence, not
task identifiers. Their current `M0 T95 S1` mapping is recorded in
`../etc/operations/task-identifier-governance.md`.

## Historical Lookup

Use the pre-governance [status ledger](../planning/status.md) and the retained
evidence records to locate legacy work. The presence of a legacy identifier is
evidence of an earlier record, not proof that it is a current task, milestone,
subtask, or deliverable.
