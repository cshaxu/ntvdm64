# S10 shard 3: TOOLHELP guest build and query boundary

Four pending `wow16/toolhelp` files, with hash comparison in `toolhelp-input-observer.py` and captured JSON. All four current bytes match both original inventory hashes. Read-only review; no guest rebuild or new execution.

| File | Source/build role and disposition |
|---|---|
| `class1.c` | Historical local `ClassFirst`/`ClassNext` implementation. `toolhelp/makefile:115-119` explicitly says it is kept as reference and not built for WOW: `ththunks.asm` forwards those functions instead. `wthtbl2.h:15-16` registers both as original `UNIMPLEMENTEDAPI`; this establishes the selected source-table policy, not success of class enumeration. Historical alternate implementation file, not evidence the feature works. |
| `toolhelp.h` | Declares guest TOOLHELP structs and API, included by `toolpriv.h`; the makefile lists it for numerous actual guest objects. Retained guest API/build interface. Its fields and per-function behaviors are not certified by identity or include edges. |
| `string.h` | Retained C-runtime string declarations listed as build dependency for global/module/notify/stack/task source units, and included as `<string.h>` by those files. Actual compiler header search resolution is unverified; do not equate this header to a runtime string implementation. |
| `TOOLHELP.RES` | Retained historical resource artifact. The inspected WOW makefile link recipe at line 164 invokes `rc16 -t -30 toolhelp.rcv toolhelp.dll`, rather than naming TOOLHELP.RES; another build/resource route is not ruled out. File is a historical resource candidate, with production-image contribution unresolved. |

Approved guest manifest lists `TOOLHELP.DLL` SHA-256 `DDEECCC103F7E3C105602A6F791792BAA7778A796709BE63983A8A921DF80CFE` equal to retained `wow16/toolhelp/TOOLHELP.DLL`; `O:/winnt/system32/TOOLHELP.DLL` is absent at this observation. This is guest archive identity, not a proof the inspected sources built it or are loaded through every search path.

Existing T470 S2 (`m0-t470-s2-toolhelp-query-disposition.md`) separately records a real guest negative witness using two supplied immutable ToolHelp images: `TaskFirst` and valid `ModuleFirst` returned zero, and `ClassFirst` returned original WOW unimplemented zero. The report attributes inability to establish a usable query provider to guest KERNEL/ToolHelp private-layout incompatibility but does not identify a specific faulty field; it states class failure is independently original. Do not misrepresent this as a new test, an all-ToolHelp failure, or proof that other methods succeed. File-role classification must not remove the corresponding compatibility limitation or later recovery obligation.

Replay:

```powershell
$env:PYTHONDONTWRITEBYTECODE='1'; python artifacts/mvdm-audit/shard-3/toolhelp-input-observer.py
Get-Content src/mvdm/wow16/toolhelp/makefile
rg -n 'ClassFirst|ClassNext|UNIMPLEMENTEDAPI' src/mvdm/wow16/toolhelp/ththunks.asm src/mvdm/wow32/wthtbl2.h
rg -n -i 'TOOLHELP\.DLL' assets/winnt-guest-manifest.json
Test-Path O:/winnt/system32/TOOLHELP.DLL
Get-Content docs/etc/evidence/m0-t470-s2-toolhelp-query-disposition.md
```
