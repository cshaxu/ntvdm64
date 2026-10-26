# M0 T466 S1 — WOW32 multimedia owner matrix

## Result

The selected production bridge is substantially narrower than the historical
`wmmedia*` source corpus.  The current source is byte-identical to the pinned
OpenNT comparison source for `wmmedia.c`, `wmmedia1.c`, `wmmedia2.c`,
`wmmstruc.c`, `wmmstru1.c`, `wmmstru2.c`, `wmmalias.c`, `wmmedia.h`, and
`wmmtbl2.h`.  The latter intentionally says “Gutted 21-Feb-1995”: its only
registered multimedia entry is `MMCallProc32` → `WMM32CallProc32`.

That means neither a symbol list nor the disabled historical thunk bodies
admit an implementation.  The active path is an original Win16 `mmsystem`
caller passing a 32-bit driver procedure plus five `DWORD` arguments through
the registered bridge.  The bridge optionally synchronizes the DOS directory,
then invokes that procedure.  It owns no device, queue, callback schedule, or
guest memory allocation.

## Active family ledger

| Family | Actual original caller and host boundary | Authority and lifetime | S1 disposition / executable evidence |
| --- | --- | --- | --- |
| Generic driver message: wave, MIDI, aux, joystick, timer | `wow16/mmsystem/thunks.asm` and `stack.asm` call `MMCALLPROC32`; `wmmtbl2.h` registers `WMM32CallProc32`; the latter calls the original supplied 32-bit function with five arguments. | The Win16 mmsystem driver/handle owns its guest structures.  The target WinMM endpoint owns only its native call.  No bridge allocation persists after return. | Existing `verify-wow-native-extension-boundaries.ps1` compiles the complete selected `wmmedia.c` body and proves five-argument order, optional directory order, zero-target refusal and exception scope.  S2 must add real guest synchronous operation/error witnesses. |
| Native multimedia handle conversion | `WOW32ResolveHandle` delegates wave/MIDI handle mapping to `WINMM!WOW32ResolveMultiMediaHandle`; USER/GDI handles retain their existing owners. | `wmmalias.c::hiMMedia` is the sole original multimedia alias table, guarded by `mmHandleCriticalSection`; `GetMMedia16`, `GetMMedia32`, and `FreeMMedia16` own lookup/retirement. | Active original code, but no real guest/open-close or handle-reuse witness yet.  S2 owns synchronous handle and buffer lifecycle; no parallel mapping table is admitted. |
| Native callback handoff | `WOW32DriverCallback` forwards to `WINMM!WOW32DriverCallback`. | WINMM owns callback scheduling; the existing WOW callback/task execution contract owns admission and task-loss refusal.  The bridge has no callback queue. | Red gate: S3 must prove completion, cancellation, reentry, task loss, queued buffer retirement and failed-open cleanup before any asynchronous feature is declared. |
| MCI and command-table helpers | Historical `wmmedia*.c` declares `mci*` and private `mmAPI*` targets; the Win16 mmsystem source has MCI callers. | The original MCI device/command owner is not active through `wmmtbl2.h` in this selected build. | Do not revive the inactive body or invent a provider.  S2 first proves a reached `MMCallProc32` MCI target and its public boundary; otherwise retain it as unavailable. |
| Wave/MIDI/aux/mixer structured thunk bodies | `wmmedia1.c`, `wmmedia2.c`, `wmmstruc.c`, `wmmstru1.c`, and `wmmstru2.c`. | All bodies are under original global `#if 0`; they have no registered active dispatch entry. | Explicitly excluded from direct activation.  A future caller-proven operation may recover only its minimum exact original body after its callback/buffer cleanup closure is designed. |
| Public host surface/device availability | `wmmedia.c::Get_MultiMedia_ProcAddress` lazily loads `WINMM.DLL`; the active bridge is x86. | The installed x86 `WINMM.DLL` owns device enumeration and returns zero when hardware is absent. | `verify-wow32-dynamic-host-surface.ps1` found all 78 original named exports. `verify-wow32-multimedia-feasibility.ps1` calls only public enumeration, timer capability and MCI error-text endpoints; zero device counts are accepted evidence of absence, not success. |

The current public witness (`build/t466-s1-feasibility-r8`) reports one wave
output, two wave inputs, one MIDI output, three mixers, sixteen joystick
slots, and a 1--1,000,000 timer period range.  Those values are environment
facts only; they do not establish that a Win16 caller can open, buffer, or
receive a completion from any device.

## State and boundary rules

- `mmHandleCriticalSection` protects the one original multimedia alias table;
  it cannot become a provider registry or callback scheduler.
- The guest's `MMCALLPROC3216` frame is borrow-only for the immediate original
  bridge call.  No native pointer, handle or completion object may be retained
  after that call unless a later admitted family proves its original owner and
  retirement path.
- `WINMM.DLL` is a public modern-host service, not an NT4 private layout.
  Its dynamic exports being present proves loader feasibility only.
- No guest source, driver, media or guest binary was modified or rebuilt.

## S2/S3 finite handoff

S2 may take only a caller-proven synchronous family that has: a real Win16
caller, exact `MMCALLPROC3216` target/argument layout, public x86 host result,
and normal/error/alias-buffer release proof.  It may not enable a directory of
disabled functions.  S3 owns every completion/cancel callback, queued buffer,
device close and task-rundown condition.  Device absence remains an explicit
result path rather than a fake device.

## Evidence limits

The S1 checks prove source selection, active bridge ordering and current host
surface/feasibility.  They do not prove real sound playback/recording, MIDI,
MCI, mixer behavior, callback delivery or task/device cleanup.  Those remain
unpassed S2/S3 obligations.
