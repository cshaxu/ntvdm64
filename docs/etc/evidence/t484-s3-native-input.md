# T484 S3 P28: native input service producers

Two runtime runners now delegate to CMD native-input-contracts. Four fixture
executables live in the existing WOW32 graph. Membership and lifecycle tests
link its exact production membership object, not a copied or reconstructed
object. The lifetime fixture continues to include the current authored lifetime
implementation with its existing allocation/native-failure seams.

The original standalone membership producer omitted the current compat include
root and failed at its first header. The native target supplies that root;
no source or assertion change was needed. Initial failed compilation remains
in the [process receipts](../../../artifacts/build-migration/t484/s3-native-input/processes.json).

Five final runtime cases pass: private native queue sharing, focus/capture,
membership, full input lifetime, and expected terminal exit91 after the third
attachment fails. [Markers](../../../artifacts/build-migration/t484/s3-native-input/markers.txt)
retain the naive-group counterexample, three allocation failures, idempotence,
external references, rejoin, middle/last exit and zero live allocations.
[Image hashes](../../../artifacts/build-migration/t484/s3-native-input/images.json)
identify the tested outputs. These private nonactive-desktop fixtures are not
real guest input or complete product lifecycle acceptance.

[Cached full build](../../../artifacts/build-migration/t484/s3-native-input/build-result.json)
and [11 raw/22 dual publication identity checks](../../../artifacts/build-migration/t484/s3-native-input/publication.json)
passed. The byte-identical products need no new publication copy.
No product/mirror/guest source changed. S3 remains active.
