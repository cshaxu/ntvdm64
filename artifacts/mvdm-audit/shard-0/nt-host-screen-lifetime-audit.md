# Redirected output and initial active-buffer identity

`softpc.new/host/src/nt_hosts.c::InitScreenDesc` first copies the inherited
stdout handle into both `sc.OutputHandle` and `sc.ActiveOutputBufferHandle`.
If stdout is not `FILE_TYPE_CHAR`, both original and current code obtain
`CONOUT$` and replace `sc.OutputHandle`. The current code additionally closes
the inherited stdout and distinct inherited stderr after replacing the
process standard handles; this was intended to release superseded pipe writers.
The original code did not close those values at this point. Neither body
updates `sc.ActiveOutputBufferHandle` in that branch.

`nt-host-screen-fixture.py` verifies frozen original/current source hashes,
extracts both actual function bodies, and compiles them with explicit test-only
handle/API stubs. For a Console input and pipe stdout/stderr alias it reports
`original output=33 active=22 active_closed=0 redirected=1` and
`current output=33 active=22 active_closed=1 redirected=1`. Both invoke the
mock setup path. This directly demonstrates a newly closed value remaining in
the active-buffer field in that controlled configuration. The compiler emits
one pointer/integer warning because this bounded fixture represents handles
as integers; it does not model actual Win32 handle values, handle reuse,
Console IPC or later initialization. It is not a product hang reproduction.

Original `nt_mouse.c::MouseDisplay` loops while `ShowConsoleCursor(active,
TRUE)` is negative. The selected `console_client.c` returns -1 if its output
is neither a recognized client output nor a valid Console screen buffer;
`nt_graph.c` and `nt_event.c` can later republish `sc.OutputHandle` as active.
Whether any real reachable path calls `MouseDisplay` with the stale value
before such a reset remains unverified, as does normal aliasing of stdin,
stdout and stderr at actual launch. A closed numeric value can also be reused;
do not assume every later use fails or claim a live hang from the fixture.

Before changing this boundary, trace standard-handle duplication by NTSRV,
the exact first active-buffer use and failure/cleanup ordering for redirected
DOS, shared/aliased standard handles and graphics transitions. A later repair
must preserve pipe EOF and independently owned DOS-record handles, and test
the source entry plus actual redirected guest lifecycle. The project-owned
mirror has not been changed in this review.
