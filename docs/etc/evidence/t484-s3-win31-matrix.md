# T484 S3 P63 Win31 matrix witness producers

B0071 delegates its two authored test witnesses to the existing CMD/native
fixture graph. W31CHECK.EXE uses win31_startup_contract.asm and W31DOS.COM uses
win31_dosonly_contract.asm with the original NASM flat-binary profile. The
normal target assembles both; an externally supplied WindowsProbeImage selects
the DOS-only target and is copied unchanged. Neither target builds original
guest media. The shared runtime helper records the owned command, PID, deadline
and exit before copying outputs to the evidence root.

The matrix's placeholder uniqueness, 128-byte path bounds, runtime patching,
startup/exit assertions and cleanup are unchanged. Its private-desktop inspector
still compiles a managed helper via Add-Type; dynamic witness path filling and
runtime/package staging also remain. B0071 therefore stays pending, with these
specific remaining dependencies rather than a claim of whole-chain completion.

Owned verification compares both outputs against the previous NASM commands,
checks an external-image copy without executing it, rejects unsupported x64
targets and parses both the runner and helper. Raw commands and logs live in
build/t484-s3-win31-matrix; concise receipts are retained in
artifacts/build-migration/t484/s3-win31-matrix. No new Windows matrix runtime
pass is implied by these producer checks. Final delivery also requires cached
full host build, unchanged dual-publication hashes and governance verification.

Actual verification passed: W31CHECK.EXE (740 bytes) and W31DOS.COM (191 bytes)
match their reference assembler outputs byte for byte. The supplied-image
control remains unchanged, and both unsupported architecture requests reject
with exit2. Script parsing passes. This is not a new Win31 runtime matrix run.

Cached full host build and final verification pass. No-op witness hashes and
timestamps remain unchanged;11 host input hashes and22 dual-publication hashes
match. No product recopy is required. Documentation governance passes.
Inventory remains355 entries:134 completed dispositions and221 remaining
(54 pending producer chains,167 awaiting reconciliation).
