# M0 T466 S3 — asynchronous multimedia owner audit

## Purpose

This is an in-progress source audit, not an asynchronous-device acceptance
claim.  It determines whether an unconnected project provider must be restored
before a real Win16 callback/cancellation witness is attempted.

## Reached architecture

The original Win16 `mmsystem` does not resolve multimedia protocol entrypoints
from project `WOW32.DLL`.  `init.c::ThunkInit` loads `WINMM.DLL` through
`LoadLibraryEx32W`, then asks original KERNEL `GetProcAddress32W` for:

```
NotifyCallbackData  wod32Message  wid32Message  mod32Message
mid32Message        aux32Message  mci32Message  tid32Message
joy32Message
```

The selected original Win16 thunk assembly passes those resolved procedures
through `MMCALLPROC32`.  `NotifyCallbackData` publishes the original fixed
`CALLBACK_DATA` ring to WINMM; it is withdrawn before the guest storage is
unlocked/freed.  The ring's producer/consumer ordering and interrupt delivery
remain the original mmsystem/host contract, not a project callback queue.

The project does export original `WOW32DriverCallback` and
`WOW32ResolveHandle`.  The former only forwards callback dispatch to existing
`WINMM!WOW32DriverCallback`; the latter forwards multimedia handle conversion
to existing `WINMM!WOW32ResolveMultiMediaHandle`.  Neither is a scheduler,
buffer registry or device provider.

## Disabled source is not a missing binding

The original `wmmedia1.c`, `wmmedia2.c`, `wmmstruc.c`, `wmmstru1.c` and
`wmmstru2.c` structured wave/MIDI converters are globally under their original
`#if 0` and have no dispatch-table registration in selected `wmmtbl2.h`.
Their `W32CommonDeviceOpen` body allocates copied headers/instance data and
releases them only from native completion/close callbacks.  Re-enabling it
would create an additional retained owner alongside the active Win16/host
protocol; it is therefore not an admissible shortcut.

Original `drvproc.c::AppExit` remains the owning task-rundown path: it resets
and closes the task's wave/MIDI/MMIO handles, asks MCI to close creator-task
devices, and leaves outstanding timer events to the original timer driver.
No project-owned callback or buffer survives that path.

## Current host boundary witness

The current x86 focused native-extension fixture resolves all nine mmsystem
entrypoints plus the two callback/handle helpers listed above directly from
SysWOW64 `WINMM.DLL`.  For every name the existing native-scope lookup wrapper
returns the exact same address.  It invokes only `tid32Message` with the known
synchronous `THUNK_TIMEGETTIME` packet; it does not fabricate a callback,
open a device or retain a host pointer.

`build/t466-s3-native-extension-r1` passed with:

```
WOW_NATIVE_EXTENSION_BOUNDARY errors=0 ... tid32_native_sync=1 mm_private_exports=11
```

## Remaining S3 proof

This audit rules out a missing project-side resolver or substitute provider.
It does not prove that a real Win16 `timeSetEvent`, wave/MIDI completion,
cancellation, failed open, or task exit reaches and drains the host callback
path.  The next source trace must identify an immutable Win16 caller/witness
for that original route and verify its callback, cancellation and task-rundown
semantics without modifying guest media or reviving disabled thunks.
