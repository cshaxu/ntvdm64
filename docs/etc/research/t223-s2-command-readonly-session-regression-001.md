# T223 S2 COMMAND Readonly Session Regression

## Question

Can the selected original COMMAND bootstrap/environment/config component
(`54:01/02/04/05/0C/0D/0F`) traverse one bound Readonly session without a
Direct-profile fallback?

## Inputs

- OpenNT owner/reference paths: `base/mvdm/dos/command/cmddisp.c`,
  `cmdmisc.c`, `cmdenv.c`, `cmdconf.c`, and `inc/cmdsvc.h`.
- Adapter package: `src/bx-vdm/bx_ntvdm_command_package_session_v1.c` and
  its bootstrap, command-context, boot-input, and checked-action helpers.
- New focused fixture:
  `tests/bx-vdm/bx_ntvdm_command_bootstrap_readonly_session_v1_test.c`.

## Procedure

1. Assemble a fresh `bx_ntvdm_boot_namespace_composition_v1` using an
   admitted Readonly mutation profile.  The profile registers COMMAND session
   context and host-global classes generically; it does not name a selector or
   service.
2. Assert both classes resolve to `REJECT_READONLY` before BOP ingress.
3. Route one normal COMMAND session through `54:02`, `54:0F` (insufficient
   paragraphs then success), `54:05`, `54:01`, `54:04` (valid and unavailable
   drive), `54:0C`, and `54:0D`.
4. Build and run with the declared MSVC x64 `/MT` closure:

   ```powershell
   powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
     -RepositoryRoot O:\repos.hobby\ntdos64 `
     -BuildRoot O:\repos.hobby\ntdos64\build\M0-T223-S2\008-command-bootstrap-readonly-session `
     -Fixture command-bootstrap-readonly-session
   ```

5. Run the existing original-owner bootstrap provider fixture in a separate
   disposable root (`009-command-bootstrap`) with the same toolchain.

## Observations

- The Readonly fixture linked and ran with `linkExitCode=0` and
  `runExitCode=0`.
- It proved `54:02` increments AX after COMSPEC capture; `54:0F` reports the
  three-paragraph requirement before publishing the environment; `54:05`
  establishes the copied SCS/DOSDATA registration; and `54:01` writes the
  declared target response through checked guest memory.
- `54:04` returns the copied session directory for C: and preserves the
  source-shaped unavailable-drive error (`CF=1`, `AX=1`) for D:.
- `54:0C` and `54:0D` return declared `C:\CONFIG.SYS` and
  `C:\AUTOEXEC.BAT` paths.  No fixture step creates, modifies, or probes a
  host boot file.
- The complementary `command-bootstrap` fixture also linked and ran with
  `linkExitCode=0`, `runExitCode=0`.

## Interpretation And Confidence

High confidence for the bounded package contract: all seven admitted services
use the one COMMAND composition session under Readonly, and the profile is
resolved before routing rather than silently selecting Direct host behavior.
This is not a claim that COMMAND streams, console/keyboard, lifecycle,
session-broker, or native execution are complete; those remain separate
original-owner packages in T223's non-goals.

## Follow-up

Run the governance gates, retain only this concise record, and then assess
whether the Direct and Readonly evidence together satisfy the active S2 exit
criteria.  Do not use a subsequent trace hit to open a singleton service task.
