# M0 T466 S3 — asynchronous multimedia owner audit

## Purpose

This is a source-ownership closure, not an asynchronous-device acceptance
claim.  It determines whether an unconnected project provider must be restored
before real Win16 device acceptance is attempted.

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

## S3 disposition

| Selected asynchronous family | Reached original owner | S3 disposition | S4 obligation |
| --- | --- | --- | --- |
| `timeSetEvent` / `timeKillEvent` | Win16 `time.c` packages `TIMEREVENT`; the original TIMER driver owns event scheduling and invokes original `DriverCallback`. | No host provider or retained project state exists to restore. The timer driver owns cancellation; its comment and `AppExit` retain timer-event retirement. | Run a real immutable Win16 timer callback/cancel witness. |
| Wave/MIDI completion and queued headers | Win16 `wave.c` / `midi.c` own headers and handle state; the original WINMM private endpoints own their native invocation and callback-ring production. | The 11-entry witness proves the existing host boundary. The disabled 32-bit converter cannot be revived because it would become a duplicate retained-header owner. | Run real device/open/error/completion/cancel and task-loss witnesses where a device is available; record explicit unavailable hardware separately. |
| MCI and command-table helpers | Original Win16 mmsystem owns the device/task records. The selected WOW32 dispatch table does not activate the historical structured MCI body. | Explicitly unavailable in this selected runtime; no provider is invented. | Select a real immutable caller before claiming MCI capability. |
| Task exit | Original `drvproc.c::AppExit` closes creator-task MCI devices and wave/MIDI/MMIO handles; `hdrvDestroy` suppresses callbacks during that rundown. | No project callback/buffer registry exists, so there is no independent state to leak or reclaim. | Verify real task loss against a completed device operation. |

S3 therefore closes the reached integration question: **there is no missing
project binding, scheduler, buffer owner or task-rundown substitute to add**.
It deliberately does not convert a private-export resolution result into a
claim that a physical device completed work.  Those immutable caller/device
observations are the next, separate S4 acceptance scope.
