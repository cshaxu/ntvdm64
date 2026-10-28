# S10 shard 3: Win16 kernel bootstrap and build carriers

Seven pending `wow16/kernel31` paths, reviewed read-only as a kernel build/input family. `kernel-boot-build-input-observer.py` / JSON captures source inventory hashes and installed image byte relationship. No guest rebuild, execution, or modification.

| File | Evidence and bounded role |
|---|---|
| `debug3/kernstub.exe` | OpenNT-4.5-only inventory, SHA-256 `4BEF21A8E6AEC527BDFFEA9DAA75F600B83B8E0F19266DC1B28CF16BA8260D6D`, identical to retained retail3 stub. `kernel.def` names `STUB 'KERNSTUB.EXE'`; makefile compiles and links kernstub for kernel target. Its 209-byte MZ payload after its 512-byte header matches bytes beginning at offset 512 in installed KRNL386.EXE; installed image SHA `D404576899CC11D23F368F2B30554E6108657CD37809837B341FD37E6B1A6AE892FC8A06BC58` matches guest manifest. This proves byte correspondence, not that this particular debug file generated the installed binary or all boot paths were executed. |
| `kernel.api` | Included by `layer.asm`; makefile includes it in kernel layer dependencies. Original message/API validation declarations, guest build interface; do not assume all validation works. |
| `newexe.h` | MZ/NE executable format declarations; `kernel31/rip.c` includes it and original makefile lists `rip.obj` dependency. Guest build interface; no conclusion on overall NE loading or all error paths. |
| `m.msg` | Zero-byte original file; no consumer established in inspected search. Candidate empty historical artifact, still pending until its production use/role is confirmed. |
| `patch.txt` | Historical module-patch explanatory text, not a build rule or current authorization. Documentation role only; patching behavior remains an independent source/runtime question. |
| `qgrep.grp` | Historical symbol/filter data named in `kernel.sym`, `krnl286.sym`, `krnl386.sym` dependencies; observed commands invoke MAPSYM. Build/diagnostic data role; whether every historical MAPSYM consumes this file directly is not proved. |
| `state.rst` | Historical editor/debug state file containing a source path and cursor position; no runtime consumer demonstrated. Candidate workspace/debug artifact; preserve source but no feature-restoration claim. |

Per-file original edition hashes are in the observer JSON, including the 4.5-only stub distinction. The known installed kernel payload does not prove source-to-binary build lineage or protected-mode, fault/return, guest task lifecycle or guest API behavior. Exact file-role exclusions must not remove those obligations.

Replay:

```powershell
$env:PYTHONDONTWRITEBYTECODE='1'; python artifacts/mvdm-audit/shard-3/kernel-boot-build-input-observer.py
rg -n -i 'STUB|kernstub|kernel\.api|newexe\.h|qgrep\.grp' src/mvdm/wow16/kernel31 -g 'makefile' -g '*.def' -g '*.asm' -g '*.c'
rg -n -i 'KRNL386\.EXE' assets/winnt-guest-manifest.json
```
