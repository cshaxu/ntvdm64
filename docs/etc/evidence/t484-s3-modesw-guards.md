# T484 S3 P209: native modesw exclusion checks

The previous tools/audit/verify-modesw-guards.py generated C/CMD and invoked
MSVC itself. Its two assertions remain applicable: the selected modesw.c
excludes the original kernel-i386 path at original diagnostic lines 50 and98.
This is a source-boundary test, not CPU30 compilation or guest execution.

The existing worker object-contract module now projects exactly the same two
bounded guard regions from current src/mvdm-host/dpmi32/modesw.c. Configuration
requires exactly two matches and tracks the original source dependency.
Native targets preprocess each region with and without i386; the independent
runner test/mvdm-host/verify-modesw-guards.ps1 asserts both positive exits and
both expected C1189 rejections. Python generation/compiler invocation is removed.
No product, mirror, original guest or addon changes.

Run: build/t484-s3-modesw-guards/run.ps1, owned PID33500, started
2026-11-24T13:58:51.6994721Z. All four cases are recorded in session/results.json;
negative targets return nonzero by design, with original lines50/98 checked.
The same run requires cached full host build and11 raw/22 dual-publication
hash checks. Unchanged package bytes are verified, not republished unnecessarily.
Final result.json and closure-result.json are the authoritative exit records.
This delivery does not claim a new Full runtime or guest acceptance.

Two supplemental direct-command candidates remain: CCPU event consumption and
old Run16 entry. Frozen316/355 and23 supplemental staging chains are unchanged.
S3 remains active; S4/S5 and the recorded simultaneous-output failure remain open.
