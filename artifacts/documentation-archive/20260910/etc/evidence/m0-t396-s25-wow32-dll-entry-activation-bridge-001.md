# M0 T396 S25 — WOW32 DLL-entry activation bridge 001

## Change

The formal WOW32 generator now writes a build-local
`wow32-dll-entry-bridge.c`.  Its stdcall `DllMain` forwards the three loader
arguments to the original cdecl `W32DllInitialize`.  This is deliberately not
a linker `/ENTRY` override: the PE still enters CRT
`__DllMainCRTStartup@12`, which then calls the bridge.  The original 77 WOW32
C bodies, source manifest, exports and parent contract are unchanged.

The first link exposed the actual historical ABI detail: the original map
symbol is cdecl `_W32DllInitialize`, not `_W32DllInitialize@12`.  The bridge
therefore explicitly converts the standard CRT `DllMain@12` callback ABI to
the original cdecl entry.  That correction is wholly inside the generated
build carrier.

## Fresh provider composition

The S25 fresh x86 provider consists of all 77 original selected bodies, four
previously admitted supports, the original resource, system-import alias
object, new entry bridge, generated import libraries and the S20 parent import
contract.  It links normally without `/FORCE`:

```text
build/M0-T396/S25/wow32-x86-final/wow32.dll
SHA-256: CEC8951B5CE20C64CF53393C22587E26ACC7441306C65E34FC6B6E0FD5BD9C3A
```

The map records all three required points:

```text
_W32DllInitialize          10002730  wow32.obj
_DllMain@12                10049780  wow32-dll-entry-bridge.obj
__DllMainCRTStartup@12     10049f29  LIBCMT:dll_dllmain.obj
```

Thus CRT remains the executable entry and the original process-attach body is
now reachable through its normal callback, rather than through a CRT-bypassing
entry replacement.

## Runtime observation

The S25 hash was staged in the fixed short-root runtime.  The focused loader
surrogate returned a non-null WOW32 module handle.  The normal Console-owning,
non-debug WOWEXEC run also retained `loaded-wow32=yes` and 23 modules, but it
no longer terminates with S24's `ntdll!RtlAllocateHeap+0x4a` null-heap access
violation.  Its bounded result is:

```text
exit=0xc0000409
elapsed-ms=7781
loaded-module-count=23
loaded-wow32=yes
```

The new WER record attributes this terminal to `ntvdm32.exe+0x5cb1b` with
fast-fail code 7.  The S20 product map places CRT `_abort` at `0x5caf2`, so
the terminal is inside that abort implementation, not proof of an entry-bridge
failure.  S26 owns the source-path provenance of the reached post-initializer
abort before any new behavior change.

## Disposition

S25 is closed: it restores the historical DLLENTRY activation without
bypassing CRT and removes the specifically attributed null-heap terminal.  It
does not claim W32Init, W32Dispatch, or Win16 lifecycle success.

