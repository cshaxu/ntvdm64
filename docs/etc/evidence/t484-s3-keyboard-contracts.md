# T484 S3 P27: native keyboard contract producers

Three interpreted producers are consolidated into the CMD keyboard-contracts
target and a runtime-only runner. Four fixtures compile current complete
function projections and, where required, the actual B history carrier.
All24 old/new extraction comparisons (21 unique projections) are LF-identical.
No product, mirror or original guest code is changed.

The first run passed three fixtures; unused_key_return failed with stack
corruption. Its old test directly passed101 to an internal100-record array.
The actual caller uses PendingKeyboardHistory, whose selected bitmap counts
at most MVDM_KEY_HISTORY_CAPACITY (100). The remaining KeyQueue.KeyCount has
only a zero assignment in current production. The invalid direct test input
was removed, not declared passing. The complete origin/count/return test now
retains203 device origins and proves that only the latest100 records return
in order. This is valid-caller coverage, not arbitrary-input safety proof.

All four final fixtures pass, including the six direct return cases, actual
history wrap/selection and device-origin/BIOS join contracts. Controlled device
and Console providers mean this is not real guest keyboard acceptance.

- [Initial failure](../../../artifacts/build-migration/t484/s3-keyboard-contracts/initial-result.json).
- [Final runtime result](../../../artifacts/build-migration/t484/s3-keyboard-contracts/runtime-result.json).
- [Projection comparison](../../../artifacts/build-migration/t484/s3-keyboard-contracts/projection-comparison.json).

[Cached full host build](../../../artifacts/build-migration/t484/s3-keyboard-contracts/build-result.json)
and [11 raw/22 published identity checks](../../../artifacts/build-migration/t484/s3-keyboard-contracts/publication.json)
passed. Unchanged images require no new publication copy.
S3 remains active; S4/S5 are not complete.
