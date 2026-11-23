# T484 S3 P10: native system fixtures

The NT I/O facade, VDM control and DOS find-layout producers now use
`tools/build-fixtures.cmd` and the shared native CMake graph. The original
source lists, architecture restrictions, forced header, C language settings
and link libraries are retained. DOS find-layout remains x86-only; the other
two fixtures run on x86 and x64. No product or mirror source changes.

Three PowerShell generators are removed after their replacements compile and
execute. The fourth, New-T282CommandRegistersNinja.ps1, refers to a missing
implementation and header. Its fixture only supplies obsolete machine-facade
symbols and has no current production provider or consumer. Both are retired,
not counted as passing coverage. New-T281OriginalBopDispatchNinja.ps1 still
references the missing header and remains a separate outstanding review.

The first candidate attempted to include that obsolete register fixture and
failed during CMake generation. Its receipt is retained. The final candidate
passed all five executable instances. The shared link helper now accepts
explicit additional libraries; redirector async was rebuilt and passed its
27 assertions as a regression check of that helper.

Cached full host build PID20096 passed. All11 raw host images and22 published
hashes match the current package. No changed image required republication;
no new product runtime or full regression pass is claimed. The package remains
d8983d2b-3fe1-4d18-8ea9-64613c84346c at both destinations.

[Receipts](../../../artifacts/build-migration/t484/s3-system-fixtures/final-result.json)
identify the final fixture outputs; adjacent initial, product-build and finish
receipts distinguish the failed candidate, host build and publication checks.
The containing commit is this batch's delivery identity. S3 continues;
remaining producers, S4 publication migration and S5 acceptance are pending.
