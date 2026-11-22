# T484 S2 P2 native post-link storage check

The production NTVDM link now runs ntvdm-tib.exe instead of Node and
Verify-VdmTibStorage.mjs. The old script is removed. The native checker reads
the actual x86 COFF and linked MAP: VdmTib must be a real dpmiint.obj
definition, exceed0xfc0 bytes, have no overlapping linked symbol, and retain
the startup binding. Truncated input and invalid COFF names/auxiliary spans
fail. This is a fixed binary validator, not build orchestration.

The two instrumentation callers use the same checker. Their stale
adapter-monitor/mvdm_vdm_tib.obj inputs now point at the existing production
obj/dpmi/dpmiint.obj. No runtime behavior, mirror code or original guest is
changed. Non-error mirror delta:0points/0lines.

## Verification and delivery

- Native MSVC build PID31936 passed against the actual product:4208-byte
  VdmTib, no overlap.
- test/tools/build/verify-tib-check.ps1 covers ten cases: actual product,
  wrong owner, missing startup binding, overlap, three truncation sizes,
  wrong machine, COMMON definition and short storage. All passed in PID25928.
  Earlier test-runner attempts exposed an empty-byte-array construction and
  propagation of the expected negative exit code; both were corrected.
- Final cached full x86/x64 build PID25928 passed,2026-11-22T16:31:25Z to
  16:33:35Z. The production link executed the native checker successfully.
- Raw comparison against S2 P1:5 images identical;6 differ only in PE timestamp
  fields,4 bytes each. This is not an equal-raw-hash claim. Runtime evidence
  is reused under the owner's timestamp-only authorization.
- Publication PID8724 passed:11 images,22 verified destination hashes,
  package d4f65024-538c-4365-a9df-2240cd637f0b. No installed process was stopped.
- Post-publication PID32296 passed the linked C-VID contract, strict9-graph
  library boundaries and all22 published hashes. This is not a new Full run.
- Documentation, relative-link and diff gates passed in PID27740. The
  containing S2 P2 commit is this batch's delivery identity.

Compact records are under
[s2-native-tib](../../../artifacts/build-migration/t484/s2-native-tib/verification-result.json).
Standalone assertions remain PowerShell; they are not build prerequisites.
The full graph generators and publishing implementation still need migration.
This P does not close S2. Task-local copies remain bounded under
build/t484-s2-tib and are disposable after retained evidence and commit.
