# Generated Artifacts

This directory contains generated outputs and retained research evidence. It is
not a source, build-input, or release directory.

## Layout

| Directory | Contents |
| --- | --- |
| `host-tools/` | Host-built helper objects and executables |
| `logs/` | Captured build and probe logs |
| `reports/` | Retained historical research reports and their migration manifests |
| `build/current/` | Reproducible current CMake or compiler build trees |
| `build/legacy/` | Non-relocatable historical build caches retained as evidence |
| `stages/` | Generated historical-source stages and manifests |
| `toolchain-runs/` | Reproducible assembler, linker, format-tool, and host-ABI comparisons |

Every retained artifact batch must be recorded in
`docs/governance/directory-reorganization.md` with its source, reason for
retention, and post-move verification result.
