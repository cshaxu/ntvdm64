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

The formal fresh Ninja graph at `D:\tmp\ntdos64-M0-T235-S1-formal-r3` built
`ntdos64-run.exe` and `runner-engine-probe.exe` from the current sources in
nine MSVC x64 `/MT` actions. Both `tests/ntdos64-run-policy.cmake` and
`tests/ntdos64-run-s1-format-policy.cmake` then passed against those outputs.
The graph generator is `tools/build/New-T235S1RunnerNinjaGraph.ps1`; its
manifest records the isolated runner/probe source set and excludes Bochs,
OpenNT runtime objects, guest media, and artifact builds.

## Closure Checks

The first formal attempt exposed two existing runner-island defects: v3/v4
profiles validated their single `target_placement` but did not project it into
the v2 launch-plan's `declared_targets` input, and the retained policy fixture
required nonempty CONFIG/AUTOEXEC handoff values without supplying the paired
options. The compatibility projection and fixture inputs were corrected; the
formal Ninja run passed. Documentation governance and `git diff --check` pass
at P1 closure.

## Transfer

P2 must introduce the versioned, session-owned copied initial-command record.
P3 and P4 then hand COM/DOS EXE/BAT and PIF respectively to the unchanged
OpenNT COMMAND owner. P5 remains responsible for the earlier PIF-selected
CONFIG/AUTOEXEC lifecycle timing.
