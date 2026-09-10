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
* `54:0F` and `54:09` are original COMMAND-side services, but they are not a
  required marker for the second-COMMAND `DoReEnter` path.  They cannot be
  used as a proxy for whether the CPU completed NTDOS `$Exec` or whether the
  child reached `REGCOM`.

## Fixed observations

Two otherwise identical fixed, Console-owning, non-debug x86 CPU40 runs used
the same staged product, media and explicit `command.com` target record:

1. S7 offered one ordinary Console key at the old `AH=1` status marker.
2. S8 supplied no keyboard input at all.

Both runs show the same final original lifecycle prefix:

```text
54:01 -> 54:0E -> 54:04 -> 50:12/50:42/50:02 -> 50:36
```

The default-off CPU40 far-return witness now establishes that the unchanged
`$Exec` return actually completes:

```text
MVDM-CPU-RETF source=00A7:1113 target=0713:0100
```

This is the original transfer into the loaded child image, so the old
interpretation (that the interval ended before the child could start) is
incorrect.  Neither run reaches a later observed `54:0F`, `54:09`,
`Do16BitPrompt`, BIOS `INT 16h` wait or DOS `CON` marker in the fixed
30-second window; those absences are observations, not a child-start
criterion.

The earlier `54:0D` and `54:0F` records occur before the one `54:01` target
record and belong to the first resident command's startup.  They cannot be
used to claim that the second child entered its own initialization or that it
re-ran AUTOEXEC.

The no-input observation finished at the same boundary.  Therefore the
previous accepted Console key/8042/ICA event is not the cause of the missing
second-command progress.

## Source-owned predecessor found

The simultaneous, default-off original `demChMod` witness records the active
file-service sequence after the child transfer.  It first finds
`C:\\WINDOWS\\SYSTEM.INI`, then applies the unchanged DOS executable search
contract to `KRNL386.EXE` through the staged system directory and the inherited
host `PATH`; every `KRNL386.EXE` attempt fails with the original DOS error.
The accompanying original DEM-open record also shows the same startup cohort
loading `O:\\NTVDM64\\SYSTEM32\\DOSX.EXE` before that search.

The immutable staged `AUTOEXEC.NT` contains its original NTVDM command
`lh %SystemRoot%\\system32\\dosx`.  `DOSX.EXE` is a DPMI/WOW bootstrap input,
not DOS `CON`; its `SYSTEM.INI`/`KRNL386.EXE` discovery is therefore a
source-owned predecessor that consumes the fixed observation interval.  It is
not attributable to BaseVDM record delivery, `$Exec`/CPU40 `RETF`, a BOP
return, Console input, or an app prompt reader.

## S8 disposition

S8 has reached its allowed evidence exit: the desired second-COMMAND
`DoReEnter -> Do16BitPrompt -> DOS CON` path is not yet observable because the
selected immutable first-shell AUTOEXEC profile enters original DOSX/DPMI/WOW
bootstrap first.  Resolving that predecessor requires an explicitly admitted
pure-DOS media/profile decision or a DPMI/WOW bootstrap recovery package;
neither may be smuggled into this COMMAND-only cohort.  S8 makes no claim that
the prompt itself, line editing, BIOS input or child return works.
