# Command Service First-Cut Admission

## Decision

The initial command path keeps the original `CmdDispatch` table in
`cmddisp.c:15-32`. A modern shim may replace only obsolete host internals of
the original named service functions. It may not create a parallel command
selector decoder, write a DOS command into registers, launch a native process,
or bypass NTDOS's normal open/EXEC flow.

The profile is one noninteractive `COMMAND.COM /C <owned DOS command>` session.
It has one immutable contained namespace, current directory, environment,
configuration policy, AUTOEXEC policy, command record, terminal capability,
and completion policy.

## Required Service Matrix

| Selector | Guest ABI | First-cut result | Modern owner |
| --- | --- | --- | --- |
| `CMDSETINFO` (5) | `DS:DX` SCSINFO; `DS:BX` DOS-binary byte; `DS:CX` FDACCESS word | validate/copy bindings for the session; no host global pointer | command bridge state |
| `GETCONFIGSYS` (12) | `DS:DX`, 64-byte receiver | atomically write declared contained empty CONFIG pathname | configuration policy |
| `GETINITENVIRONMENT` (15) | `ES:0000`, `BX` paragraph capacity | retry-size protocol then copy immutable OEM environment | profile/memory transaction |
| `CMDCOMSPEC` (2) | `DS:DX` NUL path | validate equals selected guest COMSPEC and record session metadata; set `AL` terminal status | command bridge state |
| `GETAUTOEXECBAT` (13) | `DS:DX`, 64-byte receiver | atomically write declared contained empty AUTOEXEC pathname | AUTOEXEC policy |
| `CMDGETNEXTCMD` (1) | `DS:DX` packed `CMDINFO` | validate every embedded guest range, publish one owned DOS command, set DOS binary result and `CF=0` | command profile bridge |
| `CMDGETCURDIR` (4) | `DS:SI` receiver, `AL` drive | return declared current directory for mounted drive; documented carry error otherwise | contained namespace |
| `CMDINITCONSOLE` (9) | none | mark a session terminal capability ready; no historic event thread/window/title mutation | runner terminal policy |
| `GETKBDLAYOUT` (14) | `DS:SI` keyb path, `DX` status | return `DX=0`: no host-requested KEYB.COM installation | fixed EN-US keyboard policy |
| `RETURNEXITCODE` (11) | `DX` exit, `AL` drive, `BX:CX` RdrInfo | capture final DOS exit; never request another command; `CF=0` | runner lifecycle |
| `CMDEXITVDM` (0) | none | bounded runner termination | runner lifecycle |

## Required Source Corrections

`msinit.asm:346-352` invokes `CMDSETINFO` before the command protocol. The
original `cmdSetInfo` stores direct `GetVDMAddr` pointers, including the
`SCS_ToSync` location which `cmdGetNextCmd` later writes. The modern bridge
must instead retain validated opaque guest-address bindings and commit its
single-byte SCS synchronization update through a checked memory operation.

`tcode.asm:879` reaches `CMDINITCONSOLE`. `cmdInitConsole` historically starts
an NT event thread, but that thread is not part of the command ABI. The
first-cut service acknowledges the session terminal policy only; output and
input remain separately owned stream capabilities and no host console state is
created or changed.

`tcode.asm:734-738` consults `GETKBDLAYOUT` and uses nonzero `DX` to choose a
KEYB.COM installation path. The fixed EN-US profile must explicitly return
zero. That defers keyboard layout installation without claiming that the DOS
program can never use keyboard input later.

`macro.asm:394` and `418` show that NTDOS can call `CMDGETCURDIR` for current
directory synchronization. This makes the service part of the contained
namespace baseline, rather than an optional COMMAND convenience.

## CMDINFO Commit Rules

`CMDINFO` is a packed 36-byte record (`cmdsvc.h:96-121`). `CMDGETNEXTCMD`
must first validate the record itself and all currently used embedded spans:

* `EnvSeg:0000`, `EnvSize` for a retryable environment update;
* `CmdLineSeg:CmdLineOff`, sufficient for the command UCOMBUF and CR/LF/NUL
  terminator expected by COMMAND;
* `ExecPathSeg:ExecPathOff`, `ExecPathSize` for the fully qualified owned DOS
  program path;
* the previously registered SCS-to-sync byte from `CMDSETINFO`.

Only after every span is valid may it atomically write the command line,
metadata (`CurDrive`, `NumDrives`, `CodePage`, `ExecExtType`, and batch state),
and SCS synchronization byte. Capacity retry or any validation failure leaves
all command text and metadata unchanged. The initial profile sets no raw
`pRdrInfo`, reports no redirection handles, no batch reentry, no PIF, and no
shell-out.

## Deferred Selectors

`GETSTDHANDLE` (6) remains forbidden until a specific DOS redirection caller
requires a token-based stream bridge. `CMDCHECKBINARY` (7), `CMDEXEC` (8), and
`EXECCOMSPEC32` (10) remain native-process handoff work and must fail closed in
the first profile. `CMDSAVEWORLD` (3) is not a runner checkpoint facility.
`GETSTARTINFO` (16) remains a TSR-only policy response, deferred until a trace
actually reaches its branch.

## Completion

`cmdReturnExitCode` historically uses `GetNextVDMCommand` to decide shell-out
reentry. The first profile replaces that obsolete process/session negotiation
with a one-shot disposition: record `DX` as the DOS exit code, reject/ignore
`RdrInfo` because redirection is not admitted, clear carry, and return to the
original COMMAND flow. `CMDEXITVDM` requests runner stop. Cancellation or a
fault is independently reported by the runner and cannot be converted into a
successful DOS exit.

## Outcome

M32 is complete as the first-command service admission. The next task is to
design the transactional guest-record commit used by `CMDSETINFO`, path
services, environment, and `CMDGETNEXTCMD`; it should reuse M29's no-raw-
pointer rule but needs multi-span all-or-none writes.
