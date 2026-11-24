# T484 S3 P151 broker protocol producer consolidation

## Disposition

B0238 and B0239 duplicated private CL-command generation using frozen T412
paths. Their underlying delivery/receipt and payload unit contracts remain
useful. Consolidate runners into test/broker/verify-protocol-contracts.ps1,
which delegates compilation to tools/build-fixtures.cmd x86 broker-protocol-fixtures.
Reuse the existing vdm-protocol-test target and its current transport archive;
add only the delivery-journal fixture to the same worker graph. Both fixture
sources remain unchanged. Remove the two old JavaScript producers.

Preserve x86 PE identity, actual map-provider assertions, ten-second owned
process deadlines, exit propagation and all original positive/negative fixture
assertions. The payload provider assertion now names its actual owning
common-base-srv-transport archive instead of the retired broker-transport
layout. The runtime runner does not generate source or compiler commands.

## Verification

Owned background PID15012 builds both targets and passes both executables:
receipt ownership/alias/inheritance, partial rollback, retry retention,
commit/sender survival, pipe EOF, eleven operation envelopes, eight fields,
short-buffer nonmutation and truncation/overlap/overflow rejection. The maps
verify six selected provider symbols. These are bounded unit contracts, not
real guest/RPC integration or a Full suite.

PID15264 passes cached full x86/x64 build and identity checks for11 raw host
images and22 published files. All are unchanged; no republication claimed.
No product source or original guest changed.

[Receipts](../../../artifacts/build-migration/t484/s3-broker-protocol/result.json).
Two inventory entries completed by consolidation:268 of355 complete,87 remain.
S3 continues at B0240, with prior unresolved cases and S4/S5 still open.
