# T211 S4 — COMMAND console/keyboard source, ABI and provider map

## Question

How can the original COMMAND `54:09` console initialization and `54:0E`
keyboard-layout component be represented in the CLI profile without making
VDD/event-thread state, a keyboard driver installation, or an ambient host
configuration an implicit requirement?

## Inputs

- OpenNT `src/opennt/base/mvdm/dos/command/cmddisp.c`: the dispatcher assigns
  `cmdInitConsole` and `cmdGetKbdLayout` to contiguous COMMAND service slots
  `09` and `0E`.
- OpenNT `cmdmisc.c:718-736`: `cmdInitConsole` turns on console output and
  calls the historical VDD event-thread initializer.
- OpenNT `cmdkeyb.c:56-249`: `cmdGetKbdLayout` reads the console layout,
  read-only registry mappings and system `KB16.COM` / `KEYBOARD.SYS` assets.
  Its `NoInstallkb16` path sets `DX=0` and invokes `cmdInitConsole`.
- T211 S1 COMMAND owner map and T200/S31 disposition ledger.

## Provider decision

`bx_ntvdm_command_console_keyboard_provider_v1` is the single bx-vdm
COMMAND-component owner for both services.  It embeds the existing bounded
console state as private provider state:

| Service | Current profile outcome | Historical correspondence |
| --- | --- | --- |
| `54:09` | resume after recording initialized console capability | `cmdInitConsole`, without VDD/event-thread composition |
| `54:0E` | resume, `DX=0`, then record initialized console capability | `NoInstallkb16` path in `cmdGetKbdLayout` |

This is deliberately not a claim of interactive console, keyboard-layout,
KB16, registry, or VDD support.  Public console and registry APIs exist in a
normal modern process; no missing Win32 API blocks the component.  The
historical VDD/event-thread composition and the KB16 assets are not admitted
to this CLI profile.  The provider therefore takes the original no-install
path, performs no host I/O, registry access, driver install, system mutation,
or guest-memory write.

## Migration

- `bx_ntvdm_command_package_session_v1` now owns the versioned provider and
  routes both members through it.
- The former console helper is narrowed to `54:09` state mechanics; the
  keyboard helper is called only inside the new provider.
- The detached legacy adapter-runtime `54:0E` dispatch was removed.  It may
  no longer choose a COMMAND keyboard result outside the package session.
- `54:10` remains the bootstrap/lifecycle deferred service from T211 S2; it
  is neither a console nor keyboard member.

## Verification

1. `Invoke-T198S16BootNamespaceVdmX64Probe.ps1` with fixture
   `command-console-keyboard`, build root
   `artifacts/build/t211-s4-console-keyboard-r1`, source-built and ran under
   MSVC x64 `/MT` (`passed: true`).  It checks the two member identities,
   exact resume, `DX=0`, no acceptance of `54:10`, and protected-mode refusal.
2. The same probe with `boot-namespace`, build root
   `artifacts/build/t211-s4-console-keyboard-r2`, source-built and ran under
   MSVC x64 `/MT` (`passed: true`).  The integrated generic bridge reaches
   `54:09` and `54:0E` through the COMMAND package session.

## Interpretation and follow-up

The component’s current no-install profile behavior is source-shaped and has
one owner.  It does not close T211 S4: the final packet still requires a
complete member/failure ledger review, broad regression and the requested
governance handoff.  Native tracing remains deferred until after package
closure and is not an implementation selector.
