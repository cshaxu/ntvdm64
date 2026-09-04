# M0 T389 S8 — second COMMAND re-entry boundary

## Scope

This audit starts only after the source-defined NTDOS `$Exec` child transfer
already proven by S7.  It identifies the next original boundary for the
explicit `ntvdm.exe command.com` product row without changing guest media,
injecting a host command line, or changing keyboard/CPU behavior.

## Original child contract

The selected guest sources establish the following distinction.

* `init.asm` calls `GET_COMMAND_STATE`; only its first-command branch sets
  `SCS_FIRSTCOM=1`.  Its generated AUTOEXEC filename and
  `SVC_GETAUTOEXECBAT (54:0D)` belong to that first resident command.
* `tcode.asm::REGCOM` tests `SCS_FIRSTCOM`.  A non-first command jumps directly
  to `DoReEnter`, not to `do_again` or AUTOEXEC generation.
* With `SCS_REENTERED=0` and `SCS_CMDPROMPT != Prompt32`,
  `DoReEnter::exec_comspec` calls `Do16BitPrompt(Start16, FOR_SHELLOUT)`.
  That original routine emits CR/LF and the prompt, then invokes DOS buffered
  Console input (`INT 21h`, `AH=0Ah`).
* Before the first child `54:0F`, the loaded `COMMAND.COM` does original
  resident/transient initialization and allocation.  `54:0F` is its original
  environment request; `54:09` is the later original console-initialization
  service.  Neither is an app-created substitute.

## Fixed observations

Two otherwise identical fixed, Console-owning, non-debug x86 CPU40 runs used
the same staged product, media and explicit `command.com` target record:

1. S7 offered one ordinary Console key at the old `AH=1` status marker.
2. S8 supplied no keyboard input at all.

Both runs show the same final original lifecycle prefix:

```text
54:01 -> 54:0E -> 54:04 -> 50:12/50:42/50:02 -> 50:36
```

Both runs also have the same aggregate results after that final `50:36`:

* no additional `54:0F` environment dispatch;
* no `54:09` console-initialization dispatch;
* no `Do16BitPrompt`, BIOS `INT 16h` wait or DOS `CON` evidence.

The earlier `54:0D` and `54:0F` records occur before the one `54:01` target
record and belong to the first resident command's startup.  They cannot be
used to claim that the second child entered its own initialization or that it
re-ran AUTOEXEC.

The no-input observation finished at the same boundary.  Therefore the
previous accepted Console key/8042/ICA event is not the cause of the missing
second-command progress.

## Current disposition

The first unresolved interval is now precise but not yet attributed to a
missing provider: original NTDOS `$Exec` has called `50:36`, and the next
second-COMMAND host service (`54:0F`) has not been observed.  This is the
CPU/guest initialization interval between the original handoff and the first
child environment request.  It is not an AUTOEXEC, BaseSrv, BOP-record,
Console-worker or keyboard-controller issue.

S8 must next audit that original interval as one CPU/guest startup cohort and
identify either the first failed original instruction/contract or the first
arrival at `54:0F`.  It must not force IF, synthesize IRQ1, patch
`COMMAND.COM`, or treat the old `AH=1` idle marker as a prompt.
