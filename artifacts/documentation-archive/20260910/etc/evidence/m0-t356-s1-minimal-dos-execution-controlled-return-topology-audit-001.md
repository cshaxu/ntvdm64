# M0 T356 S1 — Minimal DOS execution and controlled-return topology audit

## Scope and method

This is a static source/ABI/failure audit for the admitted CPU40/x86 package.
It does not execute the product, modify guest media, infer a repair from a
watchdog, or claim that a DOS program has run.  Inputs were the selected
repository mirrors and the current formal composition sources.  The audit
follows the original DOS command path rather than treating an observed
Base-VDM wait as a guest-execution result.

## Source-defined command and return topology

1. `app` accepts an explicit `--command` declaration and publishes one copied
   Base VDM DOS record.  The current record carries `COMMAND.COM` as the
   application, `/C <text>\r\n` as command text, a `COMSPEC=` multi-string and
   the admitted current directory.  Its local broker is deliberately a
   source-shaped one-session replacement for the NT4 CSR transport; it owns
   no guest execution semantics.
2. `mvdm-host/dos/command/cmdmisc.c:cmdGetNextCmd` is the original consumer.
   It requests `ASKING_FOR_DOS_BINARY`, preserves the environment-capacity
   retry, copies the returned `AppName`, command tail and `CMDINFO` result to
   guest locations, and reports `CF=0` only after that transfer.  Its current
   bounded guest-address and redirection identities are existing registered
   adapter boundaries; they do not create a replacement command protocol.
3. Guest `dos/v86/cmd/command/tcode.asm:557` makes
   `CMDSVC SVC_CMDGETNEXTCMD`; on clear carry it enters `run_cmd` at line 623.
   The matching guest `spc.asm:263-284` states that this `CMDINFO` layout must
   match host `cmdsvc.h`.  This is the source-defined host-to-guest handoff.
4. The same guest COMMAND source transfers the received fully qualified
   `EXECPATH` and extension through its original external-command logic:
   `misc1.asm:390-426` explicitly documents that a Base VDM command supplies
   the fully qualified application name and type, so COMMAND does not repeat
   PATH search for that first command.  It proceeds through the ordinary DOS
   `INT 21h` execution path, not `SVC_CMDEXEC`.
5. Guest NTDOS implements that ordinary execution path.  In
   `doskrnl/dos/msproc.asm`, `$Exec` allocates/initializes the program state,
   creates the executable entry and stack, then at lines 1366-1389 calls
   `SVC_DEMENTRYDOSAPP` before the far transfer to the user program.  That
   source is the owner of PSP, arena/MCB, JFN/SFT, environment and parent
   state; no host child-process implementation may replace it.
6. On command completion, guest COMMAND uses
   `SVC_RETURNEXITCODE` (`tcode.asm:1213-1227` and `1370-1382`).  The original
   host `cmdReturnExitCode` (`mvdm-host/dos/command/cmdexec.c:695-749`) updates
   current directories, completes redirection, calls `GetNextVDMCommand`,
   returns `CF=1` only for a new DOS command/re-entry, and otherwise returns
   clear carry with the retained exit code and resumes the blocked event path.
7. Separately, the initial NTDOS image transition is original guest
   `doskrnl/bios/msinit.asm:247-252` → `SVC_DEMLOADDOS` → original host
   `demmisc.c:demLoadDos` (`162-210`).  That source opens `NTDOS.SYS` from the
   selected DOS-media root and writes it to the guest address selected by the
   existing session-backed location boundary.  It is an initial load edge,
   not a substitute for later `$Exec`.
8. The original CPU40 execution body remains the only executor:
   `adapter-mvdm-host-out/softpc/mvdm_softpc_execution.c` binds a session,
   calls original `host_start_cpu()`, observes its outer return, and closes
   the original host cohort.  It neither recognizes COMMAND nor invents a
   guest return result.

## Required separation: DOS execution versus host-child execution

