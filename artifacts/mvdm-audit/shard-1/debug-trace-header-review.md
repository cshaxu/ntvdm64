# Shard 1: normalized dependency audit for debug and trace declarations

All five paths were `待核验` in the current shard ledger and each matches both frozen OpenNT editions. The observer reads the Ninja dependency output for four checked x86 worker/WOW32 and x64 frontend/service trees. It accepts only `(VALID)` object records and resolves **each dependency relative to that graph's working directory** before exact path comparison. The raw dependency and resolved path for each hit are retained in JSON. Dependency presence proves compiler input, not active debug behavior.

| File | Original role and checked build evidence |
| --- | --- |
| `softpc.new/base/inc/dsktrace.h` | Disk trace bit names and `dt0`–`dt5` macros. Exact VALID dependencies occur in the x86 worker for `obj/disks/fdisk.obj` and `obj/disks/diskbios.obj`. The selected worker flags contain `PROD`; the original `#else` makes the `dt*` macros empty under that flag. The header is therefore an actual selected compiler input, not an absent historical file; this does not prove any trace feature runs. |
| `softpc.new/base/inc/hunter.h` | Original Hunter declarations behind `#ifdef HUNTER`. Exact VALID dependency occurs for x86 `obj/bios/reset.obj`, whose source includes it unconditionally. The checked worker flags do not define `HUNTER`; selected dependency does not imply selected Hunter implementation. Other source includes are themselves conditional or belong to other components. |
| `softpc.new/base/inc/d_inst.h` | Original decoded-instruction enumerations used textually by `dasm386.c` and `decode.c`; no exact VALID dependency was found in the four checked trees. This does not classify other debug profiles or prove a disassembler is unneeded. |
| `softpc.new/base/inc/hosthunt.h` | Host-specific Hunter definitions guarded by `HUNTER`; no exact VALID dependency was found in these trees. Macro-generated, external and alternate-profile include paths remain unverified. |
| `softpc.new/base/inc/harness.h` | Original generic test-harness definitions; no exact VALID dependency was found in these trees and no specific selected consumer was established. Historical test ownership remains pending. |

Two concrete hits demonstrate why searching the Ninja log for a full absolute string is insufficient: the log stores relative paths such as `../../../../src/mvdm/softpc.new/base/inc/dsktrace.h`. Prior negative path observations made by absolute-substring matching should be independently normalized before using them as exclusion evidence. These findings do not support exclusion of disk operations, debug functionality or other build variants.

Replay `python artifacts/mvdm-audit/shard-1/debug-trace-header-observer.py`. No compiler, product or guest was run; no source, build tree or shared ledger was changed. Actual preprocessed declarations, alternate debug builds and runtime trace behavior are outside this evidence.
