# M0 T389 S6 — COMMAND nesting static correction

## Question

For the explicit `ntvdm.exe command.com` profile, does the single Base VDM
record make the first resident shell directly start an interactive second
`COMMAND.COM`, or does the original guest create an additional `/C` nesting
edge?

## Inputs

* `src/app/launch_declaration.c:209-248`, the current source-shaped Base VDM
  declaration.
* `src/mvdm-host/dos/command/cmdmisc.c:303-385`, the mirrored original
  `cmdGetNextCmd` result-to-guest conversion.
* `src/mvdm-guest/dos/v86/cmd/command/tcode.asm:530-1087` and
  `tmisc1.asm:390-546`, the original first-shell command and guest EXEC path.
* `src/mvdm-guest/dos/v86/cmd/command/init.asm:192-196, 673-746,
  1326-1333`, the original `/C`, `SingleCom`, and header rules.
* `src/mvdm-guest/dos/v86/cmd/command/tcode.asm:448-451, 1111-1315`, the
  non-first `DoReEnter` and DOS-CON path.

## Findings

1. Before S6, the app published one DOS record with
   `AppName=...\\COMMAND.COM` and `CmdLine="/C command.com\\r\\n"`.
   `cmdGetNextCmd` preserves both: it writes the application path to
   `EXECPATH`, then forms `UCOMBUF` from the file part plus that command line.
2. That obsolete record makes the first resident shell invoke a non-first
   `COMMAND.COM /C command.com`. The second shell owns `SingleCom`, does not
   request `54:01`, and does not read an interactive DOS `CON` line.
3. The obsolete second shell executes `SingleCom` through ordinary guest EXEC,
   producing a third non-first `COMMAND.COM` with no `/C` tail.
4. The third shell follows `DoReEnter`. With `SCS_CMDPROMPT != Prompt32`, it
   calls `Do16BitPrompt`, where original guest code emits `CRLF`, calls
   `PRINT_PROMPT`, and invokes `INT 21h/AH=0Ah` to read DOS `CON`.

## Selected recovery

The three-shell result is a diagnosis of the old app record, not the selected
product contract. S6 changes only the app-owned explicit `command.com` record:

* `AppName` remains `...\\COMMAND.COM`;
* `CmdLine` is the mandatory empty `"\\r\\n"` tail, rather than
  `"/C command.com\\r\\n"`;
* the original resident shell therefore guest-EXECs one non-first
  `COMMAND.COM` with no `/C` tail; and
* that second shell directly owns `DoReEnter → Do16BitPrompt → DOS CON`.

## Corrected lifecycle

```text
first resident COMMAND.COM
  -- BOP 54:01, one Base VDM record: COMMAND.COM + CR/LF -->
second COMMAND.COM
  -- original banner + Do16BitPrompt + DOS CON --> interactive shell
```

The first `54:01` remains the only Base VDM command-acquisition point in this
profile. The second shell neither reissues that BOP nor receives an app-owned
line reader; it owns guest DOS `CON`. This supersedes the obsolete T389 S1
interpretation and records why the prior `/C command.com` record was not a
valid explicit-interactive composition.

## Disposition

The product correction is limited to `src/app/launch_declaration.c`; guest
media, BOP dispatch, Base VDM record layout, and Console input ownership do
not change. The next runtime observation must find the first blocker from the
single original guest EXEC transition through the second shell's
`Do16BitPrompt`; it is not a missing second Base VDM record or an app-owned
input path.

## Focused validation

`build/M0-T389/S6/declared-command-record-r1/declared_dos_record_fixture.exe`
passes against the changed app source.  It proves that the local Base VDM
record contains the normal `COMMAND.COM` application path and exactly the
empty `CR/LF/NUL` command tail.  The fixture does not execute guest code and
therefore does not claim a runtime banner, prompt, DOS-CON read, or child
return; those remain fixed-container product-observation work.
