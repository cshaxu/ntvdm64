# Explicit interactive COMMAND profile

## Purpose

Define a small, source-shaped product composition for the OpenNT-modified
`COMMAND.COM` without changing guest media.  The profile makes an explicit
interactive shell request (`ntvdm.exe command.com`, with `-f` as a future
compatible spelling) distinct from a no-argument bootstrap and from a
one-target invocation.  The original guest's second-`COMMAND.COM` shell-out
path, rather than a host command-line producer, is the selected interaction
path.

The purpose is not to recreate a second DOS parser or the NT4 private
BaseSrv/CSRSS transport.  It is to send one original-shaped target record for
the explicitly requested child `COMMAND.COM`, then let its original shell-out
path print the prompt and consume DOS Console input itself.

## Product contract

### Launch forms

* `ntvdm.exe` starts the minimum VDM bootstrap with no interactive command
  producer and exits normally when that bootstrap has no declared work.
* `ntvdm.exe <program> [arguments]` declares one original-shaped target record.
  The original guest COMMAND path launches that target; when the direct target
  exits, the session exits.
* `ntvdm.exe command.com` requests the explicit interactive COMMAND profile.
  The bootstrap command path submits one original-shaped `COMMAND.COM` target
  record.  That child is the original non-first `COMMAND.COM` instance: it
  displays its original copyright header where its original flags permit and
  enters `Do16BitPrompt`.  The host must not recursively resubmit the record
  after the child exits.
* `ntvdm.exe -f` is a future compatible spelling for the same explicit
  interactive profile.  Its exact historical PIF/fullscreen interaction is an
  open question, not an implementation assumption.

### Established original COMMAND roles

The following are source-established conclusions, not product guesses:

* The first `COMMAND.COM` is the resident VDM control shell.  It establishes
  `SCS_FIRSTCOM=1`, suppresses its own copyright header, and normally requests
  declared work through `SVC_CMDGETNEXTCMD` / `BOP 54:01`.
* A positional DOS target is delivered to that first shell through the Base VDM
  record contract as the literal source-shaped `/C <target>` command tail.
  The first resident shell consumes `/C`; it is not passed as an argument to
  the target executable.
* A declared `COMMAND.COM` target is delivered as `/C command.com` to the
  first resident shell. It creates a second, non-first command shell, and the
  outer shell's `SingleCom` state is not inherited by that child. It is not a
  recursively resubmitted bootstrap. Subject to its original flags, the child
  prints `CopyrightMsg`; with `SCS_CMDPROMPT != 1` it follows
  `DoReEnter → Do16BitPrompt`, prints the guest prompt and reads DOS `CON`.
* `NTCMDPROMPT` is an original `CONFIG.NT`/SCS setting for shell-out and TSR
  prompt disposition.  If enabled, the original child path requests the
  32-bit COMSPEC route instead of `Do16BitPrompt`; it is not a generic switch
  that makes the first VDM-control shell interactive.
* “Permanent” describes the first shell's parent/control role, not an
  unconditional lifetime.  Original start information determines whether it
  waits for another Base VDM target or the VDM terminates after a child return.
  The direct-target and explicit-interactive product profiles deliberately
  select the latter after their declared target/child completes.

The resulting selected paths are:

```text
ntvdm.exe dosprog.exe
  first resident COMMAND.COM → one Base VDM target record → dosprog.exe
  → original parent recovery → product session end

ntvdm.exe command.com
  first resident COMMAND.COM → one Base VDM COMMAND.COM target record
  → second COMMAND.COM → original banner + Do16BitPrompt / DOS CON
  → child exit → original parent recovery → product session end
```

### Input and execution ownership

There is exactly one Console-input owner at a time:

1. The initial permanent OpenNT `COMMAND.COM` requests the single declared
   child target through its normal `BOP 54:01`/Base VDM route.
2. The non-first child `COMMAND.COM` follows original `DoReEnter` behavior.
   With `NTCMDPROMPT` disabled, it calls `Do16BitPrompt`, prints its own
   prompt and consumes input with DOS `CON`; it is therefore the Console-input
   owner.
3. SoftPC delivers those keys through its original Console event worker,
   keyboard controller, IRQ1, BIOS and DOS `CON` route.  The same ownership
   model covers `EDIT.COM`, QBASIC and every guest program launched from that
   shell.
4. With `NTCMDPROMPT` enabled, original guest code instead requests the
   original 32-bit COMSPEC path.  Whether that selected `cmd.exe` path is
   composable in the product is an explicit configuration disposition, never a
   reason to add a host line parser.
5. Child exit, EOF and cancellation retain their original shell-out/parent
   return ordering.  No app/session component reads a competing Console line.

### Presentation

