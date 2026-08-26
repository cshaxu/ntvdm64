# Documentation Rules

This repository follows the documentation-governance method referenced by
`ntvdm64`: one topic has one current authority, current state is separated
from history, and supporting records are indexed rather than deleted.

## Fixed Topology

`docs/` has four direct control documents—`README.md`, `STATUS.md`,
`QUEUE.md`, and `TODO.md`—and four authority directories: `rules/`, `design/`,
`history/`, and `etc/`. `rules/` contains enforceable constraints; `design/`
contains current product decisions; `history/` contains closed task facts; and
`etc/` contains indexed supporting material only.

## Authority Roles

| Record | Sole role | Must not contain |
| --- | --- | --- |
| `STATUS.md` | Zero or one active M/T/S packet, current technical baseline, and compact active-task progress. A task-intermission state must explicitly say that no packet is active and link the latest closure/debt record. | A backlog, multiple active S packets, or exhaustive run logs. |
| `QUEUE.md` | Ordered, unnumbered candidate T packages. | Numeric T allocations, S/P entries, an active packet, or baseline. |
| `TODO.md` | Unplanned debt, follow-ups, and deferred concerns. | Approved work or task identifiers. |
| `history/` | Closed numeric-task facts and closure evidence. | Live plans or mutable current policy. |
| `etc/` | Indexed evidence, detailed ledgers, research, and operational plans. | A competing status, queue, or architecture authority. |

The active packet may carry only compact progress. Detailed attempt/run
chronology belongs in `etc/evidence/` and is linked from Status. A complete S
brief belongs in the fixed Status packet; a multi-S working plan belongs under
`etc/operations/`.

## Migration Exception

The existing direct evidence documents and `architecture/`, `governance/`,
`planning/`, and `research/` directories predate this topology. They are
read-only retained evidence during the incremental migration defined in
`etc/operations/governance-migration.md`. Every Markdown file, including
these retained records and non-principal design detail, must appear with its
SHA-256 in `etc/operations/document-inventory.md`; the gate verifies this
exact-file snapshot. New current policy, status, queue, or design content must
use the fixed topology. Do not mass-move evidence or break inbound links.

## Records

New documents use lowercase hyphenated names. Each evidence record states its
question, inputs, procedure, observations, interpretation/confidence, and
follow-up. When a record becomes stale, retain it and link the successor rather
than silently rewriting or deleting material conclusions.

## Identifier Authority

The project task hierarchy is `M<milestone> T<task> S<subtask> P<part>` and is
defined by [task-identifier governance](../etc/operations/task-identifier-governance.md).
Legacy `M`, `G`, `R`, `r`, and pre-policy `S` labels are evidence
aliases; they do not allocate or redefine current T tasks. `STATUS.md` owns
the sole active numeric T/S packet, `QUEUE.md` owns only unnumbered candidate
T packages, and a standalone documentation task uses `M<milestone> Td`
without consuming T.
