# Sysinit Command Handoff Dependency Ledger

Status: M22 ordered static dependency record, 2026-08-08.

## Scope

This record follows the fixed NT4 EN-US source route after NTDOS `DosInit`
returns to BIOS/sysinit. It identifies the minimum command/bootstrap seams for
a bounded `COMMAND.COM /C <owned command>` research run. It does not authorize
a replacement DOS service dispatcher, a new PC/AT device model, use of host
system directories, or an implementation of any listed seam.

## Source-Backed Order

The relevant sequence is not a direct jump from NTDOS to a shell. It is:

```text
DosInit returns to sysinit
  -> CMDSVC SVC_GETCONFIGSYS
  -> DOS config processing and DOS-owned allocation/IVT work
  -> optional CONFIG.SYS device=/install=/DOS= branches
  -> DOS INT 21h open/size/EXEC of selected COMMAND.COM
  -> COMMAND.COM permanent initialization
       -> SVC_GETINITENVIRONMENT
       -> SVC_CMDCOMSPEC
       -> SVC_GETAUTOEXECBAT
       -> DOS INT 21h open/read AUTOEXEC input
       -> SVC_CMDGETNEXTCMD for the first host-supplied command
  -> COMMAND.COM executes the selected DOS command
  -> SVC_RETURNEXITCODE or SVC_CMDEXITVDM at the runner boundary
```

Evidence:

- `src/opennt/base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm:1153-1170`
  passes its `config` filename buffer through `CMDSVC SVC_GETCONFIGSYS` before
  calling `doconf`.
- `sysinit1.asm:1349-1400` shows that CONFIG processing has optional
  device-driver and `install=` passes. `sysinit1.asm:1414-1421` keeps the
  HMA/A20 route conditional on the selected `DOS=HIGH` outcome.
- `sysinit1.asm:1517-1620` selects `commnd` and uses normal DOS `INT 21h`
  open, seek, close, and EXEC operations to start the command interpreter.
  `sysinit2.asm:1547` initializes `commnd` as `\\COMMAND.COM`; parsed
  `SHELL=` content is a configuration policy input, not a core default.
- `src/opennt/base/mvdm/dos/v86/cmd/command/rdata.asm:555-572` obtains the
  permanent command environment through `SVC_GETINITENVIRONMENT`.
- `cmd/command/init.asm:1121-1128` announces the guest COMSPEC through
  `SVC_CMDCOMSPEC`; `init.asm:1405-1416` obtains an AUTOEXEC filename through
  `SVC_GETAUTOEXECBAT` and then opens it through DOS `INT 21h`.
- `cmd/command/tcode.asm:557-567` calls `SVC_CMDGETNEXTCMD` with a guest
  command-information buffer. This is the historical first-command injection
  boundary, not an instruction/register shortcut.
- `tcode.asm:1214-1227` and `tcmd2b.asm:465` use respectively
  `SVC_RETURNEXITCODE` and `SVC_CMDEXITVDM` at command completion/termination.

## Historical Host Dependencies To Replace

The original command host performs several operations that a non-invasive
research runner must not inherit:

| Historical code | Historical behavior | Required modern disposition |
| --- | --- | --- |
| `cmdconf.c:60-120` | Expands CONFIG/AUTOEXEC from the Windows directory and writes host temporary files. | Profile supplies a declared config/autoexec policy and the contained namespace exposes only the corresponding synthetic, session-local DOS paths. No system-directory read or host temp file. |
| `cmdmisc.c:25-180` | Calls `GetNextVDMCommand`, tracks session/console state, and may merge ambient state. | A runner-local one-shot command record supplies command, current drive/directory, and declared environment. It is immutable after profile admission. |
| `cmdmisc.c:635-650` | Records the guest COMSPEC in host global state. | Record the validated guest path only as session metadata; it must not read or set host `COMSPEC`. |
| `cmdredir.c:225-315` | Returns raw Win32 standard handles and has pipe-specific conversion behavior. | A later stream adapter owns three explicitly admitted streams. The initial command cut may use inherited streams, but no raw host handle becomes guest-visible without a validated bridge. |
| `cmdexec.c:538-584` | Classifies a non-DOS image and may hand off through historical process/console coordination. | Defer native-image handoff to its own bounded contract. It is not needed to boot COMMAND.COM or to execute an owned DOS command. |

## Minimum Bounded Profile

The first profile must declare, before guest mutation:

1. One immutable contained DOS namespace, including a selected default drive,
   current directory, the validated `\\COMMAND.COM` role, and the owned
   command's files.
2. A configuration policy. The minimal profile may supply an empty/safe
   session-local CONFIG input, but it must still satisfy the historical
   `SVC_GETCONFIGSYS` filename-and-DOS-open sequence. It must reject `DEVICE=`,
   `INSTALL=`, `SHELL=`, and `DOS=HIGH` rather than accidentally acquiring
   keyboard, mouse, EMS, HMA, or arbitrary program dependencies.
3. An AUTOEXEC policy. The initial command cut may expose an absent or empty
   session-local AUTOEXEC input, but it must not consult the host Windows
   directory or user's startup files.
4. A complete, declared initial DOS environment and command record for
   `SVC_GETINITENVIRONMENT` and `SVC_CMDGETNEXTCMD`. Ambient host environment,
   current directory, drive mappings, and `COMSPEC` are excluded.
5. Explicit stdin, stdout, stderr policy plus a bounded completion policy.
   The initial target is one owned DOS command and its exit status, not an
   interactive console, a shell-out session, or host process inheritance.

`COMMAND.COM` itself is consequently a hard first-command artifact; CONFIG
and AUTOEXEC are policy-controlled inputs, not a reason to add PC/AT
controllers. The DOS open/read/seek/EXEC flow does require a contained logical
filesystem and DOS memory/process semantics before a command smoke test can
pass.

## Deferred Paths

The source keeps the following outside the initial command cut:

- SoftPC keyboard and mouse resident-driver installation
  (`sysinit1.asm:1245-1297`, `1355-1375`);
- EMS stub installation (`sysinit1.asm:1299-1346`);
- XMS/A20/HMA behavior, reached only for selected high-DOS/configuration
  outcomes;
- `DEVICE=` and `INSTALL=` execution from configuration;
- redirection pipe emulation, old console event threads, printer handling,
  WOW, registry compatibility checks, and historical shell-out behavior;
- host-native 32/64-bit program dispatch. `SVC_CMDEXEC` is evidence that a
  later handoff boundary exists, not authorization to call `CreateProcess`
  from bootstrap code.

## Boundary Result

The next implementation-relevant research cut is not a disk controller or a
new BIOS interrupt. It is a **profile-owned command startup record** plus a
**contained namespace/stream projection** that preserves the historical guest
call sequence. Core remains unaware of COMMAND.COM, CONFIG.SYS, AUTOEXEC.BAT,
DOS paths, and native process classification. Mantle/profile owns those named
semantics; a future neutral execution boundary need only provide validated
guest-memory access, controlled transition/resume, and capability injection.

The next task should reconcile the fixed guest artifact closure against these
roles, then derive the contained namespace and stream contracts from the
actual DOS open/EXEC and command-service callers.
