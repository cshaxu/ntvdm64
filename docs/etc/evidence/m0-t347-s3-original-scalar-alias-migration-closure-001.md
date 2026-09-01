# M0 T347 S3 — Original scalar-alias migration closure

## Question

Can the original COMMAND/DEM/SoftPC users of the cmdSetInfo scalar locations
be migrated as one source-shaped cohort without retaining a raw GetVDMAddr
alias, changing their fixed-width guest ABI, or adding another mapper?

## Original contract

The selected original dos/command/cmdmisc.c cmdSetInfo receives:

- DS:DX — SCSINFO;
- DS:BX — SCS_Is_Dos_Binary byte; and
- DS:CX — SCS_FDACCESS word.

Original COMMAND writes the binary byte before command/PIF classification;
original PIF clears it where type is unknown. Original DEM resets the
FD-access word on disk reset. Original fixed-disk and floppy paths increment,
decrement, and read the same unsigned word. The original guest code consumes
these bytes/words as ordinary DOS data.

## Migration

cmdSetInfo now registers all three original numeric locations atomically
through mvdm_command_guest_state_set_scs_scalars. No live declaration or
definition of pIsDosBinary or pFDAccess remains.

- cmdmisc.c writes SCS_Is_Dos_Binary through the adapter; registration or
  write refusal retains a visible CF/AX=ERROR_INVALID_ADDRESS failure.
- cmdpif.c retains its original VOID signature and reports scalar refusal
  through the existing caller-owned VDMInfo.ErrorCode.
- demmisc.c retains reset order and terminates the current VDM if the
  required guest scalar is no longer valid.
- nt_fdisk.c and nt_rflop.c retain their original successful open/close order
  and unsigned 16-bit counter semantics. Open rolls the native handle state
  back on scalar refusal; a stale counter at close returns the original
  Boolean failure result rather than fabricating a completed close.

Each mirror difference is registered under MVDM-HOST-DIV-111; the adapter
contract remains ADAPTER-SOFTPC-037. The migration adds no BOP routing,
disk policy, guest-media, firmware, CPU, or mapping-manager behavior.

## Verification

Source sweep:

```text
rg -n "pIsDosBinary|pFDAccess" src/mvdm-host
```

Observed result: no live declaration, definition, or dereference.

Focused scalar contract fixture:

```text
cmd.exe /d /c call O:\\repos.hobby\\ntvdm64\\build\\M0-T347\\S2\\run-command-scalar-state-fixture.cmd
```

Observed result: exit code 0. It covers byte write, little-endian word
read/write, modular wrap, cross-session rejection, inactive/restarted memory,
and range rejection.

Forced selected CPU40/x86 rebuild:

```text
cmd.exe /d /c call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x86 -host_arch=x64 ^&^& ninja -C "O:\\repos.hobby\\ntvdm64\\build\\M0-T343\\S3\\record-x86" -j 8 original-softpc-process.exe
```

Observed result: cmdmisc.c, cmdpif.c, demmisc.c, nt_fdisk.c, and nt_rflop.c
recompiled; the original COMMAND, DEM and SoftPC host libraries rebuilt; and
original-softpc-process.exe linked successfully. Original-source warnings
remain visible and were neither suppressed nor changed by S3.

## Boundary

This closes only source migration and product-link proof for this scalar cohort.
It does not prove an NTIO/NTDOS guest reaches the affected paths, disk/floppy
hardware behavior, or continuous DOS execution. Those are later machine and
guest integration work, not a reason to retain unsafe aliases.
