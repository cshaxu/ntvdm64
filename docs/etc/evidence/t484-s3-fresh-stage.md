# T484 S3 P158 fresh package isolation

The shared staging helper and fresh-stage test retain useful independent-package
coverage. Add the unchanged authenticated RPC client to the native worker graph
as package-rpc-client; build through tools/build-fixtures.cmd x86 package-rpc-client.
No private compiler invocation or duplicate RPC implementation is added.

Both isolation tests still required exactly10 manifest rows, rejecting the
current11-image package before runtime. Update that obsolete count to11;
retain separate UUID, actual connection and retirement assertions. This changes
test package validation, not the product's existing ten-image scope contract.

Background build PID11360 passed. Real private-desktop run PID25284 passed:
two independent authenticated clients; terminating service A leaves B connected
and reconnectable; A cannot fall through to B; both concurrent DOS COMMAND /c exit
sessions return zero. The checked-in fresh-stage runner removed its owned stages
O:/tmp/n847 and O:/tmp/n616. Source guest bytes were copied unchanged from the
existing candidate-final fixture; all11 host files came from assets/release.
Raw evidence: build/t484-s3-fresh-stage/runtime-result.json and cases/.

This closes B0015 and B0104, including P157's native identity reader. It does not
close B0017's old idle-test producer, S4 publication orchestration or Full/S5.
Disposition totals:288/355 complete;67 remaining. Cached full and publication
identity results are in build/t484-s3-fresh-stage/cases-result.json.
