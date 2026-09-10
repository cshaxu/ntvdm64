# M0 T389 S2 — explicit COMMAND record contract

## Scope

Verify the app/BaseVDM boundary only. This is not evidence that a guest shell
has reached a banner, prompt, or keyboard wait.

## Source conclusion retained

The immutable OpenNT `COMMAND.COM` source assigns `SingleCom` to the instance
which parses `/C` (`mvdm-guest/dos/v86/cmd/command/init.asm:737-744`). The
first resident shell therefore consumes the app's literal
`/C command.com` record, executes a second `COMMAND.COM`, and exits. The
second executable child has no inherited `/C` tail; its non-first path is
`DoReEnter -> Do16BitPrompt -> INT 21h/AH=0Ah`
(`tcode.asm:1111-1151`, `1278-1295`).

## Product binding

`app/launch_declaration.c` uses one ordinary positional record shape for every
declared target, including `command.com`:

```text
application = <session-root>\\system32\\COMMAND.COM
command     = /C command.com\r\n\0
```

There is no app-owned special child record, no host command parser, and no
second BaseVDM publication. A no-target record remains `\r\n\0` and is not
terminal on exhaustion.

## Formal x86 verification

The existing real `base_vdm_local` fixture was extended with the exact
`command.com` positional case. It asserts that the record remains DOS-owned,
terminal only after the declared outer command returns, and has exactly the
ordinary literal `/C command.com\r\n\0` bytes.

```text
node tools/build/New-T366DeclaredDosRecordNinja.mjs . build/M0-T389/S2/declared-dos-record-x86
ninja -C build/M0-T389/S2/declared-dos-record-x86 test

PASS: local Base VDM broker contract
```

## Disposition

S2 is closed. S3 must use one fixed CPU40/x86 product and immutable media to
observe the original second-shell transition. It must not change the record
shape established here.
