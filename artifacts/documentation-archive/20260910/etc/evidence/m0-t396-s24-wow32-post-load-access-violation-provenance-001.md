# M0 T396 S24 — WOW32 post-load access-violation provenance 001

## Result

The first ordinary post-load access violation is `ntdll!RtlAllocateHeap+0x4a`.
It is reached after the complete WOW32 image loads, but before the original
WOW32 DLL initialization body has ever run.  The smallest next owner is the
build composition of the historical WOW32 `DLLENTRY=W32DllInitialize`
contract, not a WOW32 behavior rewrite.

## Ordinary observation

The S23-stage ordinary, Console-owning run of `system32\\WOWEXEC.EXE` remains
non-debug and exits `0xc0000005` with `loaded-wow32=yes` and 23 modules.  A
build-local copy of the existing observer recorded the relevant module bases:

```text
ntdll.dll base=0x77860000 size=0x001bf000
WOW32.DLL base=0x6a300000 size=0x0007d000
exception address=0x7789f8ca access=0x00000000 target=0x00000008
```

The fault relative virtual address is `0x3f8ca`.  The live SysWOW64 NTDLL
export table places `RtlAllocateHeap` at RVA `0x3f880`; therefore the observed
instruction is `ntdll!RtlAllocateHeap+0x4a`.  The zero-ish access target is
consistent with an uninitialized heap handle; it is not an instruction inside
the staged WOW32 image.  WER independently records NTDLL as fault module and
the same `0xc0000005` offset.

## Source and map correlation

The selected original `src/mvdm-host/wow32/sources` declares:

```text
DLLENTRY=W32DllInitialize
```

`src/mvdm-host/wow32/wow32.c` implements that routine.  On
`DLL_PROCESS_ATTACH` it creates `hWOWHeap` with `HeapCreate` before later
WOW32 allocation paths can call `HeapAlloc`.  The S23 generated PE map instead
shows the executable entry at `__DllMainCRTStartup@12` and the default CRT
`_DllMain@12` stub; `_W32DllInitialize` is present at a separate RVA but has
no caller in that default entry chain.  Thus the loader can successfully map
the provider while silently skipping the historical initialization contract.

Using `/ENTRY:_W32DllInitialize@12` alone is not an acceptable recovery: it
would bypass the modern CRT startup which the `/MT` provider composition still
needs.  The source-shaped smallest recovery is a generated, build-only
`DllMain` bridge with the normal three-argument Win32 DLL ABI.  CRT startup
will invoke that bridge, and it will invoke the original
`W32DllInitialize(instance, reason, reserved)` with ABI-equivalent arguments.
The outer `DllMain` is stdcall as required by CRT, while the original entry is
its actual map-confirmed cdecl `_W32DllInitialize`; the bridge is the bounded
place that performs that calling-convention conversion.
No original body, export table, CPU/BOP result, or runtime target is changed.

## Disposition

S24 is closed.  It attributes the ordinary fault sufficiently to admit a
single build-composition recovery packet.  It does not claim that `W32Init`,
dispatch, or a Win16 client lifecycle works.
