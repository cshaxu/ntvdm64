# DOS guest core/message include roles — shard 3

Five paths below were selected from `shard-3/file-review.csv` with review_class pending. `dos-inc-role-observer.py` and captured JSON record SHA-256 and byte sizes against both original editions, including comparison siblings and historical makefiles. All ten observed files match both originals. This review is read-only; no guest was rebuilt, modified or run.

| pending file | proposed status | source/build evidence and boundary |
|---|---|---|
| `dos/v86/inc/dbcs.sw` | pending | Historical build switch checks the DBCS assembler definition; COMMAND and DOS kernel makefiles list it among many inputs. Does not prove the shipped guest used a DBCS build or that DBCS behavior works. |
| `dos/v86/inc/mshead.asm` | pending | Declares the original NTDOS startup entry and includes origin/dos segment definitions; `doskrnl/dos/msconst.asm:24` includes mshead.asm and the DOS kernel makefile names it. Exact historical include resolution and shipped NTDOS provenance remain to be proved. |
| `dos/v86/inc/mstable.asm` | pending | 231-byte original wrapper includes version.inc, mssw.asm and ms_table.asm. `doskrnl/dos/makefile:301` names mstable.asm for mstable.obj, but there is also a distinct 21,852-byte `doskrnl/dos/mstable.asm` in the recipe directory; the recipe reference alone cannot determine which file the historical assembler read. Both hashes are captured. |
| `dos/v86/inc/msghan.inc` | pending | Common message-handling definitions; search found no direct include by that exact name among examined DOS sources, while NLSFUNC msg2nls.inc contains related historical material. Do not infer source/image selection or equivalent behavior without a resolved consumer. |
| `dos/v86/inc/msgserv.sa` | pending | 114,075-byte historical message-service source, distinct from 124,914-byte `msgserv.asm`. COMMAND, MEM and NLSFUNC makefiles explicitly name the `.asm` file, not `.sa`; that fact cannot establish whether another tool or recipe consumed `.sa` or whether its behavior is present. |

Current NTVDM package uses immutable guest DOS components and does not directly compile these inputs. That observation is not a reason to exclude guest DOS startup, DBCS or message services. Exact original source hashes, historical recipe mentions and sibling differences are static evidence only; deployed binary provenance, loader/locale behavior and build include resolution remain open. No restoration percentage is asserted.
