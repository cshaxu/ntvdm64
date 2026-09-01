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

The process subsequently exited with `0xC0000005`.  No BOP 51/WOW provider
load or `CallBack16` return is claimed.  The observation proves only that
existing guest media plus the original command-line contract reaches the
selected guest/DOS path; it disproves neither the remaining callback work nor
the later need to package Win16 media beside a released executable.

## Disposition

No guest loader was created.  The next S3 work remains source-shaped diagnosis
of the reached post-`54:05` continuity failure and, only after that path is
stable, the original BOP 51/WOW provider boundary.
