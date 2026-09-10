# Toolchain Probe Inventory

Date: 2026-08-07

## Purpose

`toolchain-probe/` began as an experiment workbench, so scripts, frozen
inputs, stages, logs, and compiler outputs accumulated in one directory. This
index establishes the retention boundary without rewriting historical evidence
or treating old runs as current builds.

## Categories

| Category | Current location | Governance rule |
| --- | --- | --- |
| Reproduction scripts | `toolchain-probe/*.ps1`, `*.cmd`, `*.bat` | Keep as entry points; each new output must receive an explicit `artifacts/...` path. |
| Script fixtures and small support files | `toolchain-probe/fixtures/` and adjacent headers | Keep only while named by an executable script. |
| Frozen third-party/reference inputs | `third_party/`, `reference/`, `downloads/`, `opennt-upstream-probe/`, `openwatcom-1.9/`, `openwatcom-1.9-local/` | Read-only investigation inputs; never promoted to default build dependencies. |
| Legacy generated stages and audits | Versioned `*-stage-*`, `*-probe-*`, `*-audit-*`, and managed-build directories | Freeze in place until a manifest-backed, reference-audited migration batch is approved. |
| Loose historical logs | Root `*.log`, `*.txt`, and report files | Retain as evidence; new logs belong in `artifacts/logs/` or the owning toolchain-run directory. |
| Current MASM386 evidence | `artifacts/toolchain-runs/masm386-pristine-*` | Already migrated and referenced by the pristine assembly audit. |

## Enforced Writing Rule

No new generated directory may be created below `toolchain-probe/`. Scripts
must accept an output/stage argument or default below `artifacts/`. The support
provider script is the first migrated example: it uses
`artifacts/toolchain-runs/opennt-support-provider-current` by default and
requires an explicit 32-bit clang-cl target.

## Migration Gate

Migrate a legacy category only after all of the following are recorded:

1. Exact script and documentation references.
2. Pre- and post-move tree inventory: file count, byte count, and ordered
   SHA-256 digest.
3. Current default build/test verification.
4. The smallest specific toolchain or artifact probe that proves the moved
   entry point still writes to its designated `artifacts` location.

This is intentionally incremental. Historical stages are valuable provenance,
and a directory cleanup must not convert them into undocumented or misleading
"current" results.
