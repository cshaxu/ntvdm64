# T484 S3 P54 file cache producer

## Change

B0059 previously called objcopy and i686-w64-mingw32-gcc directly from its
PowerShell runner, using the retired wow32/obj/wkfileio.obj default path.
The CMD `wow-file-cache` target now selects `original_wkfileio` from the actual
WOW product graph. Native CMake owns its four import substitutions and the
existing GNU fixture link profile. The original object remains untouched;
only the test copy redirects CreateFileMappingA, MapViewOfFile, CloseHandle
and UnmapViewOfFile. No duplicate mirror source or product algorithm is added.

The assertion runner delegates construction and records hidden owned process
IDs, deadlines, exit codes and logs. It runs in a build-owned output directory
so its temporary test files cannot appear in the repo root. The optional Object
argument must name the current selected production object. Missing optional
fixture tools fail the requested target, not an unrelated product configure.

## Verification

The actual fixture passed577 checks, failures0. Coverage retains the original
real file/mapping cases, cache promotion and flush, mapping-failure cleanup,
numeric handle reuse, repeated release and eviction checks. The native GNU
link still reports `Warning: corrupt .drectve at end of def file`; this batch
preserves that fixture toolchain and does not claim a warning-free link.
Product compilation remains MSVC. Original guest bytes are unchanged.

Runs belong to build/t484-s3-file-cache; retained results, production-object
identity, cached build and dual-publication verification belong to
artifacts/build-migration/t484/s3-file-cache. No guest runtime pass is claimed.
The final no-argument runner passed the same577 checks. The no-op native
target preserves both converted-object and executable hashes; the CMD entry
rejects x64 with exit2. Cached full host build passed.
All11 raw host inputs and22 published hashes at assets/release and
O:/winnt/system32 remain unchanged; no product recopy was needed. Final script
parsing and documentation governance passed. Existing product runtime evidence
is reused under unchanged identity, not represented as a new guest run.

## Disposition

B0059 has no remaining interpreted producer. PowerShell retains only test
orchestration/assertions; the compiler and object conversion are owned by the
CMD/native graph. Other producers, staging and final clean interpreter-free
acceptance remain open under T484.
