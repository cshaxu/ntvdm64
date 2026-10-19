# Contributing

Read the [Documentation Guide](docs/README.md) and current
[Project Status](docs/states/CURRENT.md) before proposing or changing work. The
authorities linked there control this repository; this file is a submission
guide only.

## Change Record

Every change records:

- affected ownership boundary and user-visible behavior;
- source provenance, license, and redistributability effect when applicable;
- focused verification and retained evidence; and
- deferred work or an owner-approved exception.

Follow the architecture, coding, documentation, and execution rules in
`docs/rules/`. Historical source and BYOB material additionally follows the
[source policy](docs/etc/operations/policy/source-policy.md).

Run `powershell -ExecutionPolicy Bypass -File tools/governance/Verify-DocumentationGovernance.ps1`
and `git diff --check` when the checkout has Git metadata available.

## Build Output Layout

The [execution policy](docs/rules/EXECUTION.md#build-cache-and-disposable-lifetime)
owns the two cache roots, disposable-task budgets, safe cleanup and evidence
retention. Do not retain per-task compiler trees or copy a CMake build tree.
The root CMake entrypoint supports the actual MSVC source islands. Configure
from the matching Visual Studio developer environment, for example:

```powershell
cmake --preset msvc-x86-release
cmake --build --preset msvc-x86-release --target W16Style
# Other original-manifest targets: worker, wow32 (x86);
# native-worker/frontend/monitor/launcher/service, hook64 (x64).
```

## Incremental developer builds

Reconfigure the same architecture tree with the ccache preset to opt in;
ccache is not required for the ordinary preset. Direct cmake --build and
direct Ninja inherit fixed cache locations from the generated commands, not
the invoking shell's environment. Returning to the ordinary preset clears
the launcher. MSVC retains its existing /O2 release profile, not GCC's /O3.

```powershell
cmake --preset msvc-x86-release-ccache
cmake --build --preset msvc-x86-release-ccache --target W16Style
cmake -E env "CCACHE_DIR=$PWD/build/.ccache" ccache --show-stats
```

The lower-level source-owner generators still accept -CompilerCache and a
stable -BuildRoot below .ninja, using the same real CMake configuration.
Their generated runners initialize MSVC for direct Ninja. Current product
publication follows the active packet, never a disposable build/output copy.
Reconfigure after changing original source-owner manifest selections; do not
reuse an old graph as proof of a new source selection.
A cache hit is acceleration, not behavioral acceptance. Tests and compilers
run in owned background jobs as required by the execution rules.
