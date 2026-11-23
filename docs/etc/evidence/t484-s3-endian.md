# T484 S3 P35: host register layout fixture

The Python runner delegates compilation to CMD/native fixture targets and
retains assertions. Both x86 and x64 execute the actual register union with
and without the legacy LITTLEND macro. The CPU_30_STYLE negative control must
fail with the current excluded-branch diagnostic, not any unrelated error.

The previous negative test was stale: commit a45513248 fixed the actual host
layout and removed its LITTLEND selection. Therefore omitting LITTLEND no
longer represents a big-endian request and must compile. This migration does
not reinstate a product branch simply to satisfy that outdated assertion.
No production, mirror or guest code changed.

[x86 result](../../../artifacts/build-migration/t484/s3-endian/x86.txt) and
[x64 result](../../../artifacts/build-migration/t484/s3-endian/x64.txt) pass.
The [x86 rejection](../../../artifacts/build-migration/t484/s3-endian/x86-rejection.txt)
and [x64 rejection](../../../artifacts/build-migration/t484/s3-endian/x64-rejection.txt)
show the actual excluded original branch. Compilation failure is intentional
for these explicit negative targets, never a dependency of successful builds.

Cached full build and publication identity evidence accompany this delivery.
S3 remains active; the final clean/interpreter-free acceptance remains S5.
