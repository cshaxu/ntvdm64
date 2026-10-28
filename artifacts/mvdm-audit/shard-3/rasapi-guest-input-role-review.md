# S10 shard 3: RASAPI16 guest interface and resource group

Four pending original guest inputs under `wow16/rasapi16` are examined with `rasapi-guest-input-observer.py` and JSON; each matches both original inventory hashes. Read-only analysis; no guest rebuild, RAS connection, provider execution or product modification.

| File | Consumer and file role |
|---|---|
| `bseerr.h` | Historical Lanman base error constants, included by `rasapi16.c:17`. Guest build/API declaration carrier. |
| `ras.h` | Win16 RAS types and API declarations, included by `rasapi16.c:19` and explicitly listed in makefile:71. Guest build interface; source-level layout does not prove installed module ABI. |
| `raserror.h` | RAS error constants included by `rasapi16.c:20` and declared makefile dependency; `MapErrorCode` implements four explicit 32→16 renumberings with pass-through for other values. Guest build interface and conversion context, not a real-provider error test. |
| `rasapi16.rc` | DLL version resource describing the 16→32 API thunk, compiled to `.res` and applied to RASAPI16.DLL by makefile:83-88. Guest build resource; exact packaged resource provenance is unproved. |

Complementing `shard-1/guest-ras-winspool-review.md`: original `rasapi16.c` requires `WF_WINNT`, calls `LoadLibraryEx32W("RASAPI32.DLL")`, looks up private `Ras*Wow` exports via `GetProcAddress32W`, passes native calls through `CallProc32W`, and unloads the module in WEP. E.g. missing `RasDialWow` leads to `ERROR_INVALID_FUNCTION` before its argument allocations. Thus actual behavior depends on guest/kernel generic-thunk correctness and actual host provider private exports, not merely header retention. No RAS provider presence, export compatibility, asynchronous notification, disconnect or task-loss behavior was tested here.

The approved guest manifest's `RASAPI16.DLL` SHA-256 `AF31D54E742B4C34F451D40C7405C3198D67A77808D41B559336AD0B9FD49CAD` matches the retained guest binary; `O:/winnt/system32/RASAPI16.DLL` was absent when checked. Source/recipe does not prove binary provenance or all guest search paths. Recommend grouping these four files as retained guest compile/resource interfaces, while the actual RAS provider availability and lifecycle remain pending; do not turn a file-role classification into feature closure.

Replay:

```powershell
$env:PYTHONDONTWRITEBYTECODE='1'; python artifacts/mvdm-audit/shard-3/rasapi-guest-input-observer.py
rg -n -i 'bseerr\.h|raserror\.h|ras\.h|LoadLibraryEx32W|GetProcAddress32W|CallProc32W|FreeLibrary32W|MapErrorCode' src/mvdm/wow16/rasapi16
Get-Content src/mvdm/wow16/rasapi16/makefile
Get-FileHash src/mvdm/wow16/rasapi16/RASAPI16.DLL -Algorithm SHA256
Test-Path O:/winnt/system32/RASAPI16.DLL
rg -n -i 'RASAPI16\.DLL' assets/winnt-guest-manifest.json
```
