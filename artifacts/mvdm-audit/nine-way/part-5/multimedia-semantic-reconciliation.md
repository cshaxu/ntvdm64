# Part 5: Win16 multimedia callback and original host-provider boundary

This supplements the per-key identity/build report `multimedia-external-review.md`; it does not imply the 15 external OpenNT paths are linked into the product. Two source-original boundaries are distinct: the immutable Win16 `mmsystem` packages a procedure call via original WOW32, while the installed x86 host WINMM owns its native endpoints and devices. Project WOW32 does not own a separate asynchronous queue.

## Execution and state ownership

Original `wow16/mmsystem/init.c::ThunkInit` resolves `NotifyCallbackData`, wave/MIDI/aux/MCI/timer/joystick message entrypoints from host WINMM. Original thunk assembly calls `MMCALLPROC32`; selected `wow32/wmmedia.c::WMM32CallProc32` preserves the five argument order. Its synchronous timer packet and actual bridge-body focused witness are documented in `docs/etc/evidence/m0-t466-s2-sync-multimedia-bridge.md`.

`src/mvdm/wow32/wmmedia.c:312-355` forwards `WOW32DriverCallback` to the dynamically looked-up `WINMM!WOW32DriverCallback`; `WOW32ResolveHandle` forwards multimedia alias resolution. The guest mmsystem retains original callback-ring publication and withdrawal, host WINMM owns host scheduling, and original `drvproc.c::AppExit` retains wave/MIDI/MMIO and MCI task rundown. The original disabled converters in `wmmedia1.c`/`wmmedia2.c` are not an independent active replacement: see `m0-t466-s3-async-multimedia-owner-audit.md`. Never assert callback completion solely from export lookup.

The source `GET_MULTIMEDIA_API` macro declines to call a missing target using the per-call error value; inspect `src/mvdm/wow32/wmmedia.h:180` for the exact convention before changing failure handling. Concurrent module-load, pending callback during task loss, queued buffer ownership and reset/close semantics are not established by this source observation. There is no justification here for importing an NT4 audio driver or activating globally disabled converter bodies.

## Per-file role and recommendation

Each of the following exact owned paths has independent hashes and recipe references in `multimedia-external-observations.json` and the 15-row prior report. Preserve as original comparison/dependency evidence; no release import is recommended absent a caller-proven missing behavior.

| Exact owned file | Original role / bound on current conclusion |
| --- | --- |
| `nt/private/windows/media/winmm/regapi.c` | NT4 WINMM registry API implementation; modern installed host supplies its own behavior; no claim it is linked. |
| `nt/private/windows/media/winmm/drvrrare.c` | NT4 WINMM rare driver services; host-owned implementation and failures need individual caller evidence. |
| `nt/private/windows/media/winmm/drvr.c` | NT4 driver service implementation; not a project callback owner. |
| `nt/private/windows/media/winmm/drvr.h` | NT4 private declarations; no permission to interpret current host private state as this layout. |
| `nt/private/windows/media/winmm/playsnd.c` | NT4 sound-play provider; individual guest play and failure path unverified. |
| `nt/private/windows/media/winmm/sources` | Historical provider recipe; not a current link recipe. |
| `nt/private/windows/media/drivers/sndsys32/dll/oemsetup.inf` | Historical specific audio device installation input; not proof current host has that hardware. |
| `nt/private/windows/media/drivers/sndsys32/dll/sources` | Historical device-driver recipe only. |
| `nt/private/windows/media/drivers/sndsys32/dll/sndsys32.def` | Historical driver exports; not current device-provider guarantee. |
| `nt/private/windows/media/drivers/sndsys32/dll/drvproc.c` | Historical device driver procedures; distinct from guest mmsystem AppExit. |
| `nt/private/windows/media/drivers/drvlib/sources` | Historical driver library recipe. |
| `nt/private/windows/media/drivers/drvlib/inc/drvlib.h` | Historical driver declarations, no current ABI assumed. |
| `nt/private/windows/media/drivers/drvlib/mixerdd.c` | Historical mixer support; not proof host mixer works. |
| `nt/private/windows/media/drivers/drvlib/drvlib.c` | Historical driver-library implementation, not direct WOW32 link. |
| `nt/private/windows/media/drivers/drvlib/drvutil.c` | Historical driver support routines, not direct WOW32 link. |

## Evidence and recovery routing

`m0-t466-s3-async-multimedia-owner-audit.md` reports a historical focused x86 witness resolving eleven endpoints and synchronously calling the timer; it explicitly did not exercise callback scheduling or audio devices. Part-5 existing observer independently binds a 104-byte published `WOW32DriverCallback` body with relocation masks; this is body identity, not a verified table edge. No test was executed in this review.

S14 should run an unchanged guest timer callback/cancel/teardown witness, then real device wave/MIDI open/failure/completion/cancel and task-loss witnesses when a device exists. Report absent hardware separately, and prove original guest-ring retirement before any refactoring. Resolve a specific real MCI caller before claiming its functionality. S19 verifies matching release evidence. S13 can own actual device-specific regressions; no host-driver import or fake callback is justified by the current evidence.

Reproduce: `rg -n "WOW32DriverCallback|SafeLoadLibrary|GetProcAddress|WOW32ResolveHandle" src/mvdm/wow32/wmmedia.c`; read `docs/etc/evidence/m0-t466-s1-multimedia-owner-matrix.md`, `m0-t466-s2-sync-multimedia-bridge.md` and `m0-t466-s3-async-multimedia-owner-audit.md`; replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/nine-way/part-5/multimedia-external-observer.py`. No source, guest, shared ledger or runtime was modified.