* Text-mode guest execution uses the process Console by default.
* Graphics selection, an admitted PIF fullscreen disposition, or `Alt+Enter`
  uses the app presentation window to simulate a full DOS display.  This is
  not NT4 private hardware fullscreen.
* Changing surface never changes the active input owner.  Returning from the
  presentation window restores Console routing only when the original guest
  state permits it.
* PIF remains an original-owner configuration input.  The profile must recover
  its applicable fields and failures rather than treating CLI mode as a reason
  to skip PIF behavior.

## Source and component ownership

* `mvdm-host/dos/command` retains `cmdGetNextCmd`, `cmdGetStartInfo`,
  `cmdExec`, command parsing and guest lifecycle logic.
* `opennt-host` BaseClient/BaseSrv slices retain the selected `VDMINFO` and
  command-record layouts/order wherever independently composable.
* `adapter-mvdm-host-out` retains only same-shaped private monitor/Base VDM
  binding that public modern Win32 cannot provide directly.
* `session` owns target-record lifetime, cancellation and the bounded
  Console-owner state.  It owns no DOS parser or line reader.
* `app` owns CLI spelling, Console handles and the display surface.  It does
  not supply prompt lines.
* SoftPC remains the sole guest-machine keyboard/IRQ/display owner.  No app or
  session code writes a DOS input buffer.

## Required evidence before implementation

1. Audit the exact original target record and lifecycle for initial target,
   no-target, second `COMMAND.COM`, `DoReEnter`, `SCS_FIRSTCOM`,
   `SCS_CMDPROMPT`, child exit, EOF and cancellation.
2. Verify the original copyright-header and prompt conditions in the selected
   guest binary and fixed media.  The product must not fabricate a DOS
   `C:\\>` prompt.
3. Audit PIF and `-f` semantics before claiming compatibility with the NT4
   spelling.
4. Prove Console and presentation-window ownership transitions with actual
   text, graphics, Alt+Enter, Ctrl+C, EOF and a program that consumes keys.

## Proof plan

The profile is accepted only through all three layers below.  A trace that
merely reaches `54:01`, a banner string in an artifact, or a Console worker
marker alone is not sufficient.

1. **Static original-control-flow proof.** Record the selected guest-binary
   identity and source-to-binary provenance.  Trace the first permanent
   `COMMAND.COM` through its one Base VDM record, then prove the child has
   `SCS_FIRSTCOM != 1`, has no inherited `SingleCom`, and has the selected
   `SCS_CMDPROMPT` disposition. The proof must distinguish the outer literal
   `/C command.com` record from the child process's no-`/C` argument state. The retained source path must show
   `DoReEnter → Do16BitPrompt → PRINT_PROMPT → INT 21h/AH=0Ah`; it must also
   show the exact original `CopyrightMsg` condition.
2. **Focused local boundary proof.** With the real record layout, assert that
   exactly one initial `COMMAND.COM` target record is delivered and that no
   second record is fabricated after child completion.  Exercise the original
   SoftPC Console worker through keyboard controller/IRQ1/DOS `CON`, and
   observe the original `SCS_PROMPT16`, child/parent and `EXIT` result paths,
   including the selected terminate-after-return disposition.
   These checks may observe fixed-width states only; they may not modify guest
   state or consume a second Console input stream.
3. **Fixed-container runtime proof.** Use one formal x86 CPU40 product and
   immutable media.  Launch `ntvdm.exe command.com`, retain hashes and the
   exact command line, then record: original copyright header, original prompt,
   `ver`, `dir`, line editing/Backspace, a guest key-consuming program such as
   `EDIT.COM`, Ctrl+C, and `exit`.  For each step capture Console/window state,
   input owner and the immediate original owner of any failure.  Run the same
   target/child-return procedure for a direct positional program to prove it
   does not require the interactive-child shell.

If the child never reaches `Do16BitPrompt`, the result is a source-owned
blocker.  The repair task then begins at the first missing original transition
(record fields, `SCS_FIRSTCOM`, `SCS_CMDPROMPT`, `DoReEnter`, DOS `CON`,
keyboard IRQ or child/parent return), not at a trace-selected BOP leaf.

## Non-goals

* No guest `COMMAND.COM`, NTDOS or NTIO modification.
* No app-owned DOS command interpreter, host prompt reader or direct guest
  input injection.
* No private CSRSS/BaseSrv transport, kernel VDM, cross-process broker, WOW
  command delivery or arbitrary executable-type heuristic.
* No claim that `EDIT.COM`, Win16, graphics, PIF or `cmdExec32` is runtime
  complete before its own fixed-container evidence exists.

## Admission boundary

This proposal supersedes only the no-argument interactive premise of the
earlier interactive-console proposal after the current observational task is
closed.  It requires a separately admitted implementation packet because the
current packet expressly excludes a synthetic command record or host prompt
producer.  The current observation remains evidence; it is not rewritten as a
successful interactive-shell result.
