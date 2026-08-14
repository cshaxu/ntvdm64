# T200 S26: COMMAND initial launch/lifecycle reconciliation

## Question

Does the first source-built engine terminal (`54:01`) expose a missing
COMMAND BOP implementation, or is it the declared CLI plan's source-derived
no-next-command result?

## Original source map

OpenNT `base/mvdm/inc/cmdsvc.h` and
`base/mvdm/dos/command/cmddisp.c` assign the relevant ordered COMMAND
providers as follows:

| Phase | BOP | Original owner and source | Current contained disposition |
| --- | --- | --- | --- |
| SCS/DOSDATA registration | `54:05` | `cmdSetInfo`, `cmdmisc.c` | copied locators only; optional for CMDINFO delivery |
| COMSPEC bootstrap | `54:02` | `cmdComSpec`, `cmdmisc.c` | checked COMSPEC gather and retained initial-environment state |
| Initial environment | `54:0F` | `cmdGetInitEnvironment`, `cmdenv.c` | checked ES:0 write and original-shaped BX capacity retry |
| Next declared command | `54:01` | `cmdGetNextCmd`, `cmdmisc.c`; caller `tcode.asm` | checked 36-byte CMDINFO gather and atomic contained response |
| Normal command loop | repeat `54:01` | `tcode.asm` branches to `run_cmd` on CF clear and later returns to the COMMAND loop | exhausted immutable plan returns typed stop |
| Shell-out return | `54:0B` | `cmdReturnExitCode`, `cmdexec.c` | CF re-entry only when another declared slot exists; otherwise CF clear and low AL from DX |

`tcode.asm` explicitly performs `54:01`, branches to `run_cmd` on CF clear,
and retries only when CF signals an environment-capacity response.  The
original `cmdGetNextCmd` calls `TerminateVDM()` when the historical host has
no next command.  The contained profile has no ambient command queue: its
immutable one-slot plan is the equivalent explicit source of that condition.

`54:05` is not a prerequisite for every `54:01`: the source uses its
registered SCS/DOSDATA locations only for two post-delivery writes.  A
pre-registration CMDINFO request must therefore not generate zero-address
writes; this remains a covered negative case.

## Current state determination

The native engine fixture declares `2,1,e,00`: version two, exactly one EXE
slot, and no tail.  S25's first retained typed stop occurs at `95AB:03C1`
with copied bytes `C4 C4 54 01`.  Earlier package evidence records
`delivered=1`.  The session checks `delivered >= slot_count` before preparing
another CMDINFO gather, so that event is exactly the declared exhausted-plan
branch.

This is not a claim that a historical interactive COMMAND queue has been
recovered.  It is the CLI profile's explicit terminal policy and does not
justify adding a fake command, ambient host queue, or endpoint-specific BOP
patch.

## Verification

1. The whole bound boot-namespace/COMMAND composition was freshly built and
   run under MSVC x64 `/MT`:

   ```powershell
   powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
     -RepositoryRoot O:\repos.hobby\ntdos64 `
     -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s26-command-package-r1 `
     -Fixture boot-namespace
   ```

   It passed.  It covers the full COMMAND facade, COMSPEC/environment
   sequence, registration-present and absent CMDINFO delivery, current
   directory/boot-file/keyboard dispositions, first slot delivery, and an
   exhausted second `54:01` typed stop.

2. The focused lifecycle regression was freshly rebuilt and run:

   ```powershell
   powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
     -RepositoryRoot O:\repos.hobby\ntdos64 `
     -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s26-command-lifecycle-r2 `
     -Fixture command-entry
   ```

   It passed and covers one/two-slot `54:01` delivery, `54:0B` CF re-entry,
   single-slot CF-clear/AL exit return, duplicate return rejection,
   environment retry, and aperture refusal.  The fixture was corrected to
   set DX to the asserted DOS exit code before the single-slot `54:0B` case;
   it had previously retained the CMDINFO DS:DX offset (`0x10`) while
   incorrectly expecting a zero AL result.  Product code is unchanged.

3. One newly compiled source closure and one bounded native engine witness
   passed:

   ```powershell
   powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T198S94SourceBuiltNormalReturnNativeProbe.ps1 `
     -RepositoryRoot O:\repos.hobby\ntdos64 `
     -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s26-current-composition-r1 `
     -CompileOnly

   powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T200S22EngineMachineStageProbe.ps1 `
     -RepositoryRoot O:\repos.hobby\ntdos64 `
     -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s26-engine-native-r1 `
     -CompositionBuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s26-current-composition-r1
   ```

   The run reported `terminal=5 detail=1` and `C4 C4 54 01` at
   `95AB:03C1`, then produced the same terminal with observation disabled.

## Result and limitation

There is no demonstrated COMMAND startup/lifecycle omission in the current
one-slot profile.  The next product boundary is the CLI outer shell's direct
typed handoff to the native engine.  This S neither closes COMMAND as a whole
nor implements an interactive shell, host process, console, GUI, VDD, or
general DOS execution environment.
