# Proposal — Repository-local source closure and `refs` retirement

## Objective

Eliminate the repository-local `refs/` tree only after every live build,
configuration, include, tool, probe and test input has an owned in-repository
location. External repositories remain import-time and audit-time comparison
sources only. Retained reports move only to the owner-approved
`artifacts/research/` archive with source, role, hash, size and disposition
metadata.

## Required outcome

- No formal build, runtime, configuration, include, tool, probe or test reads
  `refs/`.
- Every retained Bochs input is under `src/bochs-core` or
  `src/adapter-bochs`; every retained OpenNT input is in its named
  `src/opennt-*` owner; project tooling and fixtures live under `tools/` and
  `tests/`.
- `refs/` is absent only after a repository-local formal Ninja closure and
  zero-reference scans prove the claimed boundary.
- Historical narrative/evidence may retain immutable textual provenance such
  as `refs/opennt/...`; it is neither a live dependency nor a reason to retain
  the tree.

## Packets

| S | Scope | Exit criterion |
| --- | --- | --- |
| S1 | Inventory every consumer and snapshot root; produce the migration/disposal ledger and order. | Every live consumer and each snapshot root has an owner/disposition and no ambiguous source input. |
| S2 | Move or rewrite live Bochs formal build/config/include consumers to `bochs-core`/`adapter-bochs`; update current fixtures. | Formal Ninja and current Bochs boundary tests resolve no `refs/bochs` path. |
| S3 | Move or rewrite reached OpenNT source/tool/probe consumers to named OpenNT roots; classify any missing owned original as a source import requirement. | Current provider/governance tools and guest staging resolve no `refs/opennt` path. |
| S4 | Retire or re-root historical-only build/probe scripts and tests; archive approved reports with manifests. | No tracked executable tool/probe/test has a `refs/` dependency; reports are under `artifacts/research/`. |
| S5 | Remove the `refs/` tree, remove its ignore/configuration treatment, and run zero-reference plus repository-local formal Ninja/test closure. | `refs/` is absent and all declared live scans/builds pass without it. |

## Non-goals

- Do not make dormant historical packages production source merely to avoid deletion.
- Do not alter recovered OpenNT or Bochs semantics, source ownership, or component boundaries.
- Do not preserve a local source archive under another generic root; retained provenance is external or a named original component.

## Verification

The final packet records a machine-readable consumer ledger, an archived-report manifest, tracked zero-reference scans separated from immutable historical documentation, repository-local Ninja build output and focused current fixture results. Each source move remains subject to the source policy and the mirror component standard.