# T228 S1 budget-terminal position local result

## Scope

This result validates only the default-off, selector-blind mantle observation
admitted in `t228-s1-budget-terminal-position-diagnostic-admission-001.md`.
It does not claim native Guest DOS continuity, BOP provider completion, or a
repair selected from a trace.

## Formal build

On 2026-08-18, the manifest-generated MSVC x64 `/MT` Ninja graph at
`build/M0-T228-S1/terminal-position-20260818a` completed the
`bin/ntdos64-native.exe` target. The graph rebuilt the declared `bx-core`,
`bx-mantle`, `bx-vdm`, `opennt-host`, and `cli` static archives and linked the
native CLI successfully.

## Focused mantle fixture

`t228-s1-budget-terminal-position-fixture.exe` passed with exit code 0. It
proves all admitted local cases:

1. a disabled observation produces no copied position after a watchdog budget
   terminal;
2. an enabled observation copies the expected fixed-width real-mode CS:IP
   record after that budget terminal;
3. a test-owned controlled generic #UD stop produces no budget-terminal
   record; and
4. disable/reset clears the record.

The fixture's #UD bridge is local test plumbing only. It neither recognizes a
BOP selector nor invokes an OpenNT provider.

## CLI contract

Against the already hash-locked T228 input root, `--validate-only` accepted
`--observe-budget-terminal-position` in both declared supported modes:

- Direct: exit code 0, `observe-budget-terminal-position=1`;
- Readonly: exit code 0, `observe-budget-terminal-position=1`.

No normal run or guest trace was executed for this result. The option is
absent by default, copies no guest bytes, and exposes only a fixed-width
post-run CS:IP observation. Any subsequent guest continuity attribution
remains a separate comprehensive integration/repair task.