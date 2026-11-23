# T484 S3 P41: PIC and BIOS probe producers

The PIC integration probe now uses the existing CMD/native CPU guest-probe
target. Its221-byte COM is byte-identical to direct NASM production, SHA256
EC00E225DA3D2B6D99DD8F7135EC2BE155E5ED6C362791F92645F45ECCB27F87.
The runner retains its runtime input hashes and owns the producer process,
timeout, logs and exit receipt. Original guest binaries are not rebuilt.

The adjacent slave-BIOS test previously parsed obsolete Ninja variables and
generated CL/LINK response files in Python. Its producer is now a native target
that obtains the actual PIC fixture compilation profile and link inputs, replaces
only the fixture object, and reuses the existing link policy. No duplicate
archive list, new product provider or product/mirror source change is introduced.
Python remains a standalone assertion runner, not a compiler-command generator.

The final native PIC fixture passes24 assertions; the slave BIOS fixture passes79
checks. Existing map checks confirm actual ICA, BIOS, RTC and mouse providers.
The second build reports no work. Existing controlled fixture seams retain the
original /force:multiple link policy and its warnings; these are not a new
product link policy. No new real-DOS IRQ0 interaction pass is claimed: unchanged
COM bytes reuse earlier runtime coverage under the owner's no-repeat policy.

[Run receipt](../../../artifacts/build-migration/t484/s3-pic-bios/result.json)
and adjacent receipts retain exact hashes and results. The slave-BIOS entry is
now migrated; the PIC entry remains pending for shared runtime/staging review.
Cached full product build, unchanged11 raw images and22 published hashes, and
documentation governance are recorded separately. S3 stays active.
