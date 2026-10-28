# Test-owned DLL: loader lock and self-request controls

Noninteractive, isolated MSVC x86 host experiment. It does not load an original VDD, invoke an NTVDM API, use guest/driver/desktop, or exercise the product's actual BOP or IOS objects. Its test-owned DLL and host code are `vdd-loader-lock-probe-dll.c` and `vdd-loader-lock-probe-host.c`.

The first case prestarts a host worker blocked on an event. A test-owned DLL process-detach callback signals that worker and performs a **test-only 1-second bounded wait** for the worker to finish `LoadLibraryA("version.dll")` and `FreeLibrary`. Observed on this host: `DETACH_WAIT result=258 timeout=1 worker_after_detach=1`: the worker could finish after detach returned, but not while detach waited. This is direct evidence of a loader-dependent wait cycle in this controlled setup; it is not an assertion that all loader calls always block or that an actual VDD did this. The bounded wait is diagnostic only, not a proposed product timeout.

The second control loads a fresh copy, calls a test DLL callback which requests removal through a host callback, returns normally, and then has the host release its DLL reference. Output `SELF_REQUEST deferred_until_callback_return=1` and exit 0 proves only that this intentionally deferred toy arrangement can return safely; it does not establish that deferring a real VDD's public uninstall is semantically acceptable, handle nested callbacks, or use actual hook tables. No unsafe unload-while-executing test was run.

Both outputs strengthen the existing `vdd-loader-quiescence-review.md` boundary: an ordinary callback drain placed in DLL_PROCESS_DETACH can deadlock with a callback/worker that needs the loader; any proposed pre-unload protocol must be verified against the actual product loader and direct-I/O paths. The full actual-object test, callback self-uninstall behavior, failure-path cleanup and independent native `FreeLibrary` remain untested.

## Replay

Use the selected project x86 MSVC driver, with test-owned output directory:

```powershell
New-Item -ItemType Directory -Force build/t473-s10/vdd-loader-lock | Out-Null
cmd /c '"build\.ninja\msvc-x86-release\wow32\msvc-x86.cmd" cl /nologo /MT /LD artifacts\mvdm-audit\shard-3\vdd-loader-lock-probe-dll.c /Fo:build\t473-s10\vdd-loader-lock\dll.obj /Fe:build\t473-s10\vdd-loader-lock\probe.dll /link /IMPLIB:build\t473-s10\vdd-loader-lock\probe.lib'
cmd /c '"build\.ninja\msvc-x86-release\wow32\msvc-x86.cmd" cl /nologo /MT artifacts\mvdm-audit\shard-3\vdd-loader-lock-probe-host.c /Fo:build\t473-s10\vdd-loader-lock\host.obj /Fe:build\t473-s10\vdd-loader-lock\host.exe'
$dll=(Resolve-Path build/t473-s10/vdd-loader-lock/probe.dll).Path
& build/t473-s10/vdd-loader-lock/host.exe $dll
```

SHA-256: DLL source `6B4DB51619374E20D39C3B2D22766BCF515ED5942BEA991CF624A9F4EC491E23`; host source `C8757064BE11C42706C5A7439310E6CB8CD693FA082969D21BA5728320E9765F`; tested DLL `73C83C3576284B1AC35B68C601CC9F0654B4BF2FF125788B412ABC25807A3`; tested EXE `C0BB87794F6FBC765751AE63F4C9010E131CDB5634B8F92B3E382EA820BC7A2B`. The DLL's timed wait is intentionally not a product pattern. No production, guest or ledger changes were made.
