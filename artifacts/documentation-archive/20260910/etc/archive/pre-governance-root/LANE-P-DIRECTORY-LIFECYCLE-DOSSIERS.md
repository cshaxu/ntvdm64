# Lane P Directory And Lifecycle Dossiers

Status: M45 design dossiers, 2026-08-08. These define the minimum directory,
console-readiness, and terminal lifecycle contracts surrounding M44. They do
not implement an event loop, a console server, or a replacement command shell.

## Source Evidence

| Fact | Evidence |
| --- | --- |
| NTDOS invokes `CMDGETCURDIR` while synchronizing CDS state. | `dos/v86/doskrnl/dos/macro.asm:394-422` |
| The historical directory handler uses drive discovery and mutable `=X:` process environment state. | `base/mvdm/dos/command/cmdmisc.c:517-578` |
| `CMDINITCONSOLE` only flips a global then starts the historical event thread. | `cmdmisc.c:718-736`; `dos/v86/cmd/command/tcode.asm:879-896` |
| `RETURNEXITCODE` takes a DOS status plus a raw redirection pointer, blocks an event thread, asks `GetNextVDMCommand` for more work, and signals reentry by carry. | `base/mvdm/dos/command/cmdexec.c:590-660`; `tcode.asm:1213-1229` |
| `CMDEXITVDM` has no payload and directly terminates the VDM. | `base/mvdm/dos/command/cmdexit.c:16-31`; `tcmd2b.asm:452-466` |

## LP-05: Current Directory

| Field | Dossier |
| --- | --- |
| Historical source/function | `cmdmisc.c:530-578`, `cmdGetCurrentDir` |
| Guest consumer | NTDOS CDS synchronization in `macro.asm` |
| Entry | `AL` is zero-based drive, `DS:SI` receives directory text |
| Historical policy replaced | physical-drive probing and mutable host `=X:` environment variables |
| Required backend | copied `DS:SI` write, declared contained namespace topology, atomic flags/register result |

The profile owns one canonical current directory for every declared logical
drive. In the initial CLI cut there is exactly one mounted drive and its
directory is an OEM absolute DOS path, `X:\` or a declared contained
subdirectory, at most 66 bytes including NUL. There is no host current
directory, ambient drive query, or environment synchronization.

For a declared drive, the service writes the exact directory plus NUL through
one ordinary-RAM transaction and clears carry. For an undeclared drive it
writes no bytes, sets carry, and sets `AX=1`, preserving the original
invalid-drive result. A path that cannot fit the 67-byte historical receiver
is rejected at profile admission; the historical `AX=0` oversize result is
reserved for a future multi-drive profile that can actually produce it.

Fixtures prove valid root/subdirectory writes, invalid-drive no-write/`AX=1`,
receiver routing failure with no mutation, and no host environment or drive
enumeration access.

## LP-06: Console Readiness

| Field | Dossier |
| --- | --- |
| Historical source/function | `cmdmisc.c:729-736`, `cmdInitConsole` |
| Guest caller | original COMMAND initialization in `tcode.asm:879` |
| Historical policy replaced | global `fConOutput` and historical event-thread creation |
| Required backend | pre-bound session console capability and one idempotent readiness state |

The service is an acknowledgement, not a request to create a Windows console.
The research runner binds its console/video sink before guest execution. On
first invocation, `CMDINITCONSOLE` atomically marks that existing session sink
ready; repeated invocation is idempotent. It changes no guest memory,
register, FLAGS, mode, or machine event. If no sink was bound, it fault/stops
instead of allocating a console, opening a device, creating a thread, or
falling back to a hidden host UI.

This admits visible CLI output only when the future core/mantle display or
stream contract already supplies it. It is not permission to import the
historical event/thread machinery.

## LP-07: One-Shot Completion

| Field | Dossier |
| --- | --- |
| Historical source/function | `cmdexec.c:602-655`, `cmdReturnExitCode` |
| Guest caller | original COMMAND `tcode.asm:1213-1229` |
| Entry | `DX` DOS exit status, `AL` current drive, `BX:CX` legacy redirection pointer |
| Historical policy replaced | raw redirection cleanup, event thread blocking/resume, and `GetNextVDMCommand` reentry decision |
| Required backend | M44 one-shot session state, runner terminal-result callback, constrained FLAGS result |

When the active M44 first-command request returns, this Lane P body accepts it
only while the session state is `one_command_running` and only when `BX:CX` is
zero, as M44 wrote `pRdrInfo=0`. It captures `DX` as a zero-extended 16-bit DOS
exit result, records `AL` only as a declared contained drive observation, and
publishes one terminal event to the runner. The runner may expose that
zero-extended value as the process exit status and a redacted trace fact.

It then transitions the session to `completed`, clears carry, and never calls
`GetNextVDMCommand`, blocks/resumes an event thread, consumes a redirection
pointer, merges directories, or schedules a second command. A second
invocation, nonzero `BX:CX`, invalid lifecycle state, or terminal callback
failure is a fault/stop with no terminal-success publication. This creates a
bounded `COMMAND.COM /C` result rather than a hidden historical command loop.

`CMDEXITVDM` is separately admissible only as a terminal stop notification
with no asserted DOS exit value. Until a trace proves whether the selected
`/C` path reaches LP-07 or `CMDEXITVDM`, a smoke test may not claim an exit
status merely because the session stopped.

## Deferred

Multi-drive updates, directory mutation after program execution, inherited
host directory state, keyboard layout loading, interactive prompts, CTRL-C,
standard stream redirection, batch reentry, 16/32-bit shell-out, TSR behavior,
and Win32 process handoff remain separate work. They have no fallback to the
historical global state or process APIs.
