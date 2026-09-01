# M0 T355 S24 — session-disposal predicate observation

## Recovery choice

`session_dispose` is project-owned lifecycle code, not an OpenNT mirror body.
Its existing rejection contract has exactly three preconditions: invalid
session, nonzero thread-binding count, and an armed termination escape.  The
smallest useful recovery rung is therefore a fixed-width optional explanation
on the existing operation, rather than changing original SoftPC worker or
shutdown behavior.

`session_dispose_with_reason` evaluates the same predicates in the same order
before any teardown.  `session_dispose` remains its exact no-report wrapper.
On failure, `app/entry.c` preserves return `72` and writes only the reason
name/code when `MVDM_SESSION_DISPOSE_REPORT_PATH` is present.  It records no
guest address, host handle, pointer or MVDM value.

## Formal build

The selected formal CPU40/x86 command was:

```text
build\M0-T355\S20\formal-x86\run-ninja-parallel.cmd original-softpc-process.exe
```

It completed all 39 actions and linked
`original-softpc-process.exe`.  Historical warnings remained visible; none
were suppressed by this S.

## Single fixed-container observation

The one admitted observation invoked `ObserveSoftpcStartup.mjs` with an
`--environment` argument intended to set
`MVDM_SESSION_DISPOSE_REPORT_PATH=O:\ntvdm\m0-t355-s24-dispose.txt`.

The observer's durable JSON proves that its current command contract ignores
unknown arguments: its child command has only the launcher, product, stage and
result file.  Consequently no disposal report existed.  The unchanged
container still exited `0x00000048` after 8 seconds; its product SHA-256 was
`8569580d94a5ec3388976bf39f743beb34350c3cabcbba95dc2b5316d6e0dbd3`.

This is a **report-unavailable** result, not evidence that no disposal
predicate was reached.  S24 does not retry the container.  A successor must
first add a narrow, explicit child-environment forwarding contract to the
observation tool, then perform one newly admitted fixed-container observation.
