# M0 T266 S4 historical-consumer archive report

## Result

- Retained test source: 7 reconstructed monitor/transport/video files were moved from the repository archive to `tests/legacy/reconstruction/`; their CMake targets remain default-disabled.
- Retired executable evidence: 34 historical scripts/manifest inputs were relocated to `docs/etc/legacy_code/`; PowerShell source names now end in `.ps1.txt`, so they are not runnable project tools.
- `refs/archive/reconstruction/` contained source inputs, not a standalone report. No source report was copied into an artifact package. This report and `archive-manifest.json` are the formal archive record.

## Integrity

`archive-manifest.json` lists each old path, destination, role, byte count and SHA-256. It is generated from the S4 Git rename set and is a research/report artifact only; it is not a runtime, build or source input.

## Boundary

The remaining textual `refs/...` mentions in documentation are immutable provenance. S5 alone removes the root and its ignore treatment after a repository-local closure check.
