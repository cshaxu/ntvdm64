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

Run `powershell -ExecutionPolicy Bypass -File tools/Verify-DocumentationGovernance.ps1`
and `git diff --check` when the checkout has Git metadata available.
