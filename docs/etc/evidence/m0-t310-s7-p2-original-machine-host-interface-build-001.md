# M0 T310 S7 P2 — original machine host-interface build evidence

## Scope

This step closes the bounded compiler-interface gap exposed when the selected
original SoftPC machine composition added its original `config.c`, `nt_pif.c`,
`nt_unix.c`, and `nt_fdisk.c` roots.  It is source/build closure only: it does
not claim a runnable controller, a console provider, or a host-disk policy.

## Source-shaped decisions

- `config.c` remains the original PIF/console configuration control flow.
  `adapter-mvdm-host-out/win32/include/conapi.h` now carries only its reached
  historical declaration shapes (`APPKEY`, `SetConsoleKeyShortcuts`, and the
  graphics buffer layout).  It deliberately supplies no replacement console
  behavior.  This is `ADAPTER-WIN32-012`.
- The adapter-private process-parameter carrier now exposes the reached
  `ConsoleHandle` member and binds it to the current public standard-output
  handle.  This preserves the source's title-identity input without exposing
  any value across a guest or component boundary.
- `nt_fdisk.c` retains its original `NtQueryInformationFile`,
  `NtFsControlFile`, and `NtDeviceIoControlFile` call shapes.  The existing
  `ADAPTER-WIN32-016` facade resolves the corresponding public `ntdll`
  exports and copies completion into the original 32-bit host-local record.
- `timeval.h` and `nt_unix.c` have registered `MVDM-HOST-DIV-032/-033`
  mirror differences: the old 32-bit `host_time` declaration is made
  explicit, and retired UCRT `_sys_errlist/_sys_nerr` storage is replaced by
  public `strerror` while preserving `host_get_system_error`'s `char *`
  result contract.

No former project-authored SoftPC patch-hook or timer fallback is selected.
No Bochs, MONITOR, kernel-VDM, or `src.old` input appears in the graph.

## Formal verification

The following commands were run outside the sandbox:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\build\New-T310OriginalSoftpcNinja.ps1 -Architecture x64 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build\M0-T310\S7\machine\x64 -j8 original-softpc-candidate

powershell.exe -ExecutionPolicy Bypass -File tools\build\New-T310OriginalSoftpcNinja.ps1 -Architecture x86 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build\M0-T310\S7\machine\x86 -j8 original-softpc-candidate
```

Both graphs completed.  Each compiles and archives 130 original `ccpu386`
translation units plus the selected original BIOS, keymouse, system, support,
and fourteen host roots.  The produced archives include
`original-ccpu386.lib`, `original-softpc-{bios,keymouse,system,support,host-roots}.lib`,
`softpc-win32-bindings.lib`, and the separate NTVDMx64 patch-evidence archive.

The compilers report historical warnings in the original CCPU/SoftPC sources;
there are no x86 or x64 compilation errors in this candidate target.

## Result and next boundary

The original controller bodies now have a common x86/x64 source-selection and
static-library closure through their immediate Win32/CRT declaration boundary.
The remaining S7 work is runtime composition: prove original create, reset,
firmware/machine initialization, bounded execution, typed stop and teardown,
then classify any enabled/unavailable device behavior by its original owner.
