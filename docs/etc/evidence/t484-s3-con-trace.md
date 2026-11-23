# T484 S3 P82 Console trace and control fixture producers

B0110's direct NASM invocation moves to the opt-in win31-con-trace native
fixture target. COM and listing are both declared outputs, so either missing
output invalidates the rule. Assembly source and its flags remain unchanged.
The runner retains its parameter validation and prepares the same batch/PIF
files; those interpreted staging operations remain pending for S4.

B0111 no longer writes a toolchain/Ninja batch script into a supplied cache.
It delegates to the CMD win101-control-fixtures target in that existing native
worker cache. The target groups the same three executable outputs, with their
existing source profiles and link inputs; no duplicated compiler commands.
Non-CMake historical caches fail rather than pretending to rebuild successfully.

Build verification PID22732 exited0: COM and listing match direct NASM bytes,
no-op hashes/mtimes are stable, and x64 is rejected. Cached full host build
PID25536 exited0. Control and preparation verification PID8544 exited0:
three native control outputs build through the public wrapper, remain cached,
and reject x64; frontend-request-client-test exercises real exit37 plus its
I/O barrier and error outcomes. The other two images are build-verified only,
not claimed as new GUI/broker integration passes.

With the existing O:/winnt/DOS4GW.PIF as read-only input, all six Console trace
preparation outputs match the fixed b5d636880 script: COM, listing, two BATs,
and two PIFs. The reference command is an audit oracle, not an input to the new
build graph. No installed package, original guest or production algorithm was
modified. All11 raw production inputs and22 dual-location published hashes
remain unchanged; no changed image needs republishing.

[Build receipts](../../../artifacts/build-migration/t484/s3-con-trace/build-result.json)
and retained verification scripts cover this scope. Replay scripts from a fresh
directory directly below build/, not from the evidence directory. Raw logs:
build/t484-s3-con-trace. Current product graph and package identities remain
unchanged. No Windows startup, TSR runtime or normal-shutdown pass is claimed.

After delivery B0111 is complete; B0110 remains pending for configuration
preparation. Fixed355 inventory:158 complete,197 remaining, comprising53
pending producer/staging/verification rows and144 awaiting reconciliation.
