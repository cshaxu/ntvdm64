# T95 S6 MSVC configuration projection-tool repair

## Question and scope

Can the proposed projection tool represent each parsed `#define` as exactly
one definition record and reject duplicate definitions before it creates an
output root?  This repair is limited to that internal representation and its
synthetic-input test.  It does not project `src/bochs/config.h` or invoke any
Bochs compiler, linker, executable, configure script, or project.

## Repair

`tools/Project-BochsConfig.ps1` now installs a
`System.Collections.ArrayList` for each macro key on first sight and appends
one `PSCustomObject` record with explicit `Add`.  It no longer uses `+=` to
store a singleton record.  Replacement validation reads the retained list's
`Count` and index directly, avoiding PowerShell conditional-expression
unrolling of a singleton collection.  Empty `#define NAME` values are recorded
as the empty string, not passed to `Trim()` as null.

`tests/bx-ntvdm-adapter/Test-BochsConfigProjection.ps1` uses only a unique
temporary synthetic header/manifest.  It verifies one `BX_CPU_LEVEL` source
definition yields one reported change, a duplicated source definition fails
before its output root exists, an empty `BX_EMPTY` define survives, deterministic
independent synthetic outputs have equal hashes, and missing/duplicate/
unexpected manifest replacement cases fail closed.

## Verification

The focused synthetic test command was:

```text
powershell -ExecutionPolicy Bypass -File tests\bx-ntvdm-adapter\Test-BochsConfigProjection.ps1
```

It passed with `Bochs config projection completeness and fail-closed
verification passed.`  Its two temporary outputs had the identical SHA-256
`28D1E1A76278AB088C931CF8450711E2210F47599D6BA78EBBB6F1802894844C` and were
removed by the test.  No `artifacts/analysis/t95-s6-msvc-x86-config-projection-*`
root was created.

## Follow-up and non-claims

The parser/tool repair does not validate the pinned manifest's selected
values, create a projected Bochs header, or prove syntax/link/runtime closure.
The next gate is coordinator review, followed only by separately admitted one
actual fresh-root projection invocation; a `/Zs` syntax observation remains a
later, separate admission.
