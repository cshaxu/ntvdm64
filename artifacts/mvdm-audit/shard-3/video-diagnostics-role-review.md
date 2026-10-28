# Legacy EGA trace and GISP statistics declarations — shard 3

Scope: pending `softpc.new/base/inc/egatrace.h` and `hg_stat.h`. `video-diagnostics-observer.py` and captured JSON compare both headers and `base/video/ega_trcr.c` with both original editions, normalize VALID retained Ninja C-dependency lines to the worker build directory, and retain the observed object symbol table and map match. All three files match both references. No product, guest, shared ledger, build state or interactive video was modified.

`egatrace.h` defines historical binary trace operation identifiers. Its observed consumer `ega_trcr.c:14-23,276` is under EGA_DUMP and writes EGA activity for a standalone tracer. The selected `ega_trcr.obj` has no such trace function symbols and the current map has no ega_trcr.obj line; normalized snapshot contains no egatrace.h edge. Propose **excluded as historical trace-format header only**. Do not exclude EGA/VGA port behavior, rendering or current debugging capability; no device output test was performed.

`hg_stat.h` declares GISP profiling counters only in non-PROD and defines `PC_S_INC` as empty in PROD. No direct include or exact normalized dependency was found in the examined selected source/dependency snapshot. Propose **excluded as historical GISP statistics header only**, not CPU execution, scheduling or profiling/debug support generally. Other build profiles and indirect/external use are not covered.

Dependency and map/object hashes are preserved; map absence does not prove absence in other builds, and this object was not bound here to the newest deployed EXE. No executed behavior or restoration percentage is inferred.
