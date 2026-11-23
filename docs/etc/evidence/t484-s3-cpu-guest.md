# T484 S3 P36: shared CPU and EMS guest probe production

Four integration runners now delegate their independent probe assembly and
PIF generator compilation to tools/build-fixtures.cmd x86 cpu-guest-probes.
The native graph contains23 COM variants, including existing negative-oracle
defines, and reuses the original ems_pif_builder target. Two EMS callers no
longer pass the retired CompilerDriver option. Original guest, product and
mirror sources are unchanged.

An independent NASM invocation for each original source/define combination
matches all23 native outputs byte for byte. Source paths in three old CPU
runners incorrectly pointed into integration rather than observation; the
native graph uses the actual unchanged sources. The runners retain source,
oracle and output hashes, negative-data behavior and existing assertions.
The PIF generator is unchanged and reuses the P8 profile-equivalence evidence.

The actual EMS Mapping runner passes its original INT67 marker and normal
launcher exit using the native producer. Its first attempt was rejected
before guest launch because the supervising invocation supplied O:/winnt
instead of the required build-owned package. The corrected run uses the
existing S2 worker package after all11 images match the current published
hashes. This is an invocation correction, not a guest/product repair.
It does not constitute a new full CPU or product runtime acceptance run.

Cached full host compilation passes. Final receipt checks11 raw inputs and22
published hashes; unchanged images require no recopy. The six edited scripts
also pass parsing and the documentation gate is recorded in the final receipt.
The initial documentation check preceded the new evidence index update and
failed; the final check runs after that update, with both receipts retained.
No diagnostic or test binary is published as a product image.

These four runners remain pending in the complete-chain ledger. LogonHelper
can be C# source compiled by Add-Type or an already-built assembly, and the
shared package helper still has interpreted identity/staging work. Those
dependencies must be reconciled rather than hidden behind the CMD producer.
The next original DIR-ordered candidates now have23 additional explicit
producer findings; absent search hits are not used to classify a file as
complete. P35's delivered status is also corrected in the ledger.

[Comparison and adjacent receipts](../../../artifacts/build-migration/t484/s3-cpu-guest/comparison.json)
retain the23 hashes, process ownership, full build and runtime results.
The containing commit delivers P36. S3, S4 and S5 remain incomplete.
