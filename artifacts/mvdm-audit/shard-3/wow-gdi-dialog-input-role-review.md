# S10 shard 3: COMMDLG and GDI guest inputs

Four pending original Win16 files are examined with `wow-gdi-dialog-input-observer.py` / JSON; each matches both original inventories. No guest build, product modification or new dialog/metafile test. The installed COMMDLG.DLL and GDI.EXE hashes match their retained binaries and approved guest manifest, but not a proof that these source inputs produced them.

| Path | Specific source/build evidence and proposed role |
|---|---|
| `commdlg/dlgs.h` | Dialog control IDs; explicitly included in `commdlg.rc` and `privcomd.h`, and listed in `dlgs.obj`, `parse.obj`, `font.obj` make dependencies. **Historical guest compile/resource interface**, not a host executable or proof all dialog semantics work. The earlier limited search of `parse.c` alone missed the RC and shared-header consumers. |
| `commdlg/smfldr5.bmp` | Retained historical bitmap; bounded text search of COMMDLG source/resource and makefile found no reference. **Keep pending** until locale/generated resource alternatives and possible unsearched consumer are resolved; no claim it is unused or a shipped resource. |
| `gdi/GDI.RES` | Historical resource artifact. GDI makefile:120-124 compiles `gdi.res` from RC/RCV and applies it with `rc16 -t` to GDI.EXE. **Guest build resource role**; the retained artifact-to-installed binary relation is not proven. |
| `gdi/metarec.c` | Historical local metafile recorder source; inspected WOW GDI recipe links gdi/layer/muldiv/fontres/sort objects, not metarec.obj. `wgtbl2.h` maps CreateMetaFile/CloseMetaFile to original WOW32 `wgmeta.c`, which uses native GDI. **Alternate source role in inspected guest recipe**, not evidence metafile feature is absent or fully restored; other local APIs and provider ownership are separate. |

Related evidence `m0-t456-s1-font-text-metafile-owner-matrix.md` traces guest metafile allocation, native HMETAFILE, callbacks and cleanup, and assigns behavioral acceptance to later work. T468 records bounded common-dialog printing acceptance. Neither is a substitute for source-to-image provenance, all metafile formats, error/teardown, all common dialog templates or resource-image membership. Preserve these obligations and keep `smfldr5.bmp` pending.

Replay:

```powershell
$env:PYTHONDONTWRITEBYTECODE='1'; python artifacts/mvdm-audit/shard-3/wow-gdi-dialog-input-observer.py
rg -n -i 'smfldr5|dlgs\.h' src/mvdm/wow16/commdlg
Get-Content src/mvdm/wow16/commdlg/makefile
Get-Content src/mvdm/wow16/gdi/makefile
rg -n 'WG32CreateMetaFile|WG32CloseMetaFile' src/mvdm/wow32/wgtbl2.h src/mvdm/wow32/wgmeta.c
rg -n -i 'COMMDLG\.DLL|GDI\.EXE' assets/winnt-guest-manifest.json
```
