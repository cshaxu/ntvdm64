# T484 S3 P214: remove obsolete runtime compiler lookup

The Win95 editor and identity runners finish their native fixture production
and path binding before scrubbing the runtime environment. Both then replace
PATH with the isolated package system32 and Windows system directories before
launch. The intervening NASM lookup and PATH prefix have no remaining consumer.
Remove those two lookups; retain compilation, staging, input, timeout, cleanup
and assertion behavior. No product or guest source/image changes.

The Win31 counterparts are not changed: their subsequent matrix runner invokes
Invoke-Win31ProbeBuild. Its native producer and toolchain discovery must be
accounted for before removing that environment prerequisite. This review does
not close the four full caller chains or the common package preparation work.

Owned run build/t484-s3-runtime-tool-path/run.ps1, PID15036,
2026-11-24T14:28:43.7550044Z through14:29:44.4068842Z, exited0.
Both changed scripts parse; comparison against pre-change HEAD proves the
runtime suffix is unchanged. Cached full host build passes; the existing
publication identity check verifies11 raw images and22 published copies
unchanged. No fresh Win95 runtime/Full pass is claimed. Final governance,
tracker and diff checks precede commit/push.

Move completed P181-P213 chronology out of Current into the existing indexed
delivery record, retaining every row and current obligation. Frozen census
remains319/355 disposed,36pending;23 supplemental preparation chains remain.
The native-output runtime failure remains open.
