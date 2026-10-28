# Shard 2: Win16 OLE client guest inputs

Three shard entries match both original trees and the frozen catalog. The accompanying observer records identities and checks historical source/build edges without executing a guest or build recipe.

| Entry | Evidence and recommendation |
| --- | --- |
| `utils.c` | Original makefile includes `utils.obj` in OLECLI's link list and has the C-to-object inference rule; `bm.c` calls the `PutStrWithLen` utility declared in `dll.h`. Candidate exclusion as an immutable historical guest build-source file only. |
| `mk.cmd` | Original script selects debug/retail environment and invokes `nmake`. Candidate exclusion as a historical build script only; it was not executed. Its particular branches do not prove how any shipped image was produced. |
| `funchead.c` | Inspected file consists of a commented LibMain function header and has no function implementation. Candidate exclusion as a historical documentation/template file only; no conclusion about the actual DLL entry point. |

`src/mvdm/bin86/olecli.dll`, ZIP `WINNT/SYSTEM32/OLECLI.DLL`, and `O:/winnt/system32/OLECLI.DLL` have the same SHA-256, `F3A76752B6A257056EC43CD6D43DA5A65244FC5C41F89E2162EF27634ECE54B4`. Identity does not establish that the reviewed source produced this binary, which module a particular application loaded, or OLE stream, clipboard, object lifetime and DDE behavior. Original guest functionality remains independently in scope.

Replay `python artifacts/mvdm-audit/shard-2/win16-oleclient-guest-observer.py` and compare the companion JSON. No product, guest, installed files, shared ledger or build tree was modified.
