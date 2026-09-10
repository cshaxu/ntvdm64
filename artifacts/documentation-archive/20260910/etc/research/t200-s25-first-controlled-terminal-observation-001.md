# T200 S25: first controlled terminal observation

## Question

What is the first accepted typed STOP reached by the source-built bounded
engine lifecycle, and which already-owned OpenNT BOP package gives it its
current disposition?

## Method and boundary

`bx_ntvdm_terminal_observation_v1` is a default-off, fixed-width bx-vdm
record.  It is invoked only after the existing generic bridge has accepted a
typed STOP outcome.  It copies the pre-existing exception event and outcome;
it neither reads guest memory nor decodes a selector or service.  The copied
exception window remains bounded to the existing fifteen bytes.

The source-built composition was compiled with:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T198S94SourceBuiltNormalReturnNativeProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s25-current-composition-r1 `
  -CompileOnly
```

The bounded engine fixture was then run with:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T200S22EngineMachineStageProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s25-terminal-observation-r1 `
  -CompositionBuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s25-current-composition-r1
```

Both use the current MSVC x64 `/MT` source closure and the existing finite
engine watchdog.

## Observation

The fixture passed and printed:

```text
t200-s24 first terminal=5 detail=1
t200-s25 stop cs=95ab eip=000003c1 bytes=15 c4 c4 54 01
t200-s24 second terminal=5 detail=1
```

`5/1` is the engine's controlled-guest-terminal / stage-controlled-stop
pair.  The first accepted stop's copied BOP window is therefore `54:01` at
`95AB:03C1`.  The fixture then disables the record and obtains the same typed
terminal a second time, proving that the observation path is default-off and
does not supply the terminal behavior.

## Owner and disposition map

OpenNT `base/mvdm/dos/command/cmddisp.c` assigns `54:01` to
`cmdGetNextCmd`.  The complete owner/disposition record is retained in
`t199-s43-command-family-owner-disposition-ledger-001.md`; its original
implementation family includes `cmdmisc.c`, `cmdenv.c`, `cmdexec.c`,
`tspc.asm`, `tcode.asm`, and `comequ.asm`.

The current bx-vdm COMMAND session preserves the relevant original direction:
when OpenNT `GetNextVDMCommand` has no command, `cmdGetNextCmd` terminates the
VDM.  The declared CLI plan has no ambient command queue, so after every
declared slot is delivered, the already-routed `54:01` provider returns the
contained typed stop.  This is the same disposition independently evidenced
by T200 S8 and by T200 S9's earlier native observation.

The result is thus an expected, already-owned COMMAND launch/lifecycle
terminal, not an unhandled raw `#UD`, a Bochs selector decision, or a new BOP
implementation admission.

## Limits

This observation does not close COMMAND, `54:01`, process execution, console,
or an interactive command queue.  It introduces no service/provider change,
no Bochs change, no device or firmware change, and no new trace-driven task.
It is retained solely as T200's package-level native integration evidence.
