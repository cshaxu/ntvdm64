# M0 T396 S16 — WOW kernel argument-carrier recovery

Date: 2026-09-07

## Boundary and recovery

Original `cmdmisc.c::GetWowKernelCmdLine` assigns a specific meaning to its
`-a` argument: it is the fully qualified first `KRNL386.EXE` guest-EXEC
carrier.  The selected Win16 program belongs instead to the separate WOW
BaseVDM record, which is consumed after the kernel bootstrap.

The product had conflated those two values.  `src/app/entry.c` passed
`declaration.target_application` (the selected `WOWEXEC.EXE`) to
`app_launch_declaration_prepare_softpc_arguments`, even though that argument
is named `wow_kernel_path`.  S16 makes the smallest source-shaped repair:

- `app_launch_declaration` builds `wow_kernel` as the already selected,
  immutable `<root>\\system32\\krnl386.exe` only for a Win16 declaration;
- the existing BaseVDM publish path continues to use
  `target_application`, so the user target is not substituted or duplicated;
- the existing original `-w -a` parser and the original guest media are
  unchanged.

No BOP result, loader branch, guest byte, or WOW32 success result was
introduced.

## Static and link proof

The current-source x86 `base_vdm_local_fixture` was compiled with the formal
graph flags and linked against the unchanged complete binding set.  It reports
`PASS: local Base VDM broker contract`.  Its Win16 assertion proves both
identities simultaneously:

- `wow_kernel == C:\\MVDM\\system32\\krnl386.exe`;
- published WOW record application remains `WOWEXEC.EXE`, with zero PIF
  bytes and no DOS-exhaustion terminal.

The full S16 formal graph was generated at
`build/M0-T396/S16/formal-x86/build.ninja`.  In this terminal environment
Ninja repeatedly becomes a childless stalled process after its ninth object;
that is not treated as a successful full build.  Instead, the two changed
current-source units (`src/app/entry.c` and `src/app/launch_declaration.c`)
were compiled directly using that graph's x86 flags and were linked first,
ahead of the unchanged S11 complete-archive app library and all of its
complete link inputs.  The resulting source-derived product is
`build/M0-T396/S16/s11-relink/ntvdm32-s16.exe`, SHA-256
`DD1D77E7824FF99A719BC94440FD4FC6A59BCE7B846F19D9ED46AAA8D4F639A2`.

Putting the two objects before the archive is material: COFF archive
resolution cannot extract the stale same-named `entry.obj` or
`launch_declaration.obj` after their definitions have already been resolved.
Every other linked archive input is the manifest-identified S11 complete
archive; this is a narrow current-source delta, not a mixed object claim.

## Current staged observation

The governed stage `O:\\t396` was updated with that product, the admitted S2
`WOW32.DLL`, original WOWEXEC workload, existing pure-DOS profile, and the
existing derived original `KRNL386.EXE`.  Its runtime-manifest SHA-256 is
`843ddf057fc03b6b00c0e87ad70e6d92c49bb1eada95c2ebe4efd6988b102616`.

The console-owning observer ran the bounded declared command:

```
ntvdm32.exe -f -o --command system32\\WOWEXEC.EXE
```

It ended after about seven seconds with `0xc0000409`, 15 loaded modules, and
no loaded `WOW32.DLL`.  That is not a WOW32-load success claim.  However, the
current original DEM-open report reaches:

```
O:\\T396\\SYSTEM32\\KRNL386.EXE
```

after the ordinary DOSX/COMMAND bootstrap.  The S15 predecessor had instead
identified the selected WOWEXEC image in the `-a` carrier.  The current
BaseVDM report is still `state=0102`, `owner=2`, `dos-state=0`, and
`pif-bytes=0`, so the target's WOW-record identity was preserved.  The trace
does not reach BOP `51h`.

S16 is therefore closed: the source-shaped carrier is repaired and the first
kernel EXEC is now the original KRNL386 boundary.  S17 owns only the next
original Kernel31 bootstrap boundary between that successful open and BOP
`51h`/`SafeLoadLibrary("WOW32")`; it may not undo this separation or
fabricate a loader result.
