# T484 S3 P168 direct native package identity

Remove the now-empty Set-PackageIdentity.ps1 build wrapper. Ten call sites in
nine files call the existing ntvdm-package-id.exe directly, normalize the
existing GUID input to its unchanged36-character form and explicitly reject
nonzero native exit status. The tool already supports multiple input images;
no new provider, native algorithm or product code is added. All test/tool
references to the removed wrapper are gone. Stage-System32 remains a pending
S4 assembler, but its identity operation no longer requires the wrapper.

Also delete Stage-T228S1GuestExecIntegrationInputs.ps1 (45 lines): no current
consumer; its fixed T225 build paths require obsolete source-built original
guest binaries, synthesize the old runner-v8 Direct/Readonly profile, and
explicitly forbid release/default use. It is not current immutable media or
an active test fixture. Keep historical provenance in Git and inventory; do
not rebuild these guest inputs to preserve an expired tool. Together the two
deleted scripts remove59 lines; caller substitutions add10 net lines.

Validation: build/t484-s3-direct-identity, supervisor12488/child22304. Parse all
changed PS scripts; actual package-identity fixture passes accepted/mixed/
unstamped cases; native empty-GUID/missing/locked writes reject and preserve
existing bytes. Existing fresh-stage test passes two independent RPC packages,
retirement/reconnect/no-fallback, and both DOS sessions. Cached full and11 raw
input/22 published hashes pass unchanged. No product publication required.

Additional normal-user-environment run rpc-waits: supervisor5780/child31232.
All five actual reply, connect, disconnect, loss and bootstrap console markers
pass with the directly stamped fixture and fresh package. B0031 is complete.
B0337 and B0344 are retired prerequisites of the later S4 chain, completed
within current S3 shared setup cleanup; S4 itself is not admitted or closed.
Other changed callers are syntax/audit checked, not claimed to have their full
DOS/Win31/Full suites rerun. Their separate pending obligations remain.

Input records, process records, result JSON, logs and runtime manifests remain
under that batch. Owned package copy is removed after both runs terminate;
scoped stages use the existing cleanup. closure-result.json records governance.
Completion is307/355 with48 remaining. No original guest or release asset changed.
