# M0 T412 S1 — OS-managed resource attachment feasibility

## Question and authority

The owner approved completing T412 according to its proposal, including the
authenticated resource-transfer exception. The objective remains reducing
mirror/overlay/autonomous differences through original OpenNT owners and
building run16.exe, basesrv.exe and ntvdm.exe. This record tests only a finite
transport prerequisite; its two fixture executables are not those products.

Original `srvvdm.c` retains the policy for standard-stream duplication, paired
wait resources and rollback, as recorded in the
[owner audit](m0-t412-s1-broker-owner-audit.md#resource-transfer-authority-decision-required).
Reopening paths or substituting autonomous I/O pumps would not retain the
original shared object/position contract. Authenticated local RPC system_handle
attachments are an OS-managed binding candidate, not a copied CSR subsystem or
replacement BaseSrv state machine. No imported source was changed by this probe.

## Reproduction

Tracked inputs:

- `tests/broker/resource_attachment.idl`: typed file/event attachments; no
  sender-local handle integer or guest pointer is a message field.
- `tests/broker/resource_attachment.c`: two test-only process roles, a local
  authenticated endpoint and finite resource assertions. The security callback
  checks the caller's impersonated AuthenticationId against the server's logon.
- `tools/audit/Verify-BrokerResourceAttachment.mjs`: build and bounded owned-child
  execution. Run with `node tools/audit/Verify-BrokerResourceAttachment.mjs`.

Everything is generated below `build/M0-T412/S1/rpc-resource`. Each invocation
regenerates MIDL outputs and recompiles every fixture object and executable;
the graph does not reuse old product objects. Build uses MSVC 2022 x86 `/MT`,
SDK 10.0.22621.0 and MIDL `/env win32 /target NT100`. PE machine must be 014c.
The test creates unique endpoint and file names per run. No runtime package
file is modified, and no fixture executable is deployed to O:/ntvdm64.

## Assertions and observations

1. Client writes `A`, leaving its file offset at one, then transfers the file
   and an unsignaled event through local RPC. The server verifies offset one,
   writes `B`, duplicates a return reference and signals the event.
2. Client receives the result after the RPC's input-attachment lifetime ends.
   It sees a signaled event and offset two on its original reference. It closes
   that reference and reads `AB` through the returned resource. This proves
   shared file position and retained return ownership across the call.
3. In a separate process pair the client submits a read-only file reference.
   The server's write fails with ERROR_ACCESS_DENIED; no output resource is
   returned, the event remains unsignaled and file length remains one.
4. Both client and server must exit zero within bounded waits. Forced cleanup
   is allowed only for that exact fixture child after failure and is not a pass.

The positive and read-only-negative cases pass. `shared-result.json` and
`readonly-result.json` retain exact results; `build.log` and per-case server
logs remain in the build directory. No guest workload was executed by this
transport-only test. The existing published product remains unchanged.

## Scope and next gate

This establishes the selected toolchain/OS can convey authenticated local
file/event capabilities without reopening files or authoring a byte-pump.
It does not prove the whole broker, hostile-peer security, pipes/EOF, process
attachments, aliases among all three standard streams, cancellation, concurrent
requests, Console handles or original BaseSrv resource-release ordering.
Those remain explicit composition/integration tests, not waived requirements.

Next S1 work must freeze how the package-private binding materializes original
process-local structures and resource ownership, together with Console identity
and WOW registration, then report the coherent design before S2 source recovery.
The authenticated exception is approved; no further approval request is needed
for sequential S2--S6 work within the proposal. No task closure is claimed here.
