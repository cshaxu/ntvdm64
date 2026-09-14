# T412 S3 reusable RPC security boundary

## Question and scope

Can the S1 resource-transfer proof supply a reusable authenticated broker
boundary without reproducing BaseSrv policy or NT4 CSR internals?

The approved source-recovery boundary stops at CSR transport. Original BaseSrv
record/selection/cleanup functions remain in opennt-host. Native local RPC
authentication is therefore a finite modern mechanism, not recovered source
or a new task policy. rpc_security.c/.h under broker replaces the test-owned
authentication body; it is not yet linked into a product executable.

## Implementation and ownership

The owning process captures its token AuthenticationId and session once into
immutable native-only scope. Authorization requires WINNT packet privacy,
impersonates the authenticated RPC client, queries that actual token, compares
both values and reverts before returning. It never trusts a copied user key,
PID or session in command data. Token handles are closed on the queried paths.
Failed RPC reversion denies and attempts RevertToSelf; failure of both invokes
fail-fast rather than returning an impersonated pooled thread. That exceptional
failure branch is not experimentally verified.

The interface still uses local-only and secure-only registration. Microsoft
[RpcServerRegisterIfEx documentation](https://learn.microsoft.com/en-us/windows/win32/api/rpcdce/nf-rpcdce-rpcserverregisterifex)
describes the interface authorization callback and its per-client/interface
invocation. Its result does not replace per-command worker/operation authority.
Production registration, endpoint security and operation-specific checks remain
S3 work; this module alone does not establish a complete secure broker.

## Procedure and observations

Run node tools/audit/Verify-BrokerResourceAttachment.mjs. Outputs now use
build/M0-T412/S3/rpc-resource, preserving the earlier S1 root. The test compiles
the actual broker security source, generated MIDL stubs and owned hidden x86
helpers. Existing direct/relay file position, returned ownership, read-only
denial and absent-target checks are retained. New mismatched expected logon
and session cases use actual RPC callers and deliberately altered server scope;
they are controlled predicate negatives, not real second-user/session tests.
Rejected requests leave file/event state unchanged and the server listening.
Only that owned test helper is terminated during cleanup, not a product worker.

An initial integrity-level request test incorrectly expected rejection. It
actually completed. After fixing log capture to await process close/pipe drain,
the server reported AUTH level=6 service=10: WINNT packet privacy. The revised
test requires that observed level and normal transfer behavior; authorization
has not been relaxed. This local runtime observation does not prove that an
actual lower-level incoming call would be accepted, nor that another protocol
has the same protection behavior.

Every run records client/server results and drained server output. All eight
final cases pass, with status retained in per-case result files; no guest program is
executed. Three-product links, real request dispatch, resource receipt lifetime,
unauthenticated peers, genuine other-user/session access and reversion failure
injection remain outstanding integration/security gates.
