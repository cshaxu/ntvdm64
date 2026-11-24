# T484 S3 P141 native resource attachment producer

B0219 retains meaningful cross-process resource coverage. Move MIDL and all
five compilation units plus two links into worker/resource-tests.cmake and
`tools/build-fixtures.cmd x86 broker-resource-fixtures`. Formal worker archives
continue to provide BaseSrv dispatch, registry, RPC peer authorization, receipts,
codec and supporting libraries. Native dependency edges cover the IDL/header/
stubs and selected libraries; no old graph parsing or manual compiler scripts.
The JS file is now solely a bounded process/assertion runner accepting binary
and fresh output directories. Its final-map owner checks remain unchanged.

Two stale fixture assumptions, not product failures, were corrected:
- PID23076 compiled but server scope capture failed before READY. That high-level
  helper now validates an entire published package. These PID-qualified local
  test endpoints exercise peer identity and resources, not package discovery.
  Use the existing broker_rpc_token_scope on a real process TOKEN_QUERY handle,
  close it immediately, and retain actual authorization/peer-process providers.
  Published package identity behavior remains in its separate package tests.
- PID32620 reached real authenticated transfers, registry and revocation, but
  its handwritten Check envelope omitted console_mode and used the obsolete96
  byte prefix. Add the current field and derive payload offset with offsetof;
  do not alter product decoding or acceptance. Remove the exactly duplicated
  invalid-handle/receipt-exhaustion block and duplicate protocol include.

PID15936 passes all nine cases; final runner PID22776 repeats them with child PID records: shared, readonly, relay, relay-readonly,
unavailable, wrong-scope, wrong-session, low-auth and wrong-peer. Positives
require original Check deep-copy/abort, first-VDM transitions, receipt role,
generation/repeat revocation and registered-process drain. Rejection cases
leave servers listening until owned cleanup; that is not normal termination.
The low-auth request is observed as Windows packet privacy, not proof that a
weaker authentication level was accepted. No guest code runs in these tests.

Run after the CMD build:

```text
node tools/audit/Verify-BrokerResourceAttachment.mjs build/.ninja/msvc-x86-release/worker/resource-tests build/resource-attachment-proof
```

Cached full product build and11 raw/22 published-copy hash checks follow the
final fixture version. No production/mirror/guest modification, no new Full
runtime pass, no changed product publication. S3/S4/S5 remain open.
Raw evidence: build/t484-s3-resource-native (cases4 is the final passing run).
[Receipt](../../../artifacts/build-migration/t484/s3-resource-native/result.json).
