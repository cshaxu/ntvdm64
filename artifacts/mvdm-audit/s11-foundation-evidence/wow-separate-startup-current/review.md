# Separate WOW startup after launcher correction

On the current sealed package, a native hooked test program used
`CREATE_SEPARATE_WOW_VDM` to start two hash-identified prebuilt Win16 guests.
The witness observed two distinct NTVDM worker processes and both startup-only
launchers return while their workers were alive. Each guest received its own
idle callbacks; after one worker exited, the peer continued, and both worker
handles subsequently reported exit zero. The test output, guest/task/process
identities, input hashes and sealed package manifest are retained here.

The existing test runner now optionally accepts prebuilt guests with two
mandatory SHA-256 values, so this case can be rerun without assembling guest
code. A missing-hash invocation was rejected. The source files listed in the
test inputs do not by themselves prove the prebuilt guests' build provenance.
No guest or installed image was modified.

This verifies successful default startup and retirement for two separate WOW
workers. It does not independently inject a separate-WOW `--wait` startup
failure or cover all task lifetimes, and it does not resolve the separate
intermittent SOL 1400.
