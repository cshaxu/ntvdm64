# T427 S3 application-search isolation

## Contract and implemented boundaries

Reference: S2 main 7d1bd3452 and its verified eight-file package. S3 removes
run16's implicit package-directory preference. The production resolver in
src/run16-exe/application_search.c searches CWD, then each caller PATH entry,
with COM/EXE/BAT/PIF order within each directory. Explicit suffixes are exact;
absolute, relative and drive-qualified paths never search another directory.
Empty/unset PATH means CWD only. Optional quoted and empty PATH entries retain
their explicit placement. Candidate paths are canonicalized without invoking
SearchPath's implicit executable-directory search. Capacity errors clear the
output and fail; existing image classification and shell fallback are retained.

NTVDM's project command_process_compat adapter now names its internally
generated system32/COMMAND.COM interpreter explicitly using common's own-image
root. User-requested COMMAND/EDIT/MEM have no filename exceptions. The original
guest parser/EXEC, CLI arguments (including compact command/c), lifecycle,
RPC authentication and wire versions are unchanged. No new RPC root validation.
APP0.0.427, control38, I/O25 remain the S2 values.

## Reproduction and verification

All run paths below are relative to build/M0-T427/S3 unless identified otherwise.
The validated S2/r001 x86 dependency cache is intentionally reused, not a new
unrelated full rebuild. New production search and dependent run16/NTVDM links
are rebuilt; the matching WOW32 provider is relinked against the new ntvdm.lib.
The formal eight-file snapshot r001/runtime is frozen before runtime gates.
MSVC14.43, SDK22621, Win32 x86 /MT, CCPU40. Existing imported warnings remain;
this record does not claim an all-source warning-free build.

| Entrypoint and exact inputs | Assertions and result |
| --- | --- |
| S2/r001/application-search-test.exe S3/r001/fixture | PASS production function: directory-first order, suffixes, explicit/drive paths, empty PATH, quoted entries, spaces/Unicode, invalid arguments and insufficient capacity. |
| S2/r001/run16-launch-options-test.exe | PASS all 19 retained CLI cases; no mandatory `--` separator introduced. |
| tests/observation/verify-application-search.ps1 -PackageRoot S3/r001/runtime -Probe S1/r001/search-identity.exe -RunRoot S3/r002 | PASS 17 actual process cases, exact GetModuleFileNameW path plus exit37. Missing explicit/empty PATH cases execute no probe and return nonzero (87); no claim of a new exact missing-file error code. |
| tests/observation/verify-search-internal-handoff.ps1 -PackageRoot S3/r001/runtime -Probe S1/r001/search-identity.exe -Observer build/M0-T425/S9/r033/console-startup-observer.exe -RunRoot S3/r004 | PASS actual DOS COMMAND outside package CWD/PATH: native batch stdout and stderr both present; actual DOS-to-native selected Z:/tests/SEARCH.EXE. Both root requests complete0. GUI probe startup is not misrepresented as DOS waiting for its eventual exit37. |
| tools/audit/Invoke-ProductVerification.ps1 -RuntimeRoot S3/r001/runtime -BuildCache S2/r001 -Observer build/M0-T425/S9/r033/console-startup-observer.exe -WindowObserver build/M0-T425/S9/r033/worker-window-snapshot.exe -LogRoot S3/r003 -Suite Product -GuestFixture build/M0-T425/S9/r008/G7.COM -WowBaselineRoots S2/r003 | PASS retained Console17, Window17 and three WOW-frontier comparisons; frozen eight hashes in r003/runtime-manifest.json. |

The actual-image cases include CWD before package/PATH, PATH directory before
extension, reversed PATH, explicit and drive-relative names, COM before EXE,
empty/unset PATH, quoted/empty entries, spaces/Unicode and ordinary explicit
package PATH/CWD membership. The inherited false NtvdmSystemRoot is not used.
The actual-image probe writes a unique CREATE_NEW UTF16 report; an old output
cannot satisfy the selected-file assertion. Unit zero-byte files prove search
only, never image classification; actual-process tests supply that boundary.

Product elapsed204220ms: preparation1302, WOW67108, Console62759, Window68817
(cleanup recorded separately in timings.json). WINMINE retains its accepted
visible frontier; SOL/WRITE retain their existing failure/depth frontiers,
not gameplay passes. Physical RDP and broader WOW limits remain unchanged.
The known long guest-path limit uses approved Z: only, removed in finally.

## Review, publication and retained scope

Review: source search is independent of own-image resource lookup; no default
SearchPath fallback, hidden package priority, guest/mirror change or parser
rewrite is introduced. PATH storage is local/heap-owned and freed on every
return after allocation; output ownership and checked NUL capacity are explicit.
S3's source diff replaces the old resolver, rather than leaving an unused
parallel policy. Internal command arguments remain on their original path.

r005/publish.ps1 checks candidate hashes against the completed Product gate,
retains coherent S2 recovery, and replaces only the eight product files in
O:/winnt. Published source/hash equality passes. Existing guest/configuration
and user data remain untouched. r006/published-smoke.ps1 checks the four
COMMAND/MEM/EDIT/native VER cases in each Console/Window route and final eight
hashes. Documentation governance, links, diff review and commit/push are the
remaining sequential closure checks; CURRENT records their actual completion.

T427 remains open for S4's whole-objective/root-caller and integration audit.
Exact Win16 directory API execution is not inferred from these search tests.
Bare package applications from elsewhere require caller PATH placement or an
explicit path; that is the intended corrected discovery contract, not a regression.
