# S10 shard 3: multimedia alias consumer follow-up

Scope: graph-selected original `wow32/wmmalias.c` and `wmmedia2.c`, following `wow-multimedia-alias-audit.md`. No product or guest execution, rebuild, or shared ledger edits. Both files match both original source inventories; their SHA-256 values are respectively `D4949206F261D22F80F96E769388AEE94037B27092C69C68EC058619F843F4FB` and `4A2A6E5E6C6E7172CB3FEA4BB9035A18C7C5A292618B9EB507B71AE9F7D76856`.

## Consumer/provider result

`walias.h:260-282` defines the multimedia alias conversion macros. A complete textual search of `src/mvdm/wow32` for these macro names, excluding their definitions, finds **only wmmedia2.c**. That file's outer `#if 0` at line 39 extends to the final `#endif`, suppressing those MIDI/wave implementations. The selected WOW32 map contains no `GetMMedia16`, `GetMMedia32` or `FreeMMedia16` function entries; its `wmmalias.obj` contribution is a `FindHMap16` static local data item, which is not an active call. This supports **inactive original multimedia-alias consumer code in this observed graph**, not a claim that all multimedia handles, all generic alias helpers, generated callers or other configurations are unused.

The selected alternative is `wmmtbl2.h:18` -> `WMM32CallProc32` -> original `wmmedia.c` invokes the supplied native function with five arguments. The current selected map lists both `WMM32CallProc32` in `wow_user_native_extension.obj` and `wow_original_WMM32CallProc32` in `wmmedia.obj`; `src/wow32-dll/source/wow_user_native_extension.c:63-83` wraps the original with execution-boundary entry/exit. This is one bridge path, not evidence that arbitrary WinMM operations are compatible. `docs/etc/evidence/m0-t466-s2-sync-multimedia-bridge.md` witnesses a specific timer packet; its owner matrix explicitly leaves other operations and callback/teardown behavior open.

Proposed disposition: both retained original files may be classified as **historical inactive multimedia conversion implementation in the observed profile**, with actual multimedia capability, handle lifecycle and callback semantics still pending under the active guest/host bridge. Do not remove code or reactivate the old thunks based on this finding. A DLL map cannot alone establish byte identity of the deployed DLL or runtime call behavior.

Reproduce from repo root:

```powershell
Get-FileHash src/mvdm/wow32/wmmalias.c,src/mvdm/wow32/wmmedia2.c -Algorithm SHA256
rg -l '\b(HDRVR32|GETHDRVR16|FREEHDRVR16|HMMIO32|GETHMMIO16|FREEHMMIO16|HMIDIIN32|GETHMIDIIN16|FREEHMIDIIN16|HMIDIOUT32|GETHMIDIOUT16|FREEHMIDIOUT16|HWAVEIN32|GETHWAVEIN16|FREEHWAVEIN16|HWAVEOUT32|GETHWAVEOUT16|FREEHWAVEOUT16)\b' src/mvdm/wow32 -g '!walias.h'
rg -n -i 'wmmalias.obj|GetMMedia|FreeMMedia|WMM32CallProc32' build/.ninja/msvc-x86-release/wow32/wow32.dll.map
rg -n 'WMM32CallProc32|wow_original_WMM32CallProc32' src/mvdm/wow32/wmmtbl2.h src/mvdm/wow32/wmmedia.c src/wow32-dll/source/wow_user_native_extension.c
```
