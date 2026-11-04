# T475 S1 — NXVM and Win32 libraries, RPC

Owner directs a distinct `src/lib-nxvm` for the existing NXVM closure and
`src/lib-win32` for project platform mechanisms, starting with RPC. Existing
`io` moved alongside RPC to keep its Win32 ownership explicit. NTVDM64
protocol clients and authorization/lifecycle policy remain in common or NTSRV.

Local binding and async completion moved to lib-win32/rpc. The service's local
peer process matching was separated from its authentication and authorization:
NTSRV still performs those checks first, then calls the extracted Windows RPC
locality, PID, and process-liveness mechanism. Errors and output clearing are
unchanged. `tests/app/win32_rpc_peer_test.c` covers eight success and refusal
cases; `tests/app/common_rpc_binding_test.c` covers 42 checks and cleanup.

The NXVM import manifest records 52 files, including 44 mechanical include
prefix adjustments. The previous upstream SHA-256 is retained per changed
file, and comparison against the previous commit proves those are the only
byte changes. The original NXVM and libvterm licenses and archive are retained.

The fresh x86 and x64 product builds and ownership negative controls passed;
the x86 frontend-window-library test passed. The staged ten-image package was
checked against all ten fresh input hashes and output hashes; the 63 immutable
guest files and 274 NTVDM export ordinals passed. Build and test command, PID,
time, exit status and output logs are retained under `build/t475-s1`.

Control passed all 12 selected gates on the same ten-image package, including
real DOS/native interaction, rejection, failure and nested-window scenarios.
An earlier run was interrupted by the session switch and is not counted. The
optional external DOS4GW/NESticle fixture was absent and skipped; Full was not
run. The original guest long-PATH limitation remains. Package
`b08813cb-cf4a-48d5-8ec8-cd0590a35f47` is published only to this worktree's
`assets/release`; all ten release hashes matched.
