# Shard 2: Win16 timer guest build and package identity

Six pending files were compared with both original source trees and the frozen catalog. The observer and its JSON record replayable SHA-256 and path evidence. No guest or product was executed, changed or built.

| File | Evidence and limited recommendation |
| --- | --- |
| `libinit.asm`, `local.asm`, `math.asm` | Identical to both originals. Historical makefile explicitly lists their corresponding objects in the TIMER driver link. Candidate exclusions as immutable guest build-source files only. |
| `timer.inc` | Identical to both originals; included by each of those three sources. Candidate exclusion as immutable guest build input only. |
| `sysinfo.inc` | Identical to both originals; included by `libinit.asm`. Candidate exclusion as immutable guest build input only. |
| `TIMER.DRV` | Identical to both originals and `WINNT/SYSTEM32/TIMER.DRV` in `assets/winnt.zip`. Candidate partial, based on package identity, not runtime behavior. The direct installed path `O:/winnt/system32/TIMER.DRV` was absent at inspection; absence alone does not prove that a particular guest cannot resolve an alternative path. |

The matching historical makefile calls `link16` and uses objects and `timer.def`; the latter exports `WEP` and `DriverProc`. These references demonstrate original build intent, not that the shipped image was produced by this recipe. File identity is not proof that the module loads, nor proof of timer operation, callbacks, resolution, cancellation or teardown. No runtime results are asserted. The previous `guest-loader-location-audit.md` records additional search-path and optional-module limits. Do not interpret build-source exclusion as removing timer capability or a conclusion that the implementation is fully restored.

Replay with `python artifacts/mvdm-audit/shard-2/win16-timer-guest-observer.py`; compare the result with the companion JSON. This observer reads source, catalogs, the package and installed path only; it does not use the historical clean target.
