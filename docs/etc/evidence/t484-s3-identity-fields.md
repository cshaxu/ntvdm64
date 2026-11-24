# T484 S3 P180 shared DOS identity fields

## Scope and retention

The Win31/Win95 identity runners exercise per-VM program identity, continuation,
ordered commands and retirement. They are maintained focused tests, not all
mandatory calls of the Full runner. Retain their behavioral assertions; merge
the duplicated PowerShell COM mutation into the existing native
win31-probe-paths target. This is a producer migration, not a new product fix.
No original guest, mirror, published product or installation is modified.

The binder now shares its unique-marker/bounds logic across the existing
128-byte controller paths and the authored DOS templates. Identity mode admits
only the declared markers from dos-identity.asm/dos-retirement.asm. It writes
one role byte,128-byte paths,256-byte commands and a512-byte environment block.
Environment entries remain ordered and double-NUL terminated; command quotes
and the ordered-command trailing CR remain intact. All requested fields are
validated in memory before the output is opened. Disk write failure is not
claimed atomic. Inputs use the existing narrow ASCII fixture-path contract;
Unicode command-line round-tripping is not claimed.

Both callers keep their existing flags, command selection, immutable nested
MEM/COMMAND copies, guest directories and runtime assertions. The retained
PowerShell runners choose test parameters but no longer construct COM bytes.
PIF/config and shared runtime staging are still pending; neither original
candidate is marked complete. Current fixed census remains315/355,40 remaining.

## Verification

Extend the existing test/build/verify-win31-probe-paths.ps1 suite, using the
same CMD/CMake/NASM producers and stable caches. Seven actual COM variants
(base, continuation, nested, ordered, Win95, Win95 native, retirement), each
with A and B roles, match the prior clear/copy algorithm byte for byte across
the complete image. Thus role padding and all non-field bytes are preserved.
Nine COM rejection cases cover role, unknown/duplicate marker, path/command/
environment capacity, malformed environment entry, control character and odd
arguments; each leaves a preexisting output unchanged. Existing two-image and
controller tests also pass, including six real controller images and nine
older rejection cases.

First run build/t484-s3-identity-fields PID14864 fails only the attempted
Unicode rejection assertion: Windows narrow argv converts the input before
main. Preserve that failed evidence; replace the invalid assertion with a
literal LF, forbidden by the actual ASCII field contract. No product runtime
failure is inferred. Final build/t484-s3-identity-fields-final PID29932 exits0
in26.1seconds. This proves binary construction equivalence, not a new full
Win31/Win95 runtime acceptance run.

Cached full and11 raw/22 published identity verification: PID11724, same final
evidence directory. Delivery and governance receipts record final results.

PID11724 completes in64.3seconds, exit0; all11 raw and22 published hashes
match. Reuse unchanged coherent publication; no redundant install writes.
PID25580 parser/governance/diff checks pass in24.6seconds. The containing
P180 commit is the delivery identity; S3 remains active.
