# T95 S6 Narrow Companion-Link Observation 001

## Procedure

The r2 root was built exactly once with:

```text
nmake /f ntdos64-native-adapter-observation.mak ntdos64-native-adapter-observation.exe
```

It was derived from r4's source-built root, with a build-local opt-in `main.o`
and exactly nine copied narrow adapter/CLI C sources. The root manifest records
the r4 container hash, current `main.cc` hash, all nine source hashes and shim
hash. A preceding `nmake /n` emitted the same compilation/link command shape
without executing it.

The outer PowerShell observation process exceeded its 63-second transport
timeout while waiting, but it did not leave a compiler, linker, nmake or cmd
process. The root-local wrapper completed and wrote exit code `0`; the output
and map exist. This is an observer-collection limitation, not a second build
or an inferred success.

## Result

| Item | Value |
| --- | --- |
| Exit code from root-local wrapper | `0` |
| Executable | `ntdos64-native-adapter-observation.exe`, 2,494,976 bytes, SHA-256 `1B7283123409720FB0700BCCC526E9563E54A5C089A70C33F45BD398E113CBED` |
| Link map | `ntdos64-native-adapter-observation.map`, 2,474,650 bytes, SHA-256 `127C92740A0CBB61895CB32BD7CF443C730C069428CFC9BAADFB5B0AB5072E08` |
| Link log | SHA-256 `BC41B9C6BC00CA42C88F72C381B3AD97DD25547FDD9FE73373F033245EAE3ED3` |
| Adapter map definitions | `bx_ntvdm_startup_session_v1_prepare_from_environment`, `bx_ntvdm_startup_session_v1_prepare_ntio_plan`, and `byob_image_load_exact` are linked from the declared narrow objects. |
| Negative map/log result | No `bx_ntvdm_adapter_runtime`, `bx_ntvdm_host_session`, DEM/BOP or OpenNT symbol appears. |

## Interpretation

The explicit same-island companion link is available. It is a new executable,
not a modification to the default r4 target. Its only new composition closure
is the nine-object startup-session family plus declared Win32 libraries.

This proves neither process startup nor a guest transition. The immediate next
gate is a no-environment run of this exact companion against the accepted PC
reset-floor profile. It must demonstrate that absence of a CLI child
environment takes the original no-plan path before any synthetic or source-
built NTIO input is admitted.
