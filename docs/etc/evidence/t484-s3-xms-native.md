# T484 S3 P133 native XMS boundary producers

B0199 remains useful: allocation covers the original XMS block owner and real
suballocator; move covers forward-copy and failure termination control flow.
They are bounded leaf contracts, not real CPU execution or product shutdown.

Replace the PowerShell Ninja-command parser, compiler-shell generation and
manual compile/link calls with xms-allocation/xms-move CMD targets. Native
CMake obtains COMPILE_FLAGS directly from unit_obj_xms_xmsblock_obj, compiles
the current original provider with function-level selection, and links the
existing original-mvdm-host-suballoc archive. Out-of-scope dependencies keep
abort seams; no success substitute is added. Preserve source-owner map checks,
assertions and source/binary identities. Remove the unused candidate-source
selector: this maintained verifier checks current production source.

PID19888 stopped before compilation because the batch supplied an absolute
output to the existing repository-relative runner contract. Correct the batch
arguments, not the test assertions. PID18460 passes135 allocation and8 move
assertions, including two invalid-address termination cases, then runs cached
full host build and11 raw/22 publication hash comparisons. Actual terminal
result is in the receipt. Existing imported header warnings remain visible.

No original guest, mirror or product source changes. Publication is verified
unchanged, not copied again. This is not a Full runtime result; S3/S4/S5 remain
open. Raw evidence: build/t484-s3-xms-native.

[Receipt](../../../artifacts/build-migration/t484/s3-xms-native/result.json).
