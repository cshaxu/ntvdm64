# T484 S3 P120 shell callback and expired source audit

B0193 retains four current x86 native-provider contracts: adapter, adapter outer
unwind, native three-argument callback, native outer unwind. The adapter's
scoped test TU includes the actual product implementation to inspect its private
TLS, compiled with the actual product object's COMPILE_FLAGS. No production
export or test branch was added. Source/ABI scope remains synchronous and
thread-local. Real SHELL32 supplies the provider; these are not Win16 acceptance.

Remove the old Ninja cflags parser, supplied provider-build path and manual
compiler commands. Retire the wrong-cdecl experiment, direct two-argument
mismatched-provider experiment, artificial direct nested throw mode and raw
return-instruction-byte dump. Their historical evidence is preserved. Current
success/failure, nested adapter, exception propagation, TLS restoration and
subsequent-call assertions remain. Four explicit Case values replace the old
combinatorial switches; the default is the actual Adapter contract.

Owned PID18940 exits0: all four actual provider cases, cached full host build,
unchanged cached target hash/mtime, x64 rejection2,11 raw host identities and22
published copies unchanged. Existing dual publication verified, not republished.
No product code, mirror, original guest or installed runtime change.

B0194 verify-wow-source-registers.ps1 is removed, not migrated. It has no current
caller, selects zero objects using obsolete Ninja ': cc' rules, while the current
graph has native CMake C compiler rules (count in receipt). It also requires
inline DIVERGENCE IDs against old component README exception lists, superseded
by the mirror-register migration in docs/rules/CODING.md. Historical reports
remain evidence. This retired read-only script was not a build/test prerequisite;
its deletion after the full build does not change product build inputs.

[Receipt](../../../artifacts/build-migration/t484/s3-shell-callback/result.json)
records owned validation, all four transcripts, ABI profile, cache and retired
selector evidence. Raw: build/t484-s3-shell-callback. No retrospective all-tools
completion claim. S3 remains active; B0166/B0169 and S4/S5 remain open.
