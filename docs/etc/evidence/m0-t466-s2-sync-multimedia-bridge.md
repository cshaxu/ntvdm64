# M0 T466 S2 — synchronous multimedia bridge

## Source result

The selected original Win16 `mmsystem` does not call public modern WINMM
exports for its WOW timer path.  `timea.asm::timeGetTime` builds an original
`MMCALLPROC3216` packet and calls `MMCALLPROC32`; the selected original
`wow32/wmmedia.c::WMM32CallProc32` invokes its target in the fixed order
`(p5, p4, p3, p2, p1)`.  The resulting packet is exactly:

```
(uDevId=0, uMsg=0, dwInstance=0, dwParam1=0, dwParam2=0x000A)
```

`0x000A` is the selected `THUNK_TIMEGETTIME` constant.  It is synchronous
and has no guest/native buffer, alias, device, callback or completion
ownership.

The source first loads `winmm.dll` and asks original KERNEL
`GetProcAddress32` for the legacy `tid32Message` compatibility entry.  A
direct x86 runtime lookup proves that current SysWOW64 WINMM still exports
that entry.  The active original `WK32GetProcAddress32` remains intact; its
production compile routes native `GetProcAddress` through
`wow_extension_get_proc_address` only to preserve native-call scope, and that
wrapper returns the exact native address unchanged.

## Disposition

No host adaptation is necessary or admissible: inserting one would create a
second owner for an already available original private interface.  The native
WINMM module owns its ordinary loaded-module lifetime, and the selected
original mmsystem/WOW code owns packet construction and invocation.  The
production diff is therefore zero and no new `MVDM-HOST-DIV` is registered.

## Verification

`verify-wow-native-extension-boundaries.ps1` compiles the actual extension,
the bridge, and the original `wmmedia.c` body in its existing focused cases.
Its added witness proves that native WINMM exports `tid32Message`, the
existing wrapper returns the exact same address, and two exact calls are
monotonic.  A second witness writes that address and the exact packet back
through the complete selected original `WMM32CallProc32` body, proving the
same synchronous route without a new provider.  Existing scope, exception,
zero-target and native lookup checks remain required.

The current-source witness completed all four focused phases:

| Phase | Disposable root | Result |
| --- | --- | --- |
| Native extension lookup/synchronous call | `build/t466-s2-native-extension-r11` | `WOW_NATIVE_EXTENSION_BOUNDARY errors=0`; native and wrapped `tid32Message` addresses matched and two calls completed. |
| Original `WMM32CallProc32` body | `build/t466-s2-native-extension-r12` | `WOW_ORIGINAL_MM_EXTENSION errors=0`; the exact timer packet reached `tid32Message` through the selected original body. |
| Unavailable/rejection boundary | `build/t466-s2-native-extension-r13` | `WOW_PUBLIC_CALLBACK_REJECTION errors=0`; no guest entry occurred. |
| Original callback-frame preservation | `build/t466-s2-native-extension-r14` | callback snapshot/re-resolution, exception propagation and output preservation all passed. |

`git diff --check` and documentation governance also pass. Production source,
guest media, runtime images and release hashes are unchanged, so no new package
is published for this source/test-only delivery.

This evidence does not claim wave, MIDI, mixer, joystick, MCI, timers with
callbacks, device lifetime or asynchronous completion. Those capabilities
remain S3/S4 work.
