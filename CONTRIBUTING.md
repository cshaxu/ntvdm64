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

Use `build/<task-id>/<run-id>/` for disposable configure, compiler, linker and
debug output. Evidence, manifests and conclusions belong in `docs/etc/`.
`artifacts/` is reserved for reports explicitly requested by the owner and
formal versioned executable deliverables under
`artifacts/build/<task-id>-<version>/` with their manifest.
For example:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T260S8FullNinjaGraph.ps1 `
  -RepositoryRoot (Get-Location).Path `
  -BuildRoot build/M0-T267-S1/r001
ninja -C build/M0-T267-S1/r001
```

The root CMake catalogue has been removed; it is not a supported configuration entrypoint.

## Incremental developer builds

Ninja reuses objects only inside the same build root.  For ordinary local
development, retain one root such as `build/developer/x86` rather than creating
a new task run directory for every edit.  The formal task/evidence build roots
remain disposable and independent.

When `ccache.exe` is installed, opt in explicitly; the generator leaves it
disabled by default.  This route stores the cache in `build/compiler-cache`,
sets only process-local ccache configuration, and leaves the selected MSVC
flags unchanged:

```powershell
$ccache = (Get-Command ccache.exe -ErrorAction Stop).Source
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/build/New-T310OriginalSoftpcNinja.ps1 `
  -Architecture x86 `
  -RepositoryRoot (Get-Location).Path `
  -BuildRoot build/developer/x86 `
  -CompilerCache $ccache `
  -NodeExecutable $env:MVDM_NODE22
cmd.exe /d /c build\developer\x86\run-ninja-parallel.cmd ntvdm.exe
```

Use the generated `run-ninja-parallel.cmd`, not a direct compiler call: it
initializes MSVC, configures only that build's ccache process environment, and
honors `MVDM_BUILD_JOBS`.  A cache hit accelerates compilation; it is never
verification evidence and does not replace a clean task build.

The native-component, Hook64, and WOW32 Ninja generators accept the same
optional `-CompilerCache $ccache` argument.  Omit that argument for a formal
or task-evidence graph: its generated rules remain direct `cl.exe` commands.

Do not invoke a compiler from the repository root without an explicit output
path under `build/<task-id>/<run-id>/`; do not use `artifacts/` as a temporary build
directory.

After `ntvdm32.exe` has passed its admitted x86 verification and makes a
recorded improvement over the preceding published candidate, copy that tested
EXE and its required runnable package inputs to
`build/output/` for owner testing. Record any known limitation beside it; do
not publish an unverified candidate or one with a clear regression.
