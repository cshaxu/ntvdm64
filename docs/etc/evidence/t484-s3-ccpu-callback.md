# T484 S3 P21: native CCPU callback producers

The CMD ccpu-callback group replaces the T313 PowerShell and T335 JavaScript
generators, reusing current worker profiles and archives. The exception
observer also has a native target. No original guest or product source changes.

The old fixture needed current WOW include precedence and the original
stdcall entry. Its fail-fast stack resolved to host_sas_init/InitIntelMemory:
session binding must precede SAS allocation. The subsequent null call resolved
to original CallBack16's setSS: the public CPU vector was not initialized.
The fixture now uses ntvdm_dpmi_initialize and mvdm_cvidc_bind_vectors.
The intermediate SAS-only initialization did not fix that call and is not
retained. Product failure behavior and test assertions were not relaxed.

The shared CCPU fixture link policy retains /force:multiple and bounded seams
ahead of host archives. These are explicit test substitutions, not production
link evidence. The real-mode pointer and TEB seams prove frame fields and
nested CCPU return ordering, not a full WOW16 trampoline, protected-selector
translation, USER lifecycle or guest acceptance.

Final entry: test/observation/verify-ccpu-callback.ps1. Both callback and
bounded CCPU executable assertions returned zero. Their hashes and PIDs are
in the [result](../../../artifacts/build-migration/t484/s3-ccpu-callback/result.json).
The diagnostic observer's own exit code is not a passing test result.

Cached full host build and 11 raw/22 published identity checks are separate
receipts. Unchanged product images require no recopy to either publication
location. The containing commit delivers P21. Two literal Ninja generators,
inline build callers and diagnostic producers remain to reconcile in S3;
S4/S5 are not complete.
