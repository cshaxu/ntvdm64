# M0 T396 S15 — current-product WOWEXEC loader observation

Date: 2026-09-07

## Inputs

The selected, unmodified `src/mvdm-guest/wow16/test/shell/WOWEXEC.EXE` is a
Win16 NE image (MZ new-header offset `0x400`, signature `NE`), SHA-256
`38EF366EB55AECC63183F32493B837CB4BDF059D969DD27185FCE6266CDC074D`.
It was staged by `Stage-OriginalSoftpcRuntime.mjs` with the current S11 x86
archive `ntvdm32-int6.exe`, SHA-256
`0F1777C169CD6FE4598A500608D9160BB55396A2917932FEB766EA57BE53A41E`, and
the already-admitted S2 `WOW32.DLL` companion.  The resulting runtime manifest
SHA-256 is `ba83b997c372b59e94bd4391a6c09fc292700cc3cc746c7774a6db3a75390c35`.

## Result

The bounded current-product launch

```
O:\t396\ntvdm32.exe -f -o --command system32\WOWEXEC.EXE
```

exited after 6110 ms with `0xc0000409`, loaded 15 modules, and did not load
`WOW32.DLL`.  Its BOP trace reaches the original DPMI32 provider (`53:00`,
`53:01`, `53:02`, `53:04`, `53:06`, `53:07`, `53:0F`, `53:11`, `53:14`), but
contains no `51h` dispatch.

The final `$Exec` record is not evidence of a corrupted transfer frame.  The
original `msproc.asm` COM path sets `Exec_Init_CS:IP` to the PSP plus `0100h`,
sets `SS:SP`, and writes a zero return word at that stack location.  Thus the
observed `8AFF:0000` is compatible with the original COM-return convention.

## Earlier owner identified

The decisive earlier boundary is the app-to-original-WOW argument carrier.
Original `cmdmisc.c::GetWowKernelCmdLine` documents and parses `-a
%SystemRoot%\system32\krnl386.exe`; it copies that token into COMMAND's
first guest EXEC path.  The user-selected Win16 program remains in the
separate WOW BaseVDM record for the subsequently booted kernel/WOWEXEC path.

The current app instead calls `app_launch_declaration_prepare_softpc_arguments`
with `declaration.target_application`, so it supplies `-a` with
`WOWEXEC.EXE`.  Its own source comments call that value `wow_kernel_path`,
but it is not the kernel.  The original `GetWowKernelCmdLine` therefore
faithfully executes the wrong NE image through its documented first-EXEC
carrier.  This is a product composition error, not a guest-media, DPMI32,
or WOW32 provider failure.

S15 is closed at this exact owner.  S16 owns the narrow recovery: retain the
classified target in the WOW record, construct only the selected immutable
`system32\krnl386.exe` `-a` carrier, and then prove the current runtime reaches
the next original boundary without fabricating BOP 51h or altering media.
