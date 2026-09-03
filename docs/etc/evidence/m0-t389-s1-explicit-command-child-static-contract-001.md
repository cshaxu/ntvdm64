# M0 T389 S1 — explicit COMMAND child static contract

## Question

Can the product distinguish an explicit `ntvdm.exe command.com` request from
the no-target bootstrap without modifying guest media or creating a host DOS
input path, while reaching the original second-`COMMAND.COM` prompt lifecycle?

## Selected immutable guest identity

The selected source-built guest identity is the same in the fixed package and
the retained source-build output:

| Location | Bytes | SHA-256 |
| --- | ---: | --- |
| `O:\ntvdm64\COMMAND.COM` | 50,384 | `908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43` |
| `O:\ntvdm64\system32\COMMAND.COM` | 50,384 | `908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43` |
| `build/M0-T225-S8/command-primary-source-build-r6/base/mvdm/dos/v86/cmd/command/COMMAND.COM` | 50,384 | `908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43` |

The following conclusions therefore concern the staged immutable selected
guest, not a replacement `COMMAND.COM`.

## Original source contract

1. The first instance alone sets `SCS_FIRSTCOM=1` in
   `mvdm-guest/dos/v86/cmd/command/init.asm:417-419`. Its resident state
   defaults are `SCS_REENTERED=0`, `SCS_FIRSTCOM=0` and `SCS_CMDPROMPT=0` in
   `rdata.asm:371-374` before that first-instance assignment.
2. `tcode.asm:448-451` sends `SCS_FIRSTCOM != 0` to `first_inst`; a non-first
   instance unconditionally enters `DoReEnter`. The first-shell normal path
   reaches `CMDSVC SVC_CMDGETNEXTCMD` at `tcode.asm:554-558`, not a host or
   DOS console line reader.
3. The non-first `DoReEnter` path observes `SCS_REENTERED==0`, sets it to one,
   and tests `SCS_CMDPROMPT` at `tcode.asm:1129-1147`. A value other than
   `Prompt32` calls `Do16BitPrompt`; that routine performs
   `CRLF2 -> PRINT_PROMPT -> INT 21h/AH=0Ah` at `tcode.asm:1278-1295`.
4. The original header is conditional, not product-created:
   `init.asm:1326-1333` suppresses `CopyrightMsg` for a first instance,
   `SCS_CMDPROMPT==1`, or `SingleCom!=0`; otherwise it calls `RPrint`.
5. The `/C` state belongs to the instance which parses it, not to the target
   executable. `init.asm:741-745` records the remainder in that instance's
   `SingleCom` and clears permanent-command state. The existing `/C command.com`
   record is thus the source-shaped outer-shell operation: the first resident
   shell consumes `/C`, executes a second `COMMAND.COM` with no `/C` tail, then
   exits. The second shell retains `SingleCom == 0` and its `DoReEnter` path.
6. The non-first classification is implemented by the first resident shell's
   own original INT 2F handler: `rucode.asm:994-1004` handles
   `GET_COMMAND_STATE` and explicitly clears AX. The child `init.asm:399-417`
   therefore does not take `first_com`; it retains `SCS_FIRSTCOM == 0`.

## Existing product binding and required successor seam

The current positional declaration deliberately forms
`/C <requested-command>\r\n` in `app/launch_declaration.c:209-213` and arms
`terminal_on_command_exhaustion` only for a declared command at lines 143-151.
It is correct for both a one-target DOS command and an explicit
`COMMAND.COM` target: applying it to `command.com` places `SingleCom` in the
outer first shell, while the executed second shell receives no `/C` tail.

The no-target record remains a distinct bootstrap payload: selected
`system32\\COMMAND.COM` application and `CR/LF/NUL` command tail. It is not a
declared child and must not acquire terminal-on-command-exhaustion behavior.

The original `cmdGetNextCmd` initializes its first request with
`ASKING_FOR_FIRST_COMMAND` in `mvdm-host/dos/command/cmdmisc.c:126-139`; its
`VDMINFO` transport then calls `GetNextVDMCommand` at lines 236-292. The
selected BaseSrv source (`opennt-host/base/win32/server/srvvdm.c:130-153`)
separates DOS records and WOW records; it does not read Console text. These
facts preserve the required one-record, no-host-parser boundary.

## S1 conclusion and S2 admission seam

The requested profile is source-shaped at the app-record boundary already:
retain the existing positional `COMMAND.COM` application plus literal
`/C command.com` payload, then apply terminal-on-command-exhaustion only after
original child/parent return. It must remain distinct from the no-target
bootstrap, but it must not invent a second no-`/C` app record.

S1 does **not** claim that the child reaches the banner, prompt, keyboard
wait, `ver`, `dir`, `EDIT.COM`, Ctrl+C, `exit`, or a presentation mode. Those
are S2+ focused and fixed-container proof obligations. No source-owned blocker
was found at the static record/state boundary.
