# Selected DEM default-drive service: direct controlled object behavior

The [fixture](dem-default-drive-selected-fixture.c) directly invokes the current selected x86 `demgset.obj::demSetDefaultDrive` with controlled guest address/registers, `SetCurrentDirectoryOem` and `demClientError` boundaries. [Runner](dem-default-drive-selected-run.py) and [observation](dem-default-drive-selected-observation.json) preserve exact fixture, source, object and EXE hashes plus output; no production object or guest was modified. The selected object SHA-256 is `7B4172A148BCFA74D251617A3F3075628C41BEC005DE9776AA55C91CD5A20306`; the current `demgset.c` SHA is `1EF781412422CE7048FE2E40B0D8B067D4ACCCD83C35B8B29A7FB8A5D1551D7D`. The exact `demSetDefaultDrive` function text matches the pinned primary OpenNT function (SHA-256 `955542701836AE7B63D2B7E978A088524A9F2751C5732860A56CCAF9604A4F1C`); the full source files differ, so no whole-file identity is implied. The MinGW link succeeded with a `.drectve` warning.

| Controlled input | Observed selected-object result |
| --- | --- |
| `C:\\TMP`, DL=2, fake host success | Host callback once, CF=0, no error callback; AX unchanged. |
| `C:\\TMP`, DL=3 | No host/error callback, CF=1, AX=1. |
| `c:\\TMP`, DL=2 | Same mismatch result: the source compares the first byte case-sensitively. |
| `C:\\TMP`, DL=2, fake host failure with LastError=5 | Host callback once, mocked error callback receives drive `C` and sets CF=1, AX=5. |

The four assertions passed in the selected-object process. They confirm exact function behavior under controlled seams, not actual filesystem/current-directory mutation or original `demClientError` translation. The lowercase mismatch is an original-source behavior in this direct invocation, **not** an established guest bug: the retained original `dos/v86/doskrnl/dos/getset.asm::$SET_DEFAULT_DRIVE` takes its current direct `GetVisDrv`/`curdrv` path; the only `SVC_DEMSETDEFAULTDRIVE` occurrence there is commented within `if 0`. The original `demdisp.c` still lists this SVC, but table presence does not establish any active guest entry. Do not synthesize a guest failure or alter case semantics on the basis of this fixture. S12 retains separate current-directory/drive source, real guest, errors and lifecycle verification.

Replay from the repo root with the selected x86 worker object and bundled i686 MinGW:

```powershell
python artifacts/mvdm-audit/host-ch-scope/dem-default-drive-selected-run.py
```

Outputs are under `build/t473-s10/dem-default-drive`; no real directory is changed. This test is unrelated to prior blocked probes and does not rebuild or edit guest or product.
