# T484 S3 P135 retire old closure/accessor research paths

B0202 Audit-ProductDependencyClosure.ps1 is retired. Its historical seven-image
walker recurses only through .obj/.lib inputs, ignores CMake phony and generated
object-library nodes and assumes old single-root product placement. Changing
only its default root would produce incomplete closure data. There is no current
source/build caller. Preserve historical reports; do not present this old
parser as a current all-component graph proof.

B0203 AuditCvidAccessorProfile.mjs is retired. It generated compiler scripts
from removed graph variable spellings and a separate38-pair accessor fixture.
The current native tools/build/native/cvidc.cpp already derives those38 cases
and original setter offsets. test/mvdm-host/cvidc_vector_binding_fixture.c
executes the same field-isolation and scratch-preservation checks with four
values per accessor, using original production libraries and additional vector,
selection, memory and concurrency checks. Keep that owner and remove the second
historical generator. Old nine-file preprocessed consumer inventory was research,
not a current regression; its history remains, without a new coverage claim.

The batch runs cached full host build and11 raw/22 publication identity checks,
then builds and runs the existing native cvidc-vector-binding-fixture.exe with
an owned60-second deadline. This is bounded actual-CCPU coverage, not Full or
guest acceptance. No product/mirror/guest changes or new producer. Raw runs are
under build/t484-s3-retire-closure; terminal result is recorded in the receipt.

B0204/B0205 were inspected as read-only Ninja evidence collectors but are not
closed by this batch: their historical census/plan assumptions still require
retention review. S3/S4/S5 remain open.

[Receipt](../../../artifacts/build-migration/t484/s3-retire-closure/result.json).
