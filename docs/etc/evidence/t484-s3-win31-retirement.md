# T484 S3 P217: retire superseded Win31 setup scripts

The four PowerShell scripts in tools/build/win31-setup had only three old
script tests as callers. Current tools/win31-setup/APPLY.CMD uses the released
patch31.exe and SZDD.EXE. This batch removes those seven old files, retaining
historical evidence. No production source, original guest or installation changed.

The existing test/integration/win31_setup_cmd_roundtrip_test.ps1 now additionally
checks both exact adapted SHA256 identities, unchanged file extents, only the
approved KRNL386 header/40-byte range and WIN386 10-byte range, and rejection
of corrupted inputs without changing either argument. Existing repeat apply,
backup restoration and unknown-file protection checks remain. The old tests'
generator-authored JSON assertions are retired; they did not independently
prove runtime behavior. Native patch31 deliberately accepts already adapted
input, so the negative case uses corrupted input instead.

Tests use read-only O:/assets/mswindos/ewin31 and build-owned copies, under the
existing T436/T473 source-policy exceptions. No Windows installation or original
media is modified. Test source remains an assertion runner, not a build generator.

## Verification

Owned background runs and stdout/stderr/results are in
build/t484-s3-win31-retirement/final. The preceding unchanged CMD roundtrip
also passed (PID31656). The final run records the modified test, all14 addon
build/hash checks, cached full host build and11 raw/22 published image hashes.
Final delivery requires its terminal success and governance gates; this record
does not claim a new Full product regression or resolve the earlier P193/P215
runtime limitations. S4 staging and S5 interpreter-free clean acceptance remain.

Final run PID19760 passed, 2026-11-24 14:58:56Z to15:00:05Z.
All14 addon raw/release hashes and11 host raw/22 publication hashes matched;
no changed runtime bytes required copying. Frozen census now323 disposed/32
pending (26 pending producers/staging and6 reconciliation);24 supplemental
preparation chains remain. This batch removes351 net test/tool lines.
