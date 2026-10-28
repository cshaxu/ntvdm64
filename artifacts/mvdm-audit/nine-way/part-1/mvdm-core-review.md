# Part 1 MVDM: COMMAND, DEM, debugger and DOS kernel remainder

This report covers the remaining ten assigned part-1 MVDM keys outside the separately audited BDE, bin86, DOS command/device/keyboard/graphics families. `mvdm-core-evidence.json` captures each complete source/reference hash, four current normalized VALID dep checks and ZIP/system32 basename observations; replay `PYTHONDONTWRITEBYTECODE=1 python -B artifacts/mvdm-audit/nine-way/part-1/mvdm-core-observer.py`. A C source file usually appears as a Ninja direct source edge rather than in the dep log, so zero dep entries must not be interpreted as not compiled.

| Assigned key | Source identity, actual role and bounded recommendation |
| --- | --- |
| build.log | Retained historic build log, absent in primary originals but hash-identical to third OpenNT mirror; not runtime source. Inspect provenance before using it as a build witness. |
| dbg/dbg.c | Exact primary originals; current worker graph compiles debugger-bindings dbg.obj and map includes GPFault/ModuleLoad/SendVDMEvent. Debugger attach and target context/handle lifetime remain separate obligations; map alone is not a live debugger test. |
| dos/command/cmdexit.c | Exact original; direct current compile, map attributes cmdExitVDM to original-mvdm-command:cmdexit.obj. `cmddisp.c:16` maps exit service to it and it calls TerminateVDM. `shard-1/command-exit-owner-audit.md` traces guest and broker/console handoff; preserve those behavior/lifecycle obligations. |
| dos/command/cmdkeyb.h | Exact original; one VALID object dependency cmdkeyb.obj. Keyboard-layout lookup and bounded/unbounded registry string concerns are covered by `shard-3/cmd-keyboard-layout-role-review.md`; do not count declaration identity as full layout validation. |
| dos/command/cmdpif.h | Exact original PIF configuration structures; zero exact VALID deps here. Historical note says structure mirrors nt_pif.h, but actual include/profile ownership must be checked; do not infer PIF unsupported or independently maintain duplicate authority. |
| dos/dem/demmsg.c | Exact original; direct graph compiles demmsg.obj, but its implementation is inside `#if DBG`. Actual selected flags and symbols decide debug behavior; original file compilation does not establish messages are active. |
| dos/dem/demmsg.h | Exact original; 11 VALID DEM object dependencies. Debug message declarations/indices, not a production runtime success or error recovery test. |
| dos/test/test.bat | Exact original DOS test script with historical drive paths and file deletion commands; not executed, not itself runtime evidence. No user or guest files touched. |
| dos/v86/doskrnl/bios/dosmsg.cmd | Exact original historical message-build script; see `shard-1/dos-kernel-build-family-review.md`. No proof that it produced current NTIO. |
| dos/v86/doskrnl/dos/crit.asm | Exact original DOS critical-section assembly; bounded original makefile/link search found no direct crit.obj/crit.asm reference. This does not prove all kernel critical sections or other build variants absent. Preserve guest semantics and seek actual selected provider. |

Current worker map and graph are build evidence, not full deployed-image binding. The published behavior and test/lifetime gaps above remain pending; exact original bytes are not a behavioral acceptance or restoration percentage. No shared ledger, guest, product, system settings or build tree was modified.
