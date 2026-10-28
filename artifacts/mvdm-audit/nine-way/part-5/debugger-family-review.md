# Part 5: external debugger and VDMDBG family

The 17 owned original MVDM files match both frozen editions. Original sources recipes define three separate tools: VDMDBG DLL, VDMEXTS NTSD debugger extension and SoftPC VDMDEBUG program. The companion JSON records each exact path, source/recipe hashes, literal original recipe references and selected x86 worker/WOW32 graph checks. No key path appears in those two captured graphs; the archive named-output observation is in JSON. Original source/recipe identity does not prove a tool is shipped, nor that the current project has an equivalent debugging ABI.

| Owned key | Original recipe evidence | Bounded status |
| --- | --- | --- |
| `softpc.new/debugger/vdmdebug.c` | softpc.new/debugger: 46:SOURCES=vdmdebug.c | Original separate debugger implementation; required tool scope/API and live behavior remain pending. |
| `vdmdbg/vdmdbg.c` | vdmdbg: 59:SOURCES=vdmdbg.c vdmdbg.rc | Original separate debugger implementation; required tool scope/API and live behavior remain pending. |
| `vdmdbg/vdmdbg.def` | vdmdbg: No direct file-name hit in original sources recipe | Original separate debugger declaration/metadata/resource; exact use and ABI remain pending. |
| `vdmdbg/vdmdbg.rc` | vdmdbg: 59:SOURCES=vdmdbg.c vdmdbg.rc | Original separate debugger declaration/metadata/resource; exact use and ABI remain pending. |
| `vdmdbg/vdmdbg.txt` | vdmdbg: No direct file-name hit in original sources recipe | Historical debugger documentation only; debug APIs and capability stay separate. |
| `vdmexts/data.c` | vdmexts: 39:SOURCES=data.c debug.c disasm.c heap.c help.c misc.c platform.c     \ | Original separate debugger implementation; required tool scope/API and live behavior remain pending. |
| `vdmexts/debug.c` | vdmexts: 39:SOURCES=data.c debug.c disasm.c heap.c help.c misc.c platform.c     \ | Original separate debugger implementation; required tool scope/API and live behavior remain pending. |
| `vdmexts/dib.c` | vdmexts: 41:wowprof.c dib.c \ | Original separate debugger implementation; required tool scope/API and live behavior remain pending. |
| `vdmexts/disasm.c` | vdmexts: 39:SOURCES=data.c debug.c disasm.c heap.c help.c misc.c platform.c     \ | Original separate debugger implementation; required tool scope/API and live behavior remain pending. |
| `vdmexts/heap.c` | vdmexts: 39:SOURCES=data.c debug.c disasm.c heap.c help.c misc.c platform.c     \ | Original separate debugger implementation; required tool scope/API and live behavior remain pending. |
| `vdmexts/help.c` | vdmexts: 39:SOURCES=data.c debug.c disasm.c heap.c help.c misc.c platform.c     \ | Original separate debugger implementation; required tool scope/API and live behavior remain pending. |
| `vdmexts/i386/profile.c` | vdmexts: 44:i386_SOURCES=i386\profile.c i386\vdmtib.c i386\regs.c | Original separate debugger implementation; required tool scope/API and live behavior remain pending. |
| `vdmexts/i386/regs.c` | vdmexts: 44:i386_SOURCES=i386\profile.c i386\vdmtib.c i386\regs.c | Original separate debugger implementation; required tool scope/API and live behavior remain pending. |
| `vdmexts/i386/vdmtib.c` | vdmexts: 44:i386_SOURCES=i386\profile.c i386\vdmtib.c i386\regs.c | Original separate debugger implementation; required tool scope/API and live behavior remain pending. |
| `vdmexts/misc.c` | vdmexts: 39:SOURCES=data.c debug.c disasm.c heap.c help.c misc.c platform.c     \ | Original separate debugger implementation; required tool scope/API and live behavior remain pending. |
| `vdmexts/platform.c` | vdmexts: 39:SOURCES=data.c debug.c disasm.c heap.c help.c misc.c platform.c     \ | Original separate debugger implementation; required tool scope/API and live behavior remain pending. |
| `vdmexts/precomp.h` | vdmexts: 54:PRECOMPILED_INCLUDE=precomp.h | Original separate debugger declaration/metadata/resource; exact use and ABI remain pending. |

In particular, the presence of NTMON or another debugger does not prove equivalence with VDMDBG task/selector APIs, NTSD commands or the SoftPC debugger protocol. No actual debugger or guest was run; source-to-shipped-binary provenance, callback, memory and lifecycle behavior remain open. Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/nine-way/part-5/debugger-family-observer.py` and compare JSON. No guest, product, build tree, external source or shared ledger changed.
