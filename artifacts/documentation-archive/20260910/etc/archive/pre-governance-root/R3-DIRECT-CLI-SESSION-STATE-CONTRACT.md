# R3 Direct-CLI Session State Contract

Status: source-derived design gate, 2026-08-07. This is not a normal runtime
implementation and does not admit the historical event/UI subsystem.

## Proven Historical Defaults

The original `host/src/nt_reset.c` defines these host-process values before
`config()` uses them:

| Symbol | Original initialization | R3 direct-CLI profile value | Reason |
| --- | --- | --- | --- |
| `VDMForWOW` | `FALSE` | `FALSE` | The selected profile is a DOS VDM, not WOW. |
| `fSeparateWow` | `FALSE` | `FALSE` | Separate WOW is unavailable. |
| `DosSessionId` | `0` | `0` | The profile is not the historical new-console/session-server path. |

The original `host/src/nt_event.c` defines `CntrlHandlerState = 0` and owns
the later console-event policy. It also owns `hWndConsole`, which `config()`
fills by temporarily changing its title and calling `FindWindow`.

## Boundary Decision

The direct-CLI profile may eventually supply the three fixed `nt_reset.c`
inputs and an initially zeroed `CntrlHandlerState` as explicit per-session
context. This is a source-equivalent initial-state decision, not permission to
import `nt_reset.c`'s command-line parser, `ExitVDM`, VDD/WOW process policy,
or the event/UI subsystem.

`hWndConsole` is not an initial input: its normal source owner is the original
post-title lookup in `config()`. A future executing configuration trace must
have a separate title save/restore admission test before it allows that lookup.
The existing title-identity seam returns only a documented PID uniqueness token
and does not itself change a title or inspect a console.

## Explicitly Unsupported PIF Outcomes

The following code paths remain fail-closed until a later contained-terminal
contract exists:

- `SetConsoleKeyShortcuts` and PIF shortcut/reserved-key setup;
- PIF fullscreen/window transitions and fast-paste behavior;
- PIF configuration/autoexec environment expansion;
- interactive `RcMessageBox` error UI;
- any WOW, separate-WOW, reentrant, or subsequent-command state.

The future initial state must be held per research session and cleared on
session end. It must not be a process-global compatibility mode, registry
setting, console-server registration, or input to CCPU/BIOS/DEM/DOS services.

## Next Admission Condition

The generated structural-PIF fixture now supplies these state values to a
default-disabled original `config()` trace and proves its first `sas_term`
stop without an unsupported PIF/UI callback. That is narrow execution
evidence, not BYOB admission: before a real selected `pif-default` file is
used, it must be manifest-validated and separately tested against the same
unsupported branches.

The default parser branch is now independently covered by the original
`nt_pif.c` fixture described in the command-transport contract. The missing
step is profile-specific validation of the actual owner-selected role before
the same state can be admitted to `config()`.
