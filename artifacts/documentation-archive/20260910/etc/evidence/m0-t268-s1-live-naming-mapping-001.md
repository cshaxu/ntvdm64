# M0 T268 S1 — Live Naming Mapping

## Scope And Baseline

The accompanying TSV inventory records 8,728 live occurrences in 601 files
across source, tests, tools, current documentation, build metadata and
runtime-facing schemas. It excludes archived historical-record documentation.

## Mapping Rules

| Retired class | Semantic replacement | Application rule |
| --- | --- | --- |
| Former project prefix | `runner` for executable/CLI/build ownership; `RUNNER` for process-local configuration; `runtime` for generic mechanical contracts | Choose the narrow owner word rather than a new product prefix. |
| Former machine-assembly label | `machine` | Use for machine lifecycle, state, fixture and adapter concepts. |
| First temporary generation suffix | Remove when it is the sole surviving implementation; otherwise select `legacy` only for a deliberately retained prior implementation. | Never retain a numeric generation marker. |
| Second temporary generation suffix | Remove when no collision exists; otherwise select `direct`, `native`, `source`, or another owner-specific behavior word. | Never retain a numeric generation marker. |

## Collision Decisions

- The two historical BIOS bridge source pairs that collapse after suffix removal
  are renamed as `historical_bios_bridge_legacy` and
  `historical_bios_bridge`; their roles, rather than their former generation
  number, distinguish them.
- A source-path transformation from the four rules has 140 current file
  candidates and only those two collisions. All remaining file moves are
  therefore deterministic once the pair is resolved.

## Mirror Identity Exception

Five version annotations in `opennt-guest` are byte-preserved source comments
from the selected original guest input. They identify the upstream material,
not a project generation. They remain subject to the component mirror register
rather than this rename. No project-authored symbol, path, target, macro or
schema receives that exception.

## S2 Preconditions

1. Apply path moves with Git history and the collision names above.
2. Apply symbol and macro replacement per owner, never as an unreviewed
   repository-wide textual substitution.
3. Regenerate the formal build graph after each component batch; no stale
   output is evidence of a successful rename.