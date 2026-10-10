# M0 T443 S3 — COMMAND COMSPEC Contract Repair

## Decision

Original `cmdExec` owns the interpretation decision.  For its AH=1 route it
has already constructed a complete `COMSPEC /c <tail>` command line before
calling `CreateProcess(NULL, pCommand32)`.  The adapter now prefixes that
whole selected command with `run16.exe` without scanning, tokenising, or
reconstructing the tail.

`run16` identifies an inherited NTVDM execution context before it forms a
native request.  Its `CONIN$`/`CONOUT$` endpoint values are worker-local event
identities, however, not inheritable Win32 streams.  The request therefore
retains their Console-role mask: NTVWM replaces only those masked endpoint
values with the real handles of its hidden Console.  An explicitly redirected
file or pipe remains unmasked and is duplicated normally.

This is the missing half of the COMSPEC route: the injected host CMD executes
the command through valid hidden-Console streams, and its output returns via
the existing NTVWM-to-NTCON path without passing NTVDM's event identities to
`CreateProcess`.

## Post-publication S3 regression repair

The first S3 delivery suppressed `console_mask` for a nested execution scope.
That made NTVWM try to duplicate NTVDM's event identity handles into the native
`COMSPEC /c` child. `GetFileType` correctly classifies an event as
`FILE_TYPE_UNKNOWN`, and NTVWM rejected it with `ERROR_INVALID_HANDLE`.
Consequently, a real Windows Terminal `run16 command` reached the DOS prompt
but the first built-in, such as `ver`, hung while the command route waited for
the rejected child.

This was introduced by S3. It is distinct from the allocated-headless
observer's older pre-banner failure: the same observer failure reproduces from
pre-S3 `44c25e138` and is not used as evidence for the interactive regression.
The repair restores the mask for both root and nested scopes, without
restoring any removed command-tail heuristic or changing guest/original
COMMAND code.

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
| x64 `run16.exe` rebuild after mask repair | pass |
| x86 `ntvdm.exe`, `nthook32.dll`, `nthook-install-test.exe` rebuild | pass |
| `run16-image-classification-test.exe` | 424 checks, 0 failures |
| `application-search-test.exe` | pass |
| `native-capture-test.exe` | 3462 checks, 0 failures |
| `nthook-install-test.exe` | 104 assertions, pass |
| isolated `run16 command /c ver` | `MS-DOS Version 5.00.500`, exit code 0 |
| release manifest vs all ten deployed images | pass |

The console-startup observer was also tried with both the candidate and the
pre-S3 released package.  In this headless allocated-Console container both
`COMMAND` and `MEM.EXE` return `ERROR_INVALID_PARAMETER` before guest input;
the same result occurs with HEAD and therefore is not evidence for or against
S3.  It is intentionally not counted as an interactive COMMAND acceptance
result.  The owner-visible Windows Terminal `run16 command`, followed by
`ver`, pipe/redirection, and return behavior remains the final acceptance
boundary.  An automated ConPTY experiment reached its initial CMD prompt but
is not counted: the desktop runner tears down its transient child tree before
the probe can preserve a trustworthy nested-session witness.

## Published set

The post-publication repair changes `run16.exe` only. It was copied to
`assets/release` and `O:\winnt\system32`; the ten-image manifest comparison
passed before delivery. No worker protocol, guest binary, or public ABI
changed.
