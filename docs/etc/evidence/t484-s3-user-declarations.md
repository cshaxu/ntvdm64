# T484 S3 P134 USER declaration producer and audit selftest

B0200 retains two meaningful compile contracts: repeated original USER passes
preserve CLS/CALLPROCDATA pointer identity, and an excessive FNID range must
fail the original capacity guard. Move its inline generated positive source
to test/opennt-host/user_declarations.c. Native opt-in WOW targets compile it
and project the exact current capacity cohort with FNID_START zero. The shared
projection preserves original diagnostic lines. The verifier invokes CMD,
checks positive success and negative C1189 text, and saves source identities.
It no longer accepts an arbitrary compiler driver or writes compilation source.

First attempt PID27132 rejected an ambiguous #endif projection endpoint.
Use the unique following FNID_DDE_BIT marker, preserving the complete original
conditional. Second attempt PID20880 compiled the positive and rejected the
negative correctly, but a positional Select-String argument was incorrect.
Use explicit LiteralPath/Pattern; do not weaken the expected failure check.
Final runner PID16220 repeats the corrected entry, then cached full build and
11 raw/22 published identity comparisons; receipt records its terminal result.

B0201 test/tools/audit/test_library_build_boundary.py is a standalone audit
selftest, not a producer. Its12 tests replace the audit module's Ninja query
with deterministic command/dependency/input text; no compiler or generator
runs. It verifies B cannot read private C or outer headers even through aliases
and prerequisites. Retain it as standalone audit code under the existing task
scope; its language is not a build dependency. All12 tests pass in this batch.

No original guest, mirror or production code changes. No new Full result;
publication is verified unchanged. S3/S4/S5 remain open. Raw evidence is under
build/t484-s3-user-declarations, including both failed attempts.

[Receipt](../../../artifacts/build-migration/t484/s3-user-declarations/result.json).
