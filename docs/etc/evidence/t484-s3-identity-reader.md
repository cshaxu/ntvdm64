# T484 S3 native package identity reader

The Full runner uses isolated_package_cleanup.ps1. Its resource identity reader
previously compiled embedded C# on demand. The existing native package identity
writer now also provides `--read IMAGE`, using resource-only loading and returning
the same uppercase 16-byte representation. Failed image/resource reads still
return null through the test helper; a missing native tool is a prerequisite error.
No product lifecycle, guest or package bytes are changed.

The existing test/observation/verify-cleanup-identity.ps1 now covers unstamped and
missing images, exact identity byte order, unchanged file hashes and the existing
stale-process/unowned-path rejection. No second test runner is introduced.

Evidence: build/t484-s3-identity-reader. Native build and initial focused run
PID 19364 passed; first attempt 13032 caught a removed here-string terminator
left in the helper, corrected before the passing run. Final checked-in test and
governance run: PID 7640. Cached full/hash run: PID 9884. Final results are kept
in closure-result.json and cases-result.json respectively.

B0104 and B0015 remain open pending real fresh-stage dual-package validation;
this reader change alone is not proof of package isolation or Full completion.

Both final runs passed. All11 raw inputs and22 published hashes remained unchanged;
no new product publication was required. Ledger remains286/355 complete,69 open.
