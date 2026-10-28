# Part 2: DOS shared headers, assembler includes and source inputs

All 39 owned paths match both frozen original editions. The companion observer scans retained DOS v86 source/recipe text for literal basenames; its JSON records per-file counts and up to 20 source/line/snippet leads. These are **not resolved include paths**, proof of active conditional compilation, or exhaustive original dependencies: generic C header names collide with SDKs, comments may match, and builders may synthesize names.

| Owned key | Basename observations | Restricted disposition |
| --- | --- | --- |
| `dos/v86/h/assert.h` | 1 textual matches; example `dos/v86/tools/src/h2inc/h2inc.c:120` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/h/bios.h` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/h/bios.inc` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/h/dos.inc` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/h/errno.h` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/h/float.h` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/h/graph.h` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/h/limits.h` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/h/malloc.h` | 2 textual matches; example `dos/v86/tools/src/h2inc/h2inc.c:123` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/h/math.h` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/h/memory.h` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/h/mixed.inc` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/h/process.h` | 4 textual matches; example `dos/v86/tools/src/buildidx/buildidx.c:21` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/h/search.h` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/h/setjmp.h` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/h/share.h` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/h/signal.h` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/h/stdarg.h` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/h/stddef.h` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/h/time.h` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/h/varargs.h` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/inc/_parse.asm` | 1 textual matches; example `dos/v86/cmd/mem/makefile:34` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/inc/chkcpu.inc` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/inc/dbcs.sw` | 64 textual matches; example `dos/v86/inc/dossym.inc:31` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/inc/inc.tag` | 0 textual matches; example `none in captured text` | Historical change-log candidate only; verify content before integrating; related functionality stays open. |
| `dos/v86/inc/macro.def` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/inc/mft.inc` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/inc/msdata2.asm` | 0 textual matches; example `none in captured text` | Historical alternate wrapper; observed DOS kernel recipe separately assembles msdata and msinit (see shard-2/msdata-wrapper-owner-review.md). Other profiles and DOS semantics remain pending. |
| `dos/v86/inc/msghan.inc` | 1 textual matches; example `dos/v86/cmd/nlsfunc/msg2nls.inc:99` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/inc/msgserv.sa` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/inc/mshead.asm` | 3 textual matches; example `dos/v86/doskrnl/dos/makefile:67` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/inc/mstable.asm` | 2 textual matches; example `dos/v86/inc/ms_data.asm:211` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/inc/parse.asm` | 32 textual matches; example `dos/v86/inc/psdata.inc:482` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/inc/pathmac.inc` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/inc/pcgequ.inc` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/inc/pcinput.inc` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/inc/redirsym.inc` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/inc/smdossym.inc` | 0 textual matches; example `none in captured text` | Pending exact consumer, selected profile and behavior. |
| `dos/v86/inc/smifssym.inc` | 2 textual matches; example `dos/v86/doskrnl/bios/makefile:49` | Pending exact consumer, selected profile and behavior. |

See the JSON for reproducible original/current hashes and literal line evidence, including multiple hits and conditional/commented contexts. No guest build or runtime semantics were inferred, and the shared ledger was not changed. Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/nine-way/part-2/dos-shared-inputs-observer.py` and compare JSON.
