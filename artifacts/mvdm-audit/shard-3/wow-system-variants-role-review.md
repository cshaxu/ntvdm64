# S10 shard 3: Win16 SYSTEM driver and historical platform variants

Five pending paths under `wow16/system`, reviewed as a guest driver/build family. `wow-system-variants-observer.py` / JSON captures per-file hashes against both original inventories and the installed SYSTEM.DRV hash. No guest or product build/execution/change.

| File | Original chain and proposed role |
|---|---|
| `envect.asm` | HP Vectra EX-BIOS/HIL vector initialization, assembled as `envect.obj` by historical `hpsystem` recipe. It is not in the inspected standard WOW `makefile`'s `system.lrf` object list (`system.obj timer.obj`). Alternate hardware variant source; no conclusion about all hardware or timer functionality. |
| `hpsystem` | Despite its extensionless name, this is a historical makefile: target hpsystem.drv composes `hpsystem.obj timer.obj envect.obj` and `hpsystem.def`. Historical alternate driver build description, not a runnable product image. |
| `hpsystem.def` | HP Vectra SYSTEM driver description/import/export declaration, consumed by that alternate `hpsystem.lnk`. Build interface for that variant; export name similarity does not establish semantic interchangeability. |
| `romsys` | Historical makefile selecting MASM `-DROM=1` and a ROM system driver build. Its dependency says `romsys.def`, while the explicit link command spells `system.def`; preserve this inconsistency as historical recipe observation rather than inventing intended execution. |
| `system.drv` | Retained binary SHA-256 `32D88D68FD5DAE0F37766DB04742CAA912C9A4C7193D15A1E99E617FF6F794A3` exactly matches installed system32 driver and approved guest manifest. Partial installed guest identity, not proof this standard recipe built it or all timer/FPU calls work. |

The regular WOW `makefile` links system + timer and applies the resource, unlike HP's envect build; `system.asm` has `HPSYSTEM`-conditional paths. Recommend classifying the first four by historical alternate build/source role, and SYSTEM.DRV as partial guest image identity. Do not confuse alternate-source file exclusion with removal of SYSTEM timer functionality. Actual hardware detection, timer/exception semantics, source-to-binary provenance and alternate builds are untested.

Replay:

```powershell
$env:PYTHONDONTWRITEBYTECODE='1'; python artifacts/mvdm-audit/shard-3/wow-system-variants-observer.py
Get-Content src/mvdm/wow16/system/makefile
Get-Content src/mvdm/wow16/system/hpsystem
Get-Content src/mvdm/wow16/system/romsys
rg -n -i 'envect|hpsystem|romsys|HPSYSTEM' src/mvdm/wow16/system
rg -n -i 'SYSTEM\.DRV' assets/winnt-guest-manifest.json
```
