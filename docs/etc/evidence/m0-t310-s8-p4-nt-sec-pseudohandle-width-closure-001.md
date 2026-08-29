# M0 T310 S8 P4 `nt_sec` pseudo-handle width closure

## Original contract

`RegainRegenMemory` is the original SoftPC host-side helper used while a
fullscreen hardware transition returns the regeneration range to the
windowed process. It assigns `NtCurrentProcess()` to `processHandle`, then
only enters the reviewed error branch when that value is null.

The historical argument was `(DWORD)processHandle`. In that branch its value
is exactly zero; it is not a guest handle, an opaque mapping identity, or a
live host handle value. Retaining the cast nevertheless causes x64 C4311.

## Minimal correction

`MVDM-HOST-DIV-124` passes `0u` directly to the original
`DisplayErrorTerm(EHS_FUNC_FAILED, ..., __FILE__, __LINE__)` call. It leaves
the original current-process query, allocation request, failure ordering and
the normal native `HANDLE` call to `NtAllocateVirtualMemory` unchanged.

The current selected graph does not define `MONITOR` or `X86GFX`; this narrow
correction is not evidence that the fullscreen regeneration transition itself
is enabled or closed. Its future machine/video recovery must use the existing
session-owned physical mapping boundary rather than a fixed host address.

## Verification

The formal `/MT` Ninja target `obj/host/nt_sec.obj` rebuilt successfully on
both targets:

- `build/M0-T310/S8/p1-machine-source/x64/nt-sec-pseudohandle.log`
- `build/M0-T310/S8/p1-machine-source/x86/nt-sec-pseudohandle.log`

Neither log emits C4311 or a compiler error for `nt_sec.c`.
