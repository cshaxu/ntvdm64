# M0 T388 S2 — Interactive initial DOS-record binding

## Question

Can direct no-argument launch bind the original interactive first COMMAND
record without changing the T387 positional `/C` route, guest bytes, or the
Base VDM protocol?

## Implementation

- `app_launch_declaration_publish` now publishes its existing DOS record for
  both launch forms.
  - declared command: `AppName = ...\\COMMAND.COM`,
    `CmdLine = /C <declared command>\r\n\0`;
  - no declared command: the same `AppName`,
    `CmdLine = \r\n\0`.
- `app_launch_declaration_bind` leaves
  `terminal_on_command_exhaustion` clear for the latter.  Thus the existing
  Base VDM local adapter retains its original-shaped waiting state after the
  first record rather than applying T387's one-shot terminal disposition.
- App no longer handles no arguments with a MessageBox.  It still owns only
  process assembly; it does not parse a prompt, produce DOS input, write guest
  memory or change a guest binary.

## Verification

1. Generated `build/M0-T388/S2/initial-record-x86` from the existing audited
   declared-record Ninja generator and ran `ninja ... test`.
   `declared_dos_record_fixture.exe` passed.
2. The new fixture row proves the exact three-byte interactive tail,
   `COMMAND.COM` application path, DOS owner and a clear
   `terminal_on_command_exhaustion` flag.  Existing rows retain positional,
   explicit-option, invalid-option and first-request behavior.
3. Reused the identity-matched selected CPU40/x86 formal graph at
   `build/M0-T386/S3/base-env-x86` and relinked
   `original-softpc-process.exe`; the relink passed.  The sole linker warning
   is the pre-existing `.def` output-name mismatch (`LNK4070`), not a source
   or ABI failure.
4. No guest, firmware or media path was read for mutation, and no runtime
   observation was performed in this S.

## Result and handoff

The initial host-to-original-COMMAND contract is locally closed.  It does not
yet prove a visible prompt or keyboard input: those are explicitly S3/S4
owners.  The next implementation must first establish Console-first startup
and diagnostic isolation; it may not turn prompt lines into Base VDM records.
