# T199 S43: COMMAND full-family owner and disposition ledger

## Question and inputs

Can every callable OpenNT COMMAND BOP identity in `cmddisp.c`'s ordered
seventeen-entry table be assigned one contained adapter outcome, with no
accidental raw pass-through or ambient NT host dependency?

The primary source is
`src/opennt/base/mvdm/dos/command/cmddisp.c`; original implementation paths
are `cmdexit.c`, `cmdmisc.c`, `cmdredir.c`, `cmdexec.c`, `cmdconf.c`,
`cmdkeyb.c`, and `cmdenv.c`.  The S42 CMDINFO map supplies the detailed
`54:01` ABI.  Current code was audited through ingress, provider registry,
COMMAND plane/facade and bound command session.

## Complete callable ledger

| BOP | Original owner | Current bx-vdm disposition | Fixed outcome / boundary |
| --- | --- | --- | --- |
| `54:00` | `cmdExitVDM` | contained CLI terminal | typed controlled stop; no host exit-code transport claim |
| `54:01` | `cmdGetNextCmd` | source-derived CMDINFO provider | checked 36-byte gather, checked outputs, `54:0F` CF/AX retry |
| `54:02` | `cmdComSpec` | contained bootstrap | checked COMSPEC gather and copied environment state |
| `54:03` | `cmdSaveWorld` | original no-op | exact four-byte resume |
| `54:04` | `cmdGetCurrentDir` | immutable-root capability | checked `X:\` write or CF failure; no host CWD |
| `54:05` | `cmdSetInfo` | copied launch registration | typed locators only; no guest-pointer retention |
| `54:06` | `cmdGetStdHandle` | explicitly unavailable | typed controlled stop; no handles/pipes |
| `54:07` | `cmdCheckBinary` | declared DOS-only CLI profile | four-byte resume, CF clear; no host classification |
| `54:08` | `cmdExec` | explicitly unavailable | typed controlled stop; no host process |
| `54:09` | `cmdInitConsole` | contained no-install console disposition | four-byte resume; no VDD/GUI/device activation |
| `54:0A` | `cmdExecComspec32` | explicitly unavailable | typed controlled stop; no host process |
| `54:0B` | `cmdReturnExitCode` | contained no-queued-command lifecycle | four-byte resume, CF clear and zero low AL result |
| `54:0C` | `cmdGetConfigSys` | immutable boot-file provider | checked guest path write; no temporary host file |
| `54:0D` | `cmdGetAutoexecBat` | immutable boot-file provider | checked guest path write; mantle write refusal stays a typed boundary refusal |
| `54:0E` | `cmdGetKbdLayout` | keyboard-not-installed disposition | four-byte resume with DX zero; no KEYB/VDD host integration |
| `54:0F` | `cmdGetInitEnvironment` | contained initial-environment provider | ES:0 checked write and BX paragraph retry |
| `54:10` | `cmdGetStartInfo` | contained no-session disposition | four-byte resume with low AL zero; no ambient console inference |

`54:11` is the `SVC_CMDLASTSVC` sentinel, not a callable identity.  Common
ingress/plane classification rejects it before a COMMAND provider is selected.

## Procedure and observations

The fresh current-source witness
`artifacts/build/t199-s43-command-family-r1/` compiled and linked the complete
boot-namespace composition under MSVC x64 `/MT`, then ran its full in-process
fixture successfully.  It exercises all 17 facade classifications, the
original no-op, all three explicit-unavailable stops, lifecycle/profile
responses, COMSPEC/environment sequence, CMDINFO paths, registration,
current directory, CONFIG/AUTOEXEC materialization, keyboard response and
mechanical-action refusal boundaries.

S42's focused witnesses remain the field-level proof for CMDINFO and its
environment retry.  In particular, the deliberately disabled action case for
`54:0D` is a negative check on the mantle write seam; it is not evidence that
the service is unowned.

## Interpretation and limits

Every original callable slot now has one owner and executable package
disposition in the bound composition.  “Unavailable” and contained profile
responses are intentionally not claims that historical Win32 process,
redirection, VDD, GUI, KEYB or console capabilities have been recovered.
This completes COMMAND's static/family-regression package work only.  It does
not explain or close S41's native startup frontier, and it does not authorize
a new native trace.
