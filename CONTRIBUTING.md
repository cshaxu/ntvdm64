# Contributing

Read the [Documentation Guide](docs/README.md) and current
[Project Status](docs/STATUS.md) before proposing or changing work. The
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
cmake -S . -B build/M0-T218-S1/001 -DNTDOS64_TASK_ID=M0-T218-S1
cmake --build build/M0-T218-S1/001
```

Do not invoke a compiler from the repository root without an explicit output
path under `build/<task-id>/<run-id>/`; do not use `artifacts/` as a temporary build
directory.
