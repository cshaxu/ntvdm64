# S10 shard 3: Win16 debug declaration family

Three pending `wow16/inc` inputs examined via `wow-debug-input-observer.py` / JSON: current bytes match both original inventory hashes. Read-only; no guest build, debugger run, or product/shared-ledger edit.

| Path | Concrete source ownership and bounded recommendation |
|---|---|
| `debug.h` | Declares debugger-facing `QuerySendMessage`, `LockInput`, `GetSystemDebugState` and flags. USER historical recipe selects `user/debug.c`; that file protects its `QuerySendMessage`/`LockInput` bodies with `#ifndef WOW`, while `GetSystemDebugState` remains and only the no-task-queue flag is checked in the WOW branch. Original WOW32 table marks the first two `UNIMPLEMENTEDAPI`, the third `LOCALAPI`. Header is a **guest interface carrier**, not proof those APIs are operational. Many same-spelled `#include "debug.h"` calls resolve first to directory-local `write/debug.h` or `mmsystem/debug.h`; do not attribute them automatically to this header. |
| `logerror.h` | Defines `LogError`/`LogParamError` declarations/constants; concrete original includes occur in `kernel31/error.c`, `kernel31/rip.c`, `mmsystem/mmsysi.h` and Sherlock `error.c`. Kernel makefile explicitly compiles error/rip, but debug conditionals vary; include-edge and source identity do not establish deployed guest execution. Retained **guest declaration/build interface**. |
| `debugsys.inc` | Historical VMM debug equates/structures. No exact `include debugsys.inc` found in bounded WOW16 source search; no verified selected consumer or built object. Keep **pending**, rather than extrapolate from the absence of a host Ninja dependency or exclude all debugger features. |

The original local/UNIMPLEMENTED entries and selected build source do not prove the installed USER image's source lineage, task queue behavior or debugger semantics. Earlier T470 ToolHelp negative evidence has its own narrowly stated guest/provider limits; it is not a test of these declarations. No numerical feature restoration claim follows.

Replay:

```powershell
$env:PYTHONDONTWRITEBYTECODE='1'; python artifacts/mvdm-audit/shard-3/wow-debug-input-observer.py
rg -n 'QuerySendMessage|LockInput|GetSystemDebugState|#ifndef WOW' src/mvdm/wow16/user/debug.c src/mvdm/wow16/user/user.def src/mvdm/wow32/wutbl2.h
rg -n -i 'logerror\.h|debugsys\.inc' src/mvdm/wow16
Get-ChildItem src/mvdm/wow16/write/debug.h,src/mvdm/wow16/mmsystem/debug.h,src/mvdm/wow16/inc/debug.h
```
