# DOS historical build-tool declarations and inputs

Four pending shard-2 files match both original editions by SHA-256. The observer checks exact, normalized paths in VALID dependency records for captured worker and WOW32 Ninja trees and their literal build graphs; no hits were observed. This establishes neither all-profile absence nor historical C include resolution, since generic headers and libraries may come from an external SDK and stale/missing dependency records are excluded.

| File | Evidence and proposed bounded disposition |
| --- | --- |
| `dos/v86/tools/src/getsize/getsize.def` | GETSIZE module definition describes the tool; sibling `sources` selects `getsize.c` and `BUILDTOOL=1`. Historical `makedos.cmd` invokes getsize; its source reads DOS/BIOS maps and reads/writes size include data. Historical `maketool.def` resolution was not found in the inspected source trees. **Pending** if exact `.def` consumption is required; otherwise classify as historical tool metadata only, never as DOS runtime. |
| `dos/v86/h/process.h` | Historical `buildmsg.c` includes `"process.h"`; GETSIZE, STRIPDD and BUILDIDX include `<process.h>`. Exact historical include search resolution is not proven; host SDK may supply a same-named file. **Pending**. |
| `dos/v86/h/stddef.h` | C language declaration header, but actual historical consumer and include-root resolution remain unverified. **Pending**. |
| `dos/v86/lib/oldnames.lib` | Original binary library data preserved with dual-original hash identity. No exact current graph/dependency path hit; actual historical linker consumer, selected library search paths and member semantics remain unverified. **Pending**. |

GETSIZE's use in the historical kernel build and the source-level dataflow are not proof that the captured release guest image was built from it. In particular, its source references different relative read and write paths for `bdsize.inc`; no build was executed to resolve those working-directory assumptions. Retain DOS boot, guest memory and tool reproducibility as separate concerns. No historical makefiles or tools were executed. Product, guest, shared ledger and build trees were not modified.

Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/shard-2/dos-build-tool-auxiliary-observer.py` and compare its JSON. See `dos-v86-build-tools-review.md` for STRIPZ/STRIPDD/H2INC details, and `msdata-wrapper-owner-review.md` for separate NTDOS image identity.
