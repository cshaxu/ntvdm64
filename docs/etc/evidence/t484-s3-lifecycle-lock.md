# T484 S3 P129 current retirement lock producer

RetirementLockProbe retains its meaningful current concurrency contract: a
competing thread must fail to acquire the runtime exclusive lock while the
actual task-retirement scan runs. This is not a substitute for normal cleanup
or a test of a copied lock algorithm.

The native producer reads the current lifecycle source, selects retire_task,
requires exactly one scan observation point, and adds its existing test call
and declaration. It tracks the source as a configure input. Removing these
two additions recovers the normalized current source exactly. The actual owner
compiler profile is reused; one archive member replaces the ordinary owner.
No production source, mirror or original guest is changed.

Remove RetirementProbeSource and the fallback for a historical scan in
retire_current_thread. Arbitrary old implementation comparisons are not a
current acceptance capability. Remove the two old script producer branches
(35 lines after this CLI cleanup), preserving the original runtime assertions.

PID19692 passes the current concurrency witness three times, ordinary task
lifecycle assertions, cached full build and11 raw/22 published image identity
checks. Publication is verified unchanged, not repeated. The final run after
producer deletion is separately recorded in the receipt. Existing imported
header/link warnings remain visible; no duplicate-provider override is used.

[Receipt](../../../artifacts/build-migration/t484/s3-lifecycle-lock/result.json)
and build/t484-s3-lifecycle-lock contain the actual runs and projection check.
B0195 remains incomplete:28 normal native entries, one consolidated alias,
three native fault switches; dialog-lifecycle and six other fault switches
remain. File dispositions remain225 complete/130 open out of355. S3/S4/S5
remain open; no final clean/interpreter-free acceptance claim is made.
