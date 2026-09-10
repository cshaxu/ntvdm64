# M0 T335 S3 P11 — original WOW startup activation observation

## Question

Does the selected product need a newly authored guest loader before original
WOW activation can begin, or can the original SoftPC command-line route consume
the already-carried Win16 media?

## Inputs and procedure

- Fresh formal CCPU40 `original-softpc-process.exe` products were linked from
  the T335/S3/P11 Ninja graphs on both Win32/x86 and Win32/x64.
- The x86 product was staged with the existing immutable DOS and SoftPC ROM
  media.  The existing retail `build/output/wow16/kernel31/retail3/krnl386.exe`
  was supplied as the original process argument, not copied or interpreted by
  app.
- The existing console-owning, non-debug observer was given exactly:
  `-f -w -a O:\repos.hobby\ntvdm64\build\output\wow16\kernel31\retail3\krnl386.exe --ordinary-child`.
  Its sole extension permits an explicitly declared product argument tail; it
  neither parses nor transforms that tail.
- `app_launch_declaration_consume_options` removes only
  `--ordinary-child`; original SoftPC receives `-f -w -a` unchanged.

## Source result

`GetWowKernelCmdLine` in the selected original `dos/command/cmdmisc.c` reads
the process command line, locates ` -a `, and prepares the original guest EXEC
target.  Therefore the relevant activation mechanism is original COMMAND/DOS
logic, not an app-owned NE or guest loader.

The initial run stopped with `ERROR_CALL_NOT_IMPLEMENTED (120)` before guest
execution because original `host_applInit` always invokes private USER
`ShowStartGlass` for WOW.  The original USER body is a void
`NtUserCallOneParam(..., SFI__SHOWSTARTGLASS)` presentation request.  For the
declared non-GUI profile only, `ADAPTER-WIN32-039` keeps its name, parameter
and caller order while providing no presentation; it leaves hard-error
handling unavailable.

## Observation

After that bounded adapter correction, the unchanged x86 console-owning
container observed these original dispatch markers:

```text
MVDM-BOP-DISPATCH 50:11
MVDM-BOP-DISPATCH 50:3B
MVDM-BOP-DISPATCH 50:0F
MVDM-BOP-DISPATCH 50:1B
MVDM-BOP-DISPATCH 54:05
```

The selected original dispatch tables give this sequence a precise startup
meaning, rather than treating it as a list of independently selected BOP
repairs:

- `50:11` is `demLoadDos`, the original `NTDOS.SYS` load service.
- `50:3B` is `demIsDebug`, the original DOS debugger-state query.
- `50:0F` is `demGetDrives`, the original host-drive enumeration service.
- `50:1B` is `demSetDTALocation`, the original DOS DTA/current-PDB address
  registration service.
- `54:05` is `cmdSetInfo`, which records the original COMMAND `SCSINFO`,
  `IsDosBinary`, and `FDAccess` guest addresses.

`MS_bop_4` calls the original `CmdDispatch`, advances the original guest IP by
one service byte, and returns to CCPU40.  No subsequent BOP marker is
observed.  The current evidence therefore places the `0xC0000005` after the
successful return from this initialization sequence and before the next BOP
ingress; it does not identify a failed DEM/COMMAND service, a missing guest
file, or a missing Win16 loader.  It is a guest/CPU-continuity attribution
only, so no leaf BOP provider is changed by this observation.

The matching original guest continuation in
`mvdm-guest/dos/v86/doskrnl/dos/msinit.asm` makes the attribution narrower:
after `CMDSVC SVC_CMDSETINFO`, NTDOS restores saved registers, records the
BIOS exchange block, publishes DOSDATA segment pointers, switches to the DOS
disk stack, and initializes its Win386/DOSWOW instance-pointer fields.  The
current observation ends within that original NTDOS initialization continuation
or its CCPU40 execution, before a later DOS/COMMAND or WOW ingress can be
observed.  It cannot support a loader or BOP-provider repair.

The process subsequently exited with `0xC0000005`.  No BOP 51/WOW provider
load or `CallBack16` return is claimed.  The observation proves only that
existing guest media plus the original command-line contract reaches the
selected guest/DOS path; it disproves neither the remaining callback work nor
the later need to package Win16 media beside a released executable.

## Disposition

No guest loader was created.  S3/P12 subsequently closed the narrow original
`CallBack16`/CCPU40 return transaction, and S4 formally reconciled the selected
non-GUI package.  The reached post-`54:05` failure transfers to the CPU40/guest
continuity package; it does not authorize a BOP-provider or loader repair.
