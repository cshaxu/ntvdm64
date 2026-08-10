# Governance Migration

## Status

Active incremental migration, 2026-08-09.

## Decision

The project adopts the `ntvdm64` documentation topology and its referenced
documentation-governance method: a single entry point, named authorities,
separate active status/queue/debt, and indexed historical evidence. It does
not import `ntvdm64` source, build logic, or runtime architecture.

## Retained Legacy Inventory

| Legacy location | Classification | Target family | Migration condition |
| --- | --- | --- | --- |
| `planning/status.md` | Historical expanded status ledger | `history/` or `etc/evidence/` | Extract current facts into `STATUS.md`; preserve all links. |
| `architecture/` | Supporting architecture records | `etc/architecture/` | Classify each record against `design/ARCHITECTURE.md`. |
| `governance/` | Previous governance audit records | `etc/operations/` | Preserve predecessor/successor references. |
| `research/` | Research evidence | `etc/research/` | Move only a coherent evidence family with link validation. |
| Direct uppercase records | Historical evidence/work orders | `etc/research/` or `history/` | Inventory paths and update links in the same batch. |
| Root `NUL.obj`, `stdout.txt`, `stderr.txt` | Generated-object and command-log candidates | `artifacts/host-tools/` and `artifacts/logs/` | Prove no references, retain a hash manifest, then run the default verification subset. |

## Batch Rules

1. Move one bounded, classified batch only.
2. Record source and destination paths, file count, and hashes where material.
3. Update repository-relative links in the same change.
4. Run documentation governance verification and the affected focused probe.
5. Never delete material evidence merely because it is superseded.

No migration batch is authorized by this plan alone; the active packet must
admit it.
