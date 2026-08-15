# T217 S8 — COMMAND Initial-Environment Semantic Repair

## Scope

Repair only the existing source-derived provider for OpenNT COMMAND service
`54:15` / `cmdGetInitEnvironment`. This is not an AUTOEXEC merge, DOS EXEC,
or COMMAND execution claim.

## Original Owner And Contract

The owner is OpenNT COMMAND, `src/opennt/base/mvdm/dos/command/cmdenv.c`,
`cmdGetInitEnvironment` (the initial-environment loop and its `BX` paragraph
retry). The related fixed guest COMSPEC storage is
`src/opennt/base/mvdm/dos/command/cmddata.c:12`; its value is populated by
`cmdmisc.c:643-646`.

The loop excludes entries starting with `=`, then uses one `fFoundComSpec`
state bit with `_strnicmp(..., comspec, 8)`, where `comspec` is `COMSPEC=`.
It separately uses one `fFoundWindir` bit with `_strnicmp(..., windir, 6)`.
Thus it excludes only the first matching entry of each class, retains later
entries, and does not require an equals sign on other nonempty entries. After
that selection it converts ANSI to OEM and uppercases only the substring
before an equals sign. It dynamically grows the intermediate data and reports
the actual guest buffer requirement in paragraphs; it contains no 4,023-byte
policy.

## Recovery Ladder

| Rung | Disposition | Evidence |
| --- | --- | --- |
| Original translation unit | Not composable | `cmdenv.c` requires historical COMMAND globals, CCPU register access and `GetVDMAddr`; these are not a modern x64 CLI link surface. |
| Smallest seam | Retained and repaired | `src/bx-vdm/bx_ntvdm_command_initial_environment_v1.c` retains the source transform, uses a copied ANSI host snapshot and the existing typed guest publication path. |
| External intrusion | Rejected | No bx-core or bx-mantle behavior is involved. |
| New behavior | Rejected | The source defines the transform and failure/size contract. |

## Repair

The seam previously used exact-name filtering after OEM conversion and its
copied host-context validator rejected malformed and duplicate retained
entries. It now selects before conversion with OpenNT's `COMSPEC=` and
`WINDIR` first-prefix state; it permits all source-retained nonempty entries
through the copied ABI. The fixed maximum remains 65,535 bytes including the
original 72-byte maximum `COMSPEC=` record plus terminator: this is the
existing one checked 16-bit publication transaction, not a host-environment
policy. Oversize input fails at admission without truncation.

## Verification

On 2026-08-15, a fresh build used:

```text
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T217S2CommandInitialEnvironmentProbe.ps1 \
  -RepositoryRoot O:\repos.hobby\ntdos64 \
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t217-s8-command-environment-source-first-r2
```

The MSVC x64 `/MT` executable compiled and printed:

```text
bx-ntvdm COMMAND initial environment: OpenNT ANSI/OEM filter contract verified
```

The focused fixture covers ANSI-to-OEM/name-case/PROMPT/first host entries and
now installs a process-local 5,000-byte value before capture.  It verifies
that the actual `GetEnvironmentStringsA` path retains that value and produces
an OEM environment above the former 4,023-byte adapter ceiling.  Its
source-permitted duplicate-plus-malformed block verifies that the copied ABI
does not erase entries that the OpenNT loop can retain. It cannot make the
ordinary Win32 process environment contain duplicate variables, so that part
is an ABI acceptance test rather than a live-host duplicate test.

The live-capture extension was rebuilt and run on 2026-08-15 with:

```text
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T217S2CommandInitialEnvironmentProbe.ps1 \
  -RepositoryRoot O:\repos.hobby\ntdos64 \
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t217-s8-command-environment-live-over4023-r1
```

It exited `0` and wrote `run.log` plus a passed x64 `/MT` JSON record under
that build root.

## Deliberate Limits

`cmdCreateVDMEnvironment` and its AUTOEXEC merge are a separate OpenNT path.
The later DOS EXEC double-NUL scan has its own 32 KiB guest constraint. Neither
is changed or closed here. The paused SYSINIT command-name realization map
remains the next evidence subject after this repair.
