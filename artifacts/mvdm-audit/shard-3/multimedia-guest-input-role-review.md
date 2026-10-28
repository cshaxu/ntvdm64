# S10 shard 3: Win16 multimedia guest-build input group

Eight pending files under `wow16/mciole` and `wow16/mmsystem`. `multimedia-guest-input-observer.py` and JSON preserve current and frozen original SHA-256 identities; this is read-only, with no guest build, playback or product change. Each recommendation concerns a file role only, not multimedia feature closure.

| Path | Historical source/build chain and proposed role |
|---|---|
| `mciole/MCIOLE16.RES` | `mciole/makefile:62-74` links MCIOLE16.DLL then applies its `.res` via RC; guest build resource. Archive manifest contains MCIOLE16.DLL, but no proof this retained RES was used to produce it. |
| `mciole/ole.h` | `mciole.c:70` includes this OLE definitions header; `mciole.obj` is in `OBJ1` of that DLL recipe. Guest compile interface, not evidence MCI/OLE interaction works. |
| `mmsystem/int31.inc` | Included by `dpmipage.asm`, explicitly a `dpmipage.obj` dependency; selected object is in MMSYSTEM makefile `OBJ4`. Guest DPMI declarations, not proof protected-mode calls work. |
| `mmsystem/mixmgri.h` | Included by `mixer.c`; `mixer.obj` is in MMSYSTEM `OBJ5`. Guest mixer internal API input; playback/device provider behavior remains pending. |
| `mmsystem/mmiocf.c` | Historical compound-file source; `mmioCFOpen` and `mmioCFAccess` visibly contain TODO/NULL bodies, with other methods also unfinished. The inspected MMSYSTEM object list does **not** include mmiocf.obj. Treat as inactive incomplete original source in this recipe, not a working implementation and not a general claim that all MMIO/RIFF is absent. |
| `mmsystem/mmiocf.h` | Its source consumer is `mmiocf.c`; declares the compound-file interface. Same inactive-source limitation; other historical builds/export consumers not established. |
| `mmsystem/mmsex.dlg` | Included by `mmsystem.rc` and `mmsex.c`; MMSYSTEM recipe includes `mmsex.obj` only in debug branch. Resource inclusion and code selection are distinct: verify original RC conditional and selected language before claiming a shipped dialog. |
| `mmsystem/mmtask/MMTASK.RES` | MMTASK makefile constructs MMTASK.TSK and has language-dependent resource compilation/copy and RC attachment. The retained .RES is a historical resource artifact; selected language and exact source-to-shipped image identity not established. |

MMSYSTEM.DLL and MCIOLE16.DLL are named in approved guest manifest, but binary presence or recipe edges cannot establish execution of individual guest source files or proof of resource lineage. The separate `multimedia-guest-abi-role-review.md` and `wow-multimedia-alias-consumer-followup.md` retain actual host bridge, wave/MIDI/MCI, callback and task teardown obligations. Do not infer behavior percentages or eliminate original guest features merely because the host Ninja graph does not compile guest sources.

Replay:

```powershell
$env:PYTHONDONTWRITEBYTECODE='1'; python artifacts/mvdm-audit/shard-3/multimedia-guest-input-observer.py
rg -n -i 'mmiocf|mixmgri|mixer\.obj|dpmipage|int31\.inc|mmsex|mmtask' src/mvdm/wow16/mmsystem
Get-Content src/mvdm/wow16/mmsystem/makefile -TotalCount 95
Get-Content src/mvdm/wow16/mmsystem/mmtask/makefile
Get-Content src/mvdm/wow16/mciole/makefile
rg -n -i 'mmsystem\.dll|mciole16\.dll' assets/winnt-guest-manifest.json
```
