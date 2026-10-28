# S10 shard 3: tools16 build inputs

Eight pending files are absent at the *mirror path*, but all eight exist under `tools/historical/opennt-4.5/` and match the OpenNT 4.5 inventory and that directory's manifest byte for byte. See `tools16-inventory-observations.json`; regenerate with `python artifacts/mvdm-audit/shard-3/tools16-inventory-observer.py` (set `PYTHONDONTWRITEBYTECODE=1`). This distinction prevents treating retained historical build tools as lost or as product runtime components. No tool or guest was executed or built.

| Inputs | Evidence and disposition |
|---|---|
| `c13216.exe`, `c33216.exe` | Historical compiler components retained exactly; `wow16/kernel31/makefile:185` invokes `cl16` to compile the kernel. No static reference here proves which subprocesses that driver invokes or that the current host Ninja build needs them. **Historical build-tool role; not runtime; exact driver chain not verified.** |
| `cl.def` | Historical default Windows application module-definition template, its own contents specify `EXETYPE WINDOWS` and segment/stack defaults. Exact copy retained; no proof that current product build consumes it. **Historical template, not runtime.** |
| `cvpack16.exe`, `cvpack.err` | Retained historical debug packing tool and error file; `wow16/kernel31/makefile:180` declares `CVPACK = cvpack`. Name mismatch means we do not claim this specific executable is actually selected under that alias or that error-file lookup works. **Historical tooling; actual invocation and diagnostic lookup unverified.** |
| `getsize.exe` | Exact historical copy retained. `dos/v86/doskrnl/dos/makedos.cmd:4` invokes `getsize`; `dos/v86/inc/origin.inc:51-54` describes its size extraction role; source is retained under `dos/v86/tools/src/getsize`. No evidence this task invokes historical DOS rebuild. **Historical guest-build tool, not guest runtime.** |
| `link16.exe` | Exact historical copy retained. Many original makefiles explicitly invoke it (e.g. `dos/v86/doskrnl/dos/makefile:317`, `wow16/drivers/keyboard/makefile:107`); `tools/build/Build-T225S8PrimaryCommand.ps1:21,45` explicitly consumes historical copy. These are build paths, not proof of byte provenance of currently installed guest. **Retained usable build dependency; not product runtime.** |
| `stripz.exe` | Exact historical copy retained. Original `dos/v86/doskrnl/dos/makefile:323` and current `tools/build/Build-T225S8PrimaryNtdos.ps1:22,53-54` reference it; corresponding original source exists under `dos/v86/tools/src/stripz`. **Retained guest-build dependency, not product runtime.** |

Suggested ledger resolution: classify according to historical build/tool role with the relocation and exact hash; don't mark overall functionality restored and don't equate non-mirroring with absence. Their necessity for any currently authorized guest build remains a separate governance decision; current audit does not authorize guest recompilation. The helper only checks paths/hashes and does not establish successful execution, transitive toolchain closure, provenance of installed artifacts or alternate-edition runtime.

Reproduce static references:

```powershell
rg -n -i '\b(c13216|c33216|cl16|cvpack|stripz|getsize|link16)\b' src/mvdm -g '*make*' -g '*.cmd' -g '*.inc'
rg -n 'link16|stripz' tools/build/Build-T225S8Primary*.ps1
Get-Content tools/historical/opennt-4.5/cl.def
$env:PYTHONDONTWRITEBYTECODE='1'; python artifacts/mvdm-audit/shard-3/tools16-inventory-observer.py
```
