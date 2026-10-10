# M0 T443 S3 — COMMAND COMSPEC Contract Repair

## Decision

Original `cmdExec` owns the interpretation decision.  For its AH=1 route it
has already constructed a complete `COMSPEC /c <tail>` command line before
calling `CreateProcess(NULL, pCommand32)`.  The adapter now prefixes that
whole selected command with `run16.exe` without scanning, tokenising, or
reconstructing the tail.

`run16` identifies an inherited NTVDM execution context before it forms a
native request.  Such a nested request preserves its three worker-local
standard-stream endpoints.  It deliberately does **not** mark them as a
request for NTVWM's hidden Console.  A root text request retains the existing
NTVWM Console selection behavior.

This is the missing half of the COMSPEC route: the injected host CMD executes
the command, but its output must return through NTVDM's existing endpoints so
that the following DOS presentation does not overwrite it.

## Removed project heuristics

- no `|&<>` scan chooses a different launch route;
- no bare `COMMAND.COM /c` spelling is recognised specially;
- no three-argv COMMAND quote rebuild occurs in `run16`.

Host CMD remains the sole parser for built-ins, batch files, quoting, pipes
and redirection.  NTHOOK sees the actual child creates and returns only
legacy children to `run16`/NTSRV.

## Focused build and test evidence

All invocations used the task build roots and background process ownership
required by the execution rules.

| Check | Result |
| --- | --- |
| x64 `run16.exe` rebuild | pass |
| x86 `ntvdm.exe`, `nthook32.dll`, `nthook-install-test.exe` rebuild | pass |
| `run16-image-classification-test.exe` | 424 checks, 0 failures |
| `application-search-test.exe` | pass |
| `native-capture-test.exe` | 3462 checks, 0 failures |
| `nthook-install-test.exe` | 104 assertions, pass |
| release manifest vs all ten deployed images | pass |

The console-startup observer was also tried with both the candidate and the
pre-S3 released package.  In this headless allocated-Console container both
`COMMAND` and `MEM.EXE` return `ERROR_INVALID_PARAMETER` before guest input;
the same result occurs with HEAD and therefore is not evidence for or against
S3.  It is intentionally not counted as an interactive COMMAND acceptance
result.  The owner-visible Windows Terminal `run16 command`, followed by
`ver`, pipe/redirection, and return behavior remains the final acceptance
boundary.

## Published set

The changed release images are `run16.exe`, `ntvdm.exe`, and `nthook32.dll`.
They were copied to `assets/release` and `O:\winnt\system32`; the ten-image
manifest comparison passed before delivery.  No worker protocol, guest binary,
or public ABI changed.
