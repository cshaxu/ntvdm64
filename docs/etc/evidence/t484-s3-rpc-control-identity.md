# T484 S3 P169 isolated RPC control package identities

B0042's only build wrapper dependency was removed by P168. Validate the current
Full six control cases and the three independently copied worker-substitution
cases. First run: build/t484-s3-rpc-control, PID19188. Full six pass; admitted-vdm,
frontend-io-fault and worker-console-exit all fail at the initial execution
connection (fixture line189), before their scenario logic. Every copied image
has the same valid identity as the preceding package, so this is not a missing
identity resource.

The runner created separate roots but reused one identity/RPC endpoint. Fix
both independent-copy branches to assign a fresh GUID to every copied image,
then perform the existing test-worker substitution. Do not change product
connection, package validation, fixture assertions, guest or test timeouts.
The endpoint is keyed by session/logon/package ID in the actual common client;
separate test packages must not share it. We did not collect the old service's
individual RPC return code, so do not invent an exact rejection code.

Intermediate PID17640 run failed before the three scenarios with native error87:
the first branch mistakenly used the second branch's file-list variable. Keep
that evidence in build/t484-s3-rpc-control-unique. Correct the first branch to
use its own image/siblings list. No intermediate candidate was published.

Final PID16836, build/t484-s3-rpc-control-final: all nine actual scenarios pass.
The formerly failing admitted case reports exact exit7 and worker assertions;
frontend I/O failure and console exit also complete their original assertions.
Only six are selected by current Full; the three extra cases are separate
focused coverage, not a claim that the entire optional RPC matrix was run.
Cases/result JSON, observer console assertions and process records retained.

Final cached full/publication identity verification is build-check.ps1,
cases-result.json; all11 raw inputs and22 published hashes unchanged. No new
product publication. The documentation gate is closure-result.json. Disposable
package copies from all three terminated runs are removed after verifying no
owned runtime remains. S3 remains active;308/355 disposed,47 remaining.
