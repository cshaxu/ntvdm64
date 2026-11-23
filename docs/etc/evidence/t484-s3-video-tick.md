# T484 S3 P84 video projection and CCPU runner reconciliation

B0115 now uses the native blocked-video-tick target. It selects the same three
current nt_graph.c functions, retains their internal directives, and compiles
the unchanged fixture with /MT /O2 /W4 /WX /TC. Unique signature and top-level
closing-brace boundaries are checked; source changes retrigger configuration.
No original/mirror file is edited or original guest rebuilt.

The public runner retains fresh build-owned output, input hashes and runtime
failure propagation. Its unused arbitrary CompilerWrapper parameter is replaced
by explicit x86/x64 architecture selection (default x86); no current caller
uses that override. Removing the cleanup-helper import also removes an
unnecessary transitive staging dependency from this standalone fixture.

Run PID31560 exited0 on x86/x64: blocked ticks preserve dirty/cursor/mode/stream
state, resume and final paint work, and real error propagation remains asserted.
Cached full host build PID21084 exited0. The first projection comparison
PID21236 failed solely because CMake wrote CRLF while the audit oracle used LF.
PID12056 compares normalized source text, confirms all three exact bodies,
verifies stable cached outputs, and exits0. No product change was used to make
that comparison pass.

B0116 and B0117 are standalone assertion runners: their only subprocesses are
the supplied native fixture and native oracle. They neither compile nor stage
products. Native run-ninja-parallel.cmd builds the existing three CCPU targets;
131 selected-CCPU cases pass (13 paging/fault,118 bit-index), plus the existing
native oracle assertions. Link-owner checks remain enforced. Original fixture
/FORCE duplicate seam warnings are retained in raw logs, not new production
link behavior or proof of guest exception delivery.

[Final receipt](../../../artifacts/build-migration/t484/s3-video-tick/final-result.json)
and the two CCPU result records preserve outcomes. Raw build/t484-s3-video-tick
contains first-failure and corrected checks. All11 raw production inputs and22
published hashes remain unchanged; both publication locations are verified.
The retained final.ps1 can be replayed from a fresh directory directly below
build after adjusting its two explicit evidence output arguments.

P36-P60 rows move verbatim from CURRENT to the indexed early-deliveries record;
no evidence is discarded. Following successful governance and delivery, B0115
is migrated and B0116/B0117 retained as native test delegates. Fixed355 total:
164 complete,191 remaining (53 pending producer/staging/verification rows,
138 awaiting reconciliation). S3 remains active.
