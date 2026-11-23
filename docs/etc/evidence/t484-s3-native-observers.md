# T484 S3 P7: native observation helpers

Four interpreted test producers are replaced by native x86 targets:
COMMAND native stdout child, stream echo, ConPTY stream observer, and the
keymouse console observer plus its authored DOS probe. The original sources,
optimization/warning/runtime settings and CL-driven link library policy remain.
command-tools and guest-keymouse are the documented CMD selections. There
were no executable callers of these four old producer scripts.

The four new PE files differ from binaries actually built by the old scripts
only in verified COFF/debug timestamp fields. KMTST.COM is byte-identical.
Native stdout, stdin echo, argument rejection, child waiting/exit and ConPTY
control output are exercised against both baseline and migrated binaries.
This does not claim a new guest mouse interaction acceptance run.

The initial added assertion incorrectly expected cmd.exe echo text inside
the ConPTY transcript: both binaries put it on inherited stdout, while the
transcript receives console control sequences. That failed attempt is kept
in result.json. The corrected same-environment comparison in behavior.json
checks the actual channels and passes; no observer or product change was
made to hide the failure.

Cached full host build PID7616 passed. The shared fixture linker change is
also checked with six x86/x64 guest-memory executable cases. Eleven raw host
images and22 published hashes remain unchanged, so no product republication
is required. No original guest, mirror or product source changed.
[Evidence](../../../artifacts/build-migration/t484/s3-native-observers/behavior.json)
includes the adjacent PE comparison, initial attempt, build and final receipts.
The containing commit is P7's delivery identity. S3 is still active; other
test producers, S4 publication and S5 final acceptance remain outstanding.
