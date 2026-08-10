# Source Layout

## Current Tree

```text
src/
  runner/                 owned non-invasive CLI and BYOB boundary
  opennt/overlay/         caller-proven reconstructed SoftPC semantics
  opennt/local/compat/    reached retired-host seams
  reconstruction/         default-disabled bounded fixtures
  bochs/, ms-dos-6/, ntvdmx64/, opennt-45/  retained comparison/source inputs
tests/
  runner/                 CLI and BYOB behavior
  reconstruction/         focused recovered-boundary fixtures
tools/                    isolated inventory, audit, and probe utilities
artifacts/                generated and retained evidence only
```

The last four source roots are not runtime dependencies merely because they
are present in the repository. See [Coding Rules](../rules/CODING.md) and the
source policy for admission requirements.

## Naming

Keep headers beside implementations. Use descriptive lowercase filenames and
preserve historical names in imported evidence. A local overlay or host seam
must identify its historical caller and owner in its evidence record.