`SVC_CMDEXEC`/`cmdExec` (`cmdexec.c:618-680`) is explicitly documented by the
original source as execution of a **non-DOS binary**.  Its `cmdExec32` worker,
public-Win32 child lifecycle and later `SVC_RETURNEXITCODE` coordination are
not the `INT 21h`/NTDOS `$Exec` path selected for this task.  A successful
Base VDM record, or a host-child result, therefore cannot demonstrate a DOS
`.COM`/minimal `.EXE` execution or its PSP parent return.

## Immediate edge disposition

| Edge | Original owner | Current disposition | Failure/limitation |
| --- | --- | --- | --- |
| declared record → Base VDM request | BaseClient/BaseSrv VDM | existing source-shaped local broker | no CSRSS, separate WOW, PIF or cross-process broker |
| Base VDM request → guest `CMDINFO` | `cmdmisc.c` + COMMAND | original mirror with registered bounded guest-location/redirection bindings | failures retain carry/error/termination branches |
| first command → ordinary DOS execute | guest COMMAND `tcode.asm`/`misc1.asm` | direct guest source/product path selected; reachability not yet observed | requires an admitted local DOS executable and compatible media root |
| DOS execute → child/PSP/arena/JFN/environment | NTDOS `$Exec` | direct guest source owner | no current positive observation; do not replace with `cmdExec32` |
| guest program entry | NTDOS `$Exec` → `SVC_DEMENTRYDOSAPP` | original guest source and existing DEM dispatch route | debugger/VDD notification sub-branches remain intentionally unavailable |
| child return → COMMAND/Base VDM completion | guest COMMAND + `cmdReturnExitCode` | original source with existing Base VDM/session event bindings | no observed controlled normal return yet |
| CPU continuation | SoftPC CPU40 `host_start_cpu` | original source with bounded session composition | nested CCPU returns are not completion; no alternate executor |

## S1 decision

The earliest complete **source-owned recovery cohort** is the already
source-defined vertical guest cohort:

`Base VDM DOS record → cmdGetNextCmd → guest COMMAND external DOS handoff →
NTDOS $Exec → guest parent/COMMAND return → cmdReturnExitCode`.

S2 must recover only any missing binding on this entire cohort after selecting
one immutable local DOS `.COM` or minimal `.EXE` asset and proving its original
media/command representation.  It must not repair the prior timeout by
altering `base_vdm_local`, make `SVC_CMDEXEC` a DOS-exec substitute, fabricate a
result, or change guest binaries.  If source review shows that a reached edge
requires an unavailable historical product shell, S2 must stop at that exact
edge and record the source-shaped terminal instead.

## Verification performed

The audit used repository-wide source searches and direct reads of the named
original mirrors, current local broker, and CPU40 composition.  No build or
runtime command was run because S1 explicitly excludes both.  The governance
admission was separately verified before this evidence was produced.

## Physical source and selected-build check

The following guest sources exactly match the selected `OpenNT` baseline by
SHA-256, preserving the source-defined COMMAND and NTDOS execution flow:

- `mvdm-guest/dos/v86/cmd/command/tcode.asm` —
  `223938eef6bc48314805cb3c35ebbd11cd6da10316ed09952c8f06cf9ca11377`.
- `mvdm-guest/dos/v86/doskrnl/dos/msproc.asm` —
  `8ef142f6940ebd00bb88a2f9fbfe73ff29842d630e96282101390e19c752c492`.

The host mirrors are deliberately non-identical only at their registered
composition seams: `cmdmisc.c` (guest leases/redirection), `cmdexec.c`
(redirection/session event bindings), and `demmisc.c` (native handle sentinel
and private loader cursor).  Their source identity remains
`OpenNT/base/mvdm/dos/{command,dem}` and their current `DIVERGENCE` annotations
make those differences reviewable.

The existing fresh formal CPU40/x86 graph at
`build/M0-T355/S36/formal-x86/build.ninja` selects the audited production
translation units: `demmisc.c`, `cmdexec.c`, `cmdmisc.c`, `base_vdm_local.c`
and `mvdm_softpc_execution.c`.  This establishes current compile-graph
selection only; S1 does not rely on it as execution evidence.
