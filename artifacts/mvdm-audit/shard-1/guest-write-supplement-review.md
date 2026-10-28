# Shard 1: pending Windows Write historical auxiliary inputs

The observer confirms these eight entries were `待核验` in the current shard ledger, and each matches both frozen OpenNT editions. It hashes the original matching makefile, link response, resources and readme, then checks four existing x86 worker/WOW32 and x64 frontend/service Ninja graphs and dependency logs. No exact scoped path appears in those checked host inputs; this does not establish absence from every possible build.

| File | Bounded role and evidence |
| --- | --- |
| `aaa.c` | Historical substitute `_malloc` stub according to its own source comment. The original makefile contains an `aaa.obj: aaa.c` rule, but its object is absent from the checked `write.lnk`; the existence of a rule alone is not proof it contributes to that executable. |
| `editftn.c` | Historical Write editing source with an explicit `editftn.obj` make rule, also absent from checked `write.lnk`. Its possible use by other build variants is not ruled out. |
| `center.bms`, `ltabbtn.bms` | Retained binary-named graphics assets; no direct reference to these filenames was found in the checked `write.rc`, which instead names device bitmap strips. Their generation/use in other resource recipes is unproved; do not equate name or file presence with a live control. |
| `edm.edm` | Historical list of source paths and KANJI-related conditional directives; the original readme points to it in discussing an old Kanji build process. This is documentation/build-support evidence, not an active compile rule. |
| `globdefs.itl` | Alternate internationalization text/definitions; the checked active source `mglobals.c` includes `globdefs.h`. No verified active rule selects the `.itl` variant. |
| `kmakend.bat` | Historical make wrapper with Kanji flags and optional source sync; `kmake.bat` has a reference to it only on a `rem` comment line in the checked copy. It was never executed. |
| `write.sed` | Historical sed script that transforms `OBJ` assignments into output lines. No invocation was established in the checked original makefile/link/resource paths. |

These are **candidate historical guest source, asset, localization or build-support roles only with respect to the four checked host graphs**. Do not exclude Write functionality or conclude the historical guest build variants were unavailable. Most importantly, the original source-tree `WRITE.EXE` SHA-256 `08EE1659788880FB593815FF9609BFE76743C1182D05E6702764BF3DFF68B30B` differs from the current `WINNT/WRITE.EXE` inside `assets/winnt.zip` SHA-256 `EB44B5D5F5A405B6A8CD018B9F790BCA2E7E895DAD4F7E1F5462EC14CCDAA87A`. Their relationship and build provenance are unproved; source/recipe observations cannot be attributed to the archived executable as exact lineage.

Replay `python artifacts/mvdm-audit/shard-1/guest-write-supplement-observer.py`; JSON records per-file identities and graph hashes. No product/guest was executed or modified, and no build tree or shared ledger changed. Guest Write behavior, selected UI assets, alternative language builds and the differing ZIP executable remain outside this static evidence.
