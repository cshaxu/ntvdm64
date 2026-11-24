# T484 S3 P159 obsolete idle runner retirement

B0017 was an uncalled standalone JS builder/runtime test for a former x86 broker,
10-second idle period and synchronous Server_Connect. Its private-main fixture
references removed idle_deadline, pending_connects and idle_stopping globals.
Current main.c owns BASESRV_IDLE_POLICY, uses five-second grace and asynchronous
Server_Connect. Restoring the old graph or expectations would test obsolete behavior.

Delete verify_ntsrv_idle.mjs and ntsrv_idle_test.c (263 lines). Keep the actual
ntsrv_idle_client.c used by the native package-rpc-client target and P158's real
RPC isolation. No current tools/test callers reference either removed file.
No production, guest, published image or product build graph changed.

Current temporal requirements belong to test/adapter-basesrv/idle_policy_test.c:
exact5s, before/at expiry, no renewal, pending admission cancellation, rearm,
occupied service, stopping and overflow. Background PID27140 rebuilt that existing
native target through its CMD runner and passed all assertions. Actual runtime
retirement remains test/integration/verify-broker-retirement.ps1 in Full; this
batch does not claim to rerun Full. The old fixture's timer/RPC-stop API fault
injection was already incompatible and is not claimed as current passing coverage.

Evidence: build/t484-s3-idle-retirement/result.json, stdout.log and build.log.
Production hashes/publication remain P158; no redundant product build/publication.
Disposition totals:289/355 completed,66 remaining. S3 continues.
