# S10 shard 1: DOS kernel historic build and notes family

Four pending paths under `dos/v86/doskrnl` were checked as a guest-kernel build/history family. All four current hashes exactly match both original editions; `dos-kernel-build-family-evidence.json` gives full hashes and bounded ZIP/system32 basename observations. Replay `PYTHONDONTWRITEBYTECODE=1 python -B artifacts/mvdm-audit/shard-1/dos-kernel-build-family-observer.py`. No guest build or execution occurred.

| File | Actual role and classification boundary |
| --- | --- |
| bios/dosmsg.cmd | Historical command script calls buildidx then nosrvbld on supplied messages and skeleton. The BIOS makefile lines 85-92 describes an older dosmsg.bat workflow rather than asserting this exact CMD was used to create current NTIO.SYS. No command was run. |
| dos/makedos.cmd | Historical source-control/build helper invokes out/in, getsize and nmake. `inc/origin.inc` comments refer to a makedos.bat workflow; not proof this exact CMD is used in the current build or that its tool dependencies are available. |
| dos/dispatch.txt | Historical commented descriptions and INT 21 dispatch table notes, not a selected compiler unit as established by filename. No same-directory dispatch.asm was found; do not infer dispatcher functionality from or against the note. |
| dos/state.rst | Retained editor session state with historical paths, not DOS runtime state or a product configuration. `shard-3/dos-messages-role-review.md` also records this item. No path inside it was accessed. |

Current source `doskrnl/bios/NTIO.SYS`, ZIP and exact installed system32 NTIO all hash `cfc8be16576bb6acd16bb8fc9b2d9a080f544bbfdd2d2d2df07ed908b3ab4937`; source `doskrnl/dos/NTDOS.SYS`, ZIP and installed NTDOS all hash `957662320654ad5251c3a8b228a5dadec28aa65dddbcba38c3658a6e7f93bc84`. Matching images do not demonstrate the four historical files produced them, nor DOS call/exit behavior. All four same-basename ZIP and installed system32 observations were negative; alternate directories, generated builds and runtime are outside scope. No shared ledger, build tree, product or guest modified.
