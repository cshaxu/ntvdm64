# T484 S3 P140 retire historical classifier diagnostics

B0218's23-line JavaScript builder and53-line path_name_abi.c observed a modern
NTDLL write beyond the original12-byte prefix and raw section status values.
They require the old O:/ntvdm64 installation and do not test today's adapter.
The maintained test/compat/base_classifier_path.c instead links the actual
x86 worker/x64 launcher provider objects and checks original-layout guards,
full-path/native equivalence, relative paths, null output and invalid input.
P139 also exercises original GetBinaryTypeW on actual DOS/WOW/PE/DLL/suffix
inputs. Retire the historical diagnostic rather than preserve a second builder.
This does not claim the raw native NTDLL behavior is a current acceptance gate.

B0220's94-line Python audit requires legacy_classifier_arch.h and its pinned
hash/old cc graph spelling. That header no longer exists: the scoped provider
is compat/base_classifier_path.c with a supported full-path call and original
output layout. No production/test caller refers to this old audit or header.
Remove the obsolete assertion, not the current adapter/guard coverage.

The checked-in verify-base-classifier-path.ps1 runs both architectures through
CMD/native targets and checks input hashes. Both have3 cases,2 successes and1
rejection, zero failures, layouts12/24. Cached full product build and11 raw/
22 published-copy identity checks follow removal. No product/mirror/guest
changes, no republication and no Full runtime claim. B0219's distinct resource
attachment behavior is not retired or marked complete by this decision.

Raw evidence: build/t484-s3-classifier-retirement.
[Receipt](../../../artifacts/build-migration/t484/s3-classifier-retirement/result.json).
S3/S4/S5 remain open.
