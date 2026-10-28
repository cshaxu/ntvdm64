# Shard 1: pending MMSYSTEM legacy driver and debug icon inputs

The three scoped files are `待核验` in the current shard ledger and match both frozen OpenNT editions. The observer rechecks their original recipe/source anchors, hashes and normalized VALID dependencies in four checked x86 worker/WOW32 and x64 frontend/service Ninja graphs. None of their exact paths appears in those host dependency logs; this does not prove absence from a guest build.

| Pending file | Original role and selection evidence |
| --- | --- |
| `wow16/mmsystem/drvrrare.c` | Original less-common installable driver source: it dynamically resolves Win3.1 USER driver entry points and contains fallback code. Its object is not in the checked MMSYSTEM object list. That list instead selects `drvr31.obj`; the original `drvr31.asm` source says it forwards installable-driver APIs to USER and explicitly references `OpenDriver` and `CloseDriver`. This does not prove the two implementations are equivalent or that all driver APIs work. |
| `wow16/mmsystem/drvrstr.c` | Original installable-driver string declaration defining `szDriverProc`, referenced by `drvrrare.c`; its object is also absent from the checked object list. Current selected `init.c` separately defines a same-named string; source-level naming is not proof the omitted object is linked. |
| `wow16/mmsystem/mmsex.ico` | Debug-dialog icon: original `mmsystem.rc` includes `mmsex.dlg` only under `#ifdef DEBUG`, and that dialog names `MMSEX.ICO` under `RC_INVOKED`. The makefile's debug resource command defines DEBUG. This is conditional historical resource-input evidence, not proof a release DLL contains the icon. |

The source-tree `MMSYSTEM.DLL` and `WINNT/SYSTEM32/MMSYSTEM.DLL` inside `assets/winnt.zip` match SHA-256 `080432D0CAD660B163D0D231D37603010F7D9366F58570AC7C74D171C518211A`. The exact `O:/winnt/system32/MMSYSTEM.DLL` path was absent at observation. Equality does not establish which sources/resources built it, whether the icon is in it, or where another guest obtains it. The separately audited `MMSYSTEM.RES` is not reclassified here.

These files are candidates for historical guest-source or debug-resource roles only in the checked recipe and host graphs. Do not exclude multimedia, installable-driver, callbacks or lifecycle capabilities. Replay `python artifacts/mvdm-audit/shard-1/guest-mmsystem-legacy-observer.py`; JSON records identities and checks. No compiler, product, guest or multimedia provider ran; no guest, product, build tree or shared ledger changed.
