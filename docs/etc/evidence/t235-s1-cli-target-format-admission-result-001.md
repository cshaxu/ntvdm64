# M0 T235 S1 — CLI Target-Format Admission Result 001

## Scope

S1 retains the public runner grammar: after zero or more recognized options,
the first non-option argument is the target path. It adds only a bounded input
classification for `.bat` and `.pif`; it does not transport that input into a
session or implement any COMMAND behavior.

## Ownership And Source Recovery

The CLI has no OpenNT equivalent: it is project-owned modern process input
admission. The original owner of the recovered semantics remains OpenNT
COMMAND:

- `base/mvdm/dos/command/cmdmisc.c:cmdGetNextCmd` receives the initial input;
- `base/mvdm/dos/command/cmdpif.c:cmdCheckForPIF` owns PIF recognition; and
- `base/mvdm/dos/command/nt_pif.c` owns PIF parsing and resolved-target data.

The directly composable original path is therefore reserved for P2--P4. S1 is
the smallest project seam: it recognizes BAT/PIF suffixes before opening the
file, reports that the S2 session-owned handoff is unavailable, and neither
parses their contents nor starts them through `CreateProcessW`. No external
intrusion or new guest/COMMAND algorithm was used.

## Format Matrix

| Input | S1 disposition |
| --- | --- |
| No target or malformed/duplicate option | Deterministic CLI error, exit 2. |
| PE32/PE64 target | Existing direct host launch remains unchanged. |
| `.com` or DOS-MZ `.exe` | Existing explicit BYOB-engine-required outcome. |
| `.bat` or `.pif`, including MZ-looking content | `COMMAND initial input requires the unavailable S2 handoff`, exit 3; no content read. |
| NE | Existing explicit WOW-unavailable outcome, transferred to P8. |

## Focused Result

`tests/ntdos64-run-s1-format-policy.cmake` was run against a fresh disposable
`build/M0-T235-S1/r1/ntdos64-run.exe`. It passed the no-target,
duplicate-option, PE host handoff, DOS COM/EXE, MZ-looking BAT/PIF, and
malformed-drive-list cases. The runner source compiled with MinGW only as an
out-of-process CLI check; it emitted the pre-existing unused
`json_skip_value` warning in `byob_profile.c`.

## Remaining Gate

The repository CMake graph could not generate its formal runner target: it
still declares missing historical v1 source files such as
`src/bx-vdm/bx_ntvdm_readonly_namespace.c`. This is outside S1's CLI boundary
and remains an unclosed formal-build gate. S1 stays active until the affected
formal target, documentation governance, and diff checks can pass; this
result is not an acceptance or package closure.

## Transfer

P2 must introduce the versioned, session-owned copied initial-command record.
P3 and P4 then hand COM/DOS EXE/BAT and PIF respectively to the unchanged
OpenNT COMMAND owner. P5 remains responsible for the earlier PIF-selected
CONFIG/AUTOEXEC lifecycle timing.
