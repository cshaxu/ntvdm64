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

## Migration Exception

The existing direct documents and `architecture/`, `governance/`, `planning/`,
and `research/` directories predate this topology. They are read-only retained
evidence during the incremental migration defined in
`etc/operations/governance-migration.md`. New current policy, status, queue,
or design content must use the fixed topology. Do not mass-move evidence or
break inbound links.

## Records

New documents use lowercase hyphenated names. Each evidence record states its
question, inputs, procedure, observations, interpretation/confidence, and
follow-up. When a record becomes stale, retain it and link the successor rather
than silently rewriting or deleting material conclusions.
