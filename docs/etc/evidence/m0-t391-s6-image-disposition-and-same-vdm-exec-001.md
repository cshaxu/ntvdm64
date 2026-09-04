# M0 T391 S6 — Image Disposition and Same-VDM DOS EXEC

## Decision

The product has two source-distinct image boundaries.

1. Before a VDM exists, app classifies one concrete direct target. DOS enters
   the existing BaseVDM/PermCom record path; native PE uses public Windows
   process creation; Win16 is explicitly held at the WOW bootstrap boundary.
2. After guest `COMMAND.COM` is running, guest DOS owns `EXEC`. A recognized
   DOS COM/MZ image runs in the existing VDM and returns through the original
   parent PSP. It never starts a product child process.

## Original evidence

- `mvdm-guest/dos/v86/doskrnl/dos/msproc.asm:387-405` exchanges and clears
  `SCS_Is_Dos_Binary`, invokes `get_binary_type`, and calls the host only for
  unknown type `0ffh`.
- `msproc.asm:401-412` shows that original `SCS_DOSONLY=0` permits the
  unknown-format `SVC_CMDCHECKBINARY` path; it does not turn known DOS images
  into host commands.
- `mvdm-host/dos/command/cmdexec.c:252-254` returns carry clear for
  `SCS_DOS_BINARY`, leaving guest DOS to execute it.
- `cmdexec.c:258-393` retains the original WOW/native `/z` preparation only
  after the DOS result has not been selected.

## Implemented correction

- Removed the unproven `mvdm_command_child_vdm` product-child adapter and its
  `cmdexec.c` pre-branch divergence from the formal graph.
- Added `mvdm_image_classification` only at direct app entry. Its enum never
  crosses `VDMINFO` or guest memory.
- Kept original `cmdmisc.c` and its `SCS_Is_Dos_Binary` write at the BaseVDM
  record boundary unchanged in meaning.

## Verification

- Formal CPU40/x86 graph `formal-x86-disposition-r2` completed all 436 actions
  and linked `original-softpc-process.exe` after the adapter removal.
- The staged `ntvdm32.exe C:\Windows\System32\cmd.exe /c exit 37` direct
  native test returned `37`, without starting SoftPC/VDM initialization.
- The staged nonexistent direct target returned product failure `69` promptly,
  proving it was handed to Windows process creation rather than entering a
  BaseVDM/PermCom session.
- A pipe is not valid evidence for guest Console input: a bounded piped
  `COMMAND.COM` smoke test remained in the original Console wait and was
  stopped. Interactive COMMAND/DOS-CON evidence remains a separate runtime
  obligation.
