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

## Process attribute-list extraction P

The owner admitted a local P for `lib-win32/process`. Its generic attribute
list helper owns allocation, initialization, one attribute update and cleanup.
NTSRV Worker creation retains Job policy, admission, resume and failure
rollback; frontend creation retains its inherited handle list, admission and
retirement decisions. Attribute values remain borrowed until list destruction.

The x64 NTSRV and x86 focused fixture were rebuilt. The production library
composition and negative ownership checks passed. The 36-check unit fixture
covers success, allocation, initialization and update failures, duplicate
initialization, resource release and repeat destruction. The package uses the
new NTSRV and nine verified baseline build products. All ten input and staged
output hashes matched; 63 immutable guest files and 274 export ordinals passed.
Build, fixture, stage and Control command/PID/output records reside in
`build/t475-process-p`.

Control passed all 12 selected gates against the identical ten-image package;
the optional DOS4GW/NESticle external fixture was absent and skipped. Full was
not run and the known original guest long-PATH limitation remains. Package
`f3161950-6ff1-4558-9080-9686bd37cb98` was published to this worktree's
`assets/release`, with all ten release hashes verified.

## Shared Win32 handle P

The owner admitted a further P to share same-access handle duplication between
NTSRV's RPC output export and NTHOOK's child attachment. `lib-win32/handle`
accepts explicit source and target process handles; its remote cleanup duplicates
the target-owned handle back with `DUPLICATE_CLOSE_SOURCE`, then closes the
local duplicate. NTSRV retains MIDL allocation, output cleanup and RPC transfer
ownership; NTHOOK retains payload, adoption and rollback decisions.

The fresh ten-image build passed frontend and BaseSrv ownership checks. The
new handle fixture passed 13 checks with an actual child process, including
local duplication, remote duplication, remote release and stale-handle
rejection. The x64 Hook installer regression passed 118 assertions. An initial
link graph generation inserted an input before a multi-output separator; it was
corrected. A later build script combined test executable names; separately
invoked fixtures passed. Build, fixture, staging and Control commands, PIDs,
times and outputs are retained under `build/t475-handle-p`. Ten fresh package
inputs and outputs matched their hashes, 63 immutable guest files and 274 NTVDM
export ordinals passed verification.

Control passed all 12 selected gates on this identical package. The optional
external DOS4GW/NESticle fixture was unavailable and skipped; Full was not
run, and the original guest long-PATH limitation remains. Package
`9baf506d-436a-411a-aeda-cf70fcfea4a0` was published to this worktree's
`assets/release`, with all ten release hashes verified.
