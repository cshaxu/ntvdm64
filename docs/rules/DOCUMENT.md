# Documentation Rules

This repository follows the documentation-governance method referenced by
`ntvdm64`: one topic has one current authority, current state is separated
from history, and supporting records are indexed rather than deleted.

## Fixed Topology

`docs/README.md` is the sole direct control document. Current task controls
live in `docs/states/` as `CURRENT.md`, `QUEUE.md`, and `TODO.md`; current
rules live in `rules/`; current design lives in `design/`; unadmitted work
belongs in `proposals/`; closed task facts live in `history/`; and `etc/`
contains indexed supporting material only.

## Authority Roles

| Record | Sole role | Must not contain |
| --- | --- | --- |
| `states/CURRENT.md` | Zero or one active M/T/S packet, current technical baseline, and compact active-task progress. A task-intermission state must explicitly say that no packet is active and link the latest closure/debt record. | A backlog, multiple active S packets, or exhaustive run logs. |
| `states/QUEUE.md` | Ordered, unnumbered candidate T packages. | Numeric T allocations, S/P entries, an active packet, or baseline. |
| `states/TODO.md` | Unplanned debt, follow-ups, and deferred concerns. | Approved work or task identifiers. |
| `proposals/` | Unadmitted candidate-package proposals. | Active packets or numbered task closure records. |
| `history/` | Closed numeric-task facts and closure evidence. | Live plans or mutable current policy. |
| `etc/` | Indexed evidence, detailed ledgers, research, and operational plans. | A competing status, queue, or architecture authority. |

The active packet may carry only compact progress. Detailed attempt/run
chronology belongs in `etc/evidence/` and is linked from Status. A complete S
brief belongs in the fixed Status packet; a multi-S working plan belongs under
`etc/operations/`.

## Migration Exception

Pre-governance records are retained only while they have a declared migration
or archival purpose. A content inventory or SHA-256 manifest may accompany an
external archive, but is not a required current-document control or a
condition for changing governed documents. New current policy, state, queue,
or design content must use the fixed topology.

## Records

New documents use lowercase hyphenated names. Each evidence record states its
question, inputs, procedure, observations, interpretation/confidence, and
follow-up. When a record becomes stale, retain it and link the successor rather
than silently rewriting or deleting material conclusions.

## Governance Gate

The documentation-governance gate is mandatory for every P and Td closure. It
rejects an extra or missing principal path, an unindexed `etc/` Markdown
record, a broken relative path or heading anchor, encoding corruption, an
invalid active/intermission state, or a `CURRENT.md` control document over 32
KiB. The size limit is a control-plane limit, not permission to discard
history: move detailed closed chronology to indexed evidence or history.

The gate validates structure, not the truth of free prose. Closure review must
still compare each changed authority with the owner request, applicable rules,
evidence, and actual repository state. Project-approved `O:\` runtime and
read-only comparison paths remain permitted where an authority explicitly
requires them; a portability check must not erase that product contract.

Every Queue candidate links exactly one proposal file; a shared program may
provide background but cannot replace a candidate proposal. `TODO.md` retains
its fixed priority, debt and admission-path table.

## Identifier Authority

The project task hierarchy is `M<milestone> T<task> S<subtask> P<part>` and is
defined by [task-identifier governance](../../artifacts/documentation-archive/20260910/strict-topology/etc/operations/task-identifier-governance.md).
Legacy `M`, `G`, `R`, `r`, and pre-policy `S` labels are evidence
aliases; they do not allocate or redefine current T tasks. `STATUS.md` owns
the sole active numeric T/S packet, `QUEUE.md` owns only unnumbered candidate
T packages, and a standalone documentation task uses `M<milestone> Td`
without consuming T.
