# Historical VDD sample family: build and delivery boundaries

Six shard-2 pending entries belong to three distinct original samples. Five match both original editions; `hpscan16.exe` is current-only and its provenance is not known. Captured x86 worker and WOW32 Ninja build graphs do not name these exact paths. This is a graph snapshot, not a proof about arbitrary external build tools or product VDD capabilities.

| Pending file | Evidence and restricted disposition |
| --- | --- |
| `vdd/samples/adlibvdd/adlibvdd.def` | Original definition names ADLIBVDD and exports DllEntryPoint; sibling `sources` declares a separate sample DLL from vdd.c. **Historical sample build metadata**; exact definition consumption by the original shared build machinery remains unverified. |
| `vdd/samples/hpscan/hpscan16.inc` | `hpscan16.asm` includes it for request-header/status declarations; historical sample makefile builds a device stub. **Historical sample guest build input** in that observed recipe, not a claim scanner support exists or is missing. |
| `vdd/samples/hpscan/hpscan16.exe` | Current-only file, absent in both original trees; sample recipe links an EXE then converts it to a SYS, but this does not identify how this exact file was produced. The current ZIP contains neither a same-name member nor an exact byte copy under any name. **Pending provenance**, retain rather than declare original or delete. |
| `vdd/samples/mscdex/tsr/messages/usa/messages.inc` | Historical TSR makefile defaults COUNTRY to usa, copies selected messages.inc, and compiles mscdexnt.asm which includes it. **Historical sample localized build input**, without proving TSR provenance or localized behavior. |
| `vdd/samples/mscdex/vdd/mscdex.h` | Historical `vcdex.c` directly includes it; sibling sources selects vcdex.c into a separate sample DLL. **Historical sample declaration**; native CD-ROM implementation and host adapter are separate. |
| `vdd/samples/mscdex/vdd/vcdex.rc` | Original `sources` selects VCDEX.RC for the separate sample DLL. **Historical sample resource input**; no published DLL identity or runtime execution is inferred. |

Suggested historical roles are scoped to the captured sample recipes and do not justify deletion or excluding ADLIB, scanner, CD-ROM, VDD, or guest behavior. For anything requiring proof of final original build output or actual service selection, retain pending. No historical makefile was run (several contain destructive cleanup), no guest was rebuilt or run, and no product or shared ledger was modified. Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/shard-2/vdd-sample-family-observer.py` and compare JSON.
