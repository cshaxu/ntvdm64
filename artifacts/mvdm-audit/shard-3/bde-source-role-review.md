# BDE debugger extension source role: shard 3

Read-only audit of four pending BDE files. The observer `bde-source-observer.py` records SHA-256 and size of each file in both original trees and current source; captured results in `bde-source-observations.json`. All six inspected files are byte-identical across the three trees. This shows source identity, not linked or operational coverage.

Original call/ownership chain: `src/mvdm/bde/sources:41,53,83-90,114` specifies a separate `bde` dynamic library containing bdecmds and bdemisc, with precompiled `precomp.h`; `bde.def:1-18` identifies Bob's Debugger Extensions and exports `db`, `dw`, `dd` and others. `bdecmds.c:49-159` sets up debugger process, thread and extension function pointers and forwards `db/dw/dd` to `DumpMemory(1/2/4)`. `bdemisc.c:26-110` reads context, parses selector/offset, obtains target base and reads/prints memory. `precomp.h:6-17` includes debugger and VDM/WOW definitions and `bde.h`; the latter declares the extension's callbacks and shared interfaces.

| path | disposition | remaining proof |
|---|---|---|
| `bde/bdecmds.c` | pending | historical debugger DLL entry, not an NTVDM guest instruction handler. Need resolve whether BDE extension is an explicit delivery obligation and if any equivalent external debugger contract exists; source identity alone does not establish execution. |
| `bde/bdemisc.c` | pending | target-memory dumping helper for debugger entry; same delivery and behavior boundary, including address interpretation and failed target reads. |
| `bde/bde.h` | pending | shared BDE extension declarations; status follows selected debugger extension closure, not guest runtime. |
| `bde/precomp.h` | pending | historical precompiled include bundle, not standalone executable behavior; depends on BDE disposition and actual include resolution. |

Current authority `docs/design/ARCHITECTURE.md:722,774,782,1239` mentions debugger boundaries, so absence of these names from the current host build graph cannot alone justify declaring the functionality out of scope or restored. It also does not prove this particular historical extension is required: establish the specific contract before deciding. No claim about process-memory reads, debugger availability, runtime behavior, or restoration percentage is made. No build, guest modification, external debugger attachment, or runtime test was performed.
