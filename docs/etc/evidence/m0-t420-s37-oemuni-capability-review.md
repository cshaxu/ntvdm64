# T420 S37 OEMUNI capability review

## Scope and baseline

S37 follows S36 closure 5ad0ac874. The original manifest selects file.c and
process.c; toemuni.c is the original test, not a production unit. S17 recovered
those sources and DIV-269 early-failure sentinels. This review must establish
actual non-ASCII guest paths, buffer/failure contracts and reached consumers,
not infer capability from source inclusion. Current product is x86 CCPU40;
original guest media remain immutable.

## Initial owner inventory

The two translation units define 29 Oem entry points. A textual call search
finds DEM/COMMAND consumers for file creation/deletion/attributes/rename,
FCB enumeration, full-path/current-directory operations, directory creation
and removal, drive/disk/volume/computer information, search/short paths and
environment conversion. ExpandEnvironmentStringsOem is reached textually
from original nt_pif.c. Additional WOW consumers are present in wdos.c,
wkfileio.c and wkman.c; their presence is not evidence of a working Win16
workload. MoveFileExOem and GetTempFileNameOem have only WOW textual consumers
in this initial search.

GetSystemDirectoryOem, GetWindowsDirectoryOem, GetTempPathOem and
CreateProcessOem have no direct call-shaped matches outside their definitions
and original test in the scanned source roots. This is a preliminary inventory,
not proof of unreachability: macro aliases, formal selection and remaining
roots must be reconciled before final disposition.

## Non-ASCII and buffer baseline

The expanded existing oemuni_failure_fixture builds original file.c and
process.c against the same formal RTL/support inputs as the product. Build:
`node tools/build/Generate-T420S17OemUniNinja.mjs .
build/M0-T420/S37/oem-contract-r1
build/M0-T420/S36/original-callback-product-r1`, followed by MSVC x86 Ninja
target oemuni-failure-fixture.exe. The reused formal inputs are those of the
S36 deployed baseline; no product source or guest input is changed.

The executable reports OEM=437 and ANSI=1252, then exits zero with:

- S37_OEM_FULLPATH_NONASCII_PART_SHORT_BUFFER_OK: non-ASCII e-acute OEM
  filename, full path checked against Windows Unicode conversion, correct
  file-part pointer/text, and one-byte buffer returns required size without
  overwriting its sentinel.
- S37_OEM_ENV_NONASCII_DELETE_MISSING_OK: Unicode-set environment value is
  retrieved through OEM conversion, removed through the original OEM entry,
  and returns ERROR_ENVVAR_NOT_FOUND afterward.

The previous null-short-path and valid file-part checks also pass. All
environment changes are process-local. This proves the current SBCS host
contract only; no DBCS, real DOS file operation, process creation or full-package
closure is claimed. Those remaining selected-family checks continue in S37.

## Real DOS non-ASCII file path

NASM builds independent `oem_file_contract.asm` into
`build/M0-T420/S37/oem-guest-r1/O37.COM`. The reusable
Verify-T420S37OemGuest.ps1 runs it with the existing non-debug Console observer
against the deployed formal x86 package, directly and under two COMMAND /c
levels. This fixed-byte probe requires host OEM 437; it does not change host
or guest code pages. Byte 9C is pound sign and has no case-fold ambiguity.

The real guest calls INT 21 directory creation, create-new, write/close,
rename, failed open of the old name (error 2), set/query/reset read-only
attributes, and reopen/read/close with exact payload comparison. Guest paths
include a non-ASCII directory and filename. The observer requires natural
launcher exit zero and S37_OEM_GUEST_CREATE_RENAME_ATTR_READ_OK. The host then
independently checks the actual Unicode `D£/B£.DAT` name and exact ASCII
payload bytes, preventing a same-wrong-encoding guest roundtrip from passing.

Both routes pass (`s37-oem-guest-r1`, runtime logs). The test creates only
private O37G descendants under runtime tests, refuses an existing directory,
and deletes only the verified file and empty directories. Failure preserves
files for inspection; no recursive cleanup or original guest mutation occurs.
Process isolation only terminates package children of its recorded launcher.
This verifies the reached DEM/OEM file-operation family, not every OEMUNI
entry or a WOW workload. Remaining consumer and buffer-family audit continues.

## Original environment-expansion output defect

Actual caller nt_pif.c uses ExpandEnvironmentStringsOem for path expansion.
The original function converts OEM input to ANSI, calls the ANSI expansion,
allocates a converted OEM result, then frees it without copying it back. The
fixture sets Unicode e-acute in its process-local environment; before repair
the output is E9 (ANSI 1252), expected 82 (OEM 437), with return length 2.
The identical omission exists in pinned OpenNT process.c.

DIV-273 retains the original conversion/Win32/cleanup sequence and publishes
the converted OEM bytes only within the supplied capacity, returning their
terminator-inclusive count. It also clears Value.Buffer after consuming its
borrowed lpDst alias, so cleanup cannot free the caller buffer if that
conversion fails. No new adapter, overlay or conversion engine is added;
the mirror delta is nine added lines including comments, four executable
statements. Allocation-failure injection and other conversion families remain
separate pending checks, not inferred from the successful path.

The original-source x86 fixture passes the repaired non-ASCII expansion,
one-byte output with untouched next-byte canary, and null-source error.
The first short-buffer assertion incorrectly assumed required length 2;
the actual original ANSI provider returns 3 on this host, reproduced by a
direct ExpandEnvironmentStringsA control. The corrected test preserves that
insufficient-buffer contract rather than changing the product to fit a test.

Formal x86 incremental rebuild deliberately reuses the S36 graph and produces
worker SHA-256 `22ff7c7978f5b5d5aa19e7a49771a2f88a93e929e4023717c956bb2731466fe3`
and VDMREDIR.dll `09686f7f5def440326185f5d0257298220a980e674073bce11c8eba03e2ea197`.
These non-diagnostic artifacts are deployed to `O:\winnt`. Both real non-ASCII
DOS routes pass under s37-oem-expand-guest-r1; all 17 product regressions pass
under s37-oem-expand-product-r1. Product and guest observations remain in the
runtime logs directory. No original guest file changes occurred.

## Conversion-failure proof and compiled inventory correction

The test-only oemuni_expand_failure_fixture includes actual process.c and
intercepts its ANSI-to-Unicode and Unicode-to-OEM calls plus a checked free.
Each conversion can return STATUS_NO_MEMORY. Before repair, stage 1 returns
length 4 with error 8: success is reported despite conversion failure.
Clearing ReturnValue on entering the conversion block fixes both failure
points. Both now return zero/error 8, preserve the output canary and never
attempt to free the borrowed output pointer. The normal non-ASCII,
short-buffer, null-input and environment checks also pass.

The fixture generator specified Ninja msvc dependencies but omitted
/showIncludes. Its first incremental run therefore reused the wrapper's old
object after included process.c changed. Adding /showIncludes supplies the
dependency record; rebuilding demonstrates both passing failure stages.
This harness correction is not a product workaround. Artifacts/logs are in
`build/M0-T420/S37/expand-failure-r1`.

The 29 textual definitions are not 29 selected functions: CreateProcessOem
is inside original `#if 0 // unused` (process.c lines 371--560). It has no
source call, and the formal map contains the other 28 OEMUNI definitions but
not CreateProcessOem. No process-launch implementation is missing from this
selected package on that account. Remaining consumer, buffer and DBCS
dispositions are still open.

Formal x86 worker/DLL relink and deployment produce hashes
`8415136752054e712ded3a1950051bb2de77900a10f8d00a403d7cd16bc87000`
and `b2bd030c71fe7b2346cee09031dc5646587312234137b8b2debca09742e32142`.
Direct/nested real DOS non-ASCII tests pass in s37-expand-failure-guest-r1;
all 17 product routes pass in s37-expand-failure-product-r1. This adds one
executable mirror statement to DIV-273; no new adapter or guest modification.

## Remaining API-family fixture

The x86 `oemuni-family-fixture.exe`, generated through the S17 generator in
`build/M0-T420/S37/family-r1`, links the actual file.c/process.c objects and
the intentionally reused S36 formal RTL inputs. Running with that build root
as its working directory exits zero with both family markers:

- S37_OEM_ENUM_SEARCH_RENAME_DELETE_DIRECTORY_OK: CP437 non-ASCII filenames,
  both rename APIs, attributes, FindFirst/FindNext including end-of-enumeration,
  SearchPath/file-part, short-name lookup, deletion and missing-file failure.
- S37_OEM_TEMP_SYSTEM_VOLUME_COMPUTER_NEGATIVE_FONT_OK: process-local TEMP/TMP,
  temporary file creation/deletion, system/Windows directories, drive type,
  disk capacity, volume/computer information, debug-string conversion and
  rejection of an absent font.

Directory creation/current-directory queries, a one-byte buffer canary,
restoration of the original working directory and removal of the private
O37CASE directory also pass. The test refuses an existing O37CASE directory
and does not recursively clean anything. Review tightened the read-only
attribute assertion to reject INVALID_FILE_ATTRIBUTES; otherwise a failed
query would incorrectly satisfy a bit-only test.

This is host-side original-owner API evidence on OEM 437, not additional
guest end-to-end or DBCS proof. The absent-font check does not demonstrate
successful loaded-font removal. No production source, deployed artifact or
guest media changed in this delivery; the preceding formal build and 17-route
product regression therefore remain its unchanged product baseline. Caller
disposition, remaining buffer cases and DBCS analysis still prevent S37 closure.

## DBCS full-path defect reproduced

The test-only oemuni_dbcs_fixture compiles the actual file.c body. It supplies
a fixed Unicode full path containing U+8868 and uses public
WideCharToMultiByte(932) at the conversion boundary. No host/system code-page
change, product hook or filesystem mutation occurs. The expected bytes come
independently from conversion of the complete Unicode path; this is a mocked
code-page boundary, not a CP932 DOS guest acceptance run.

Build/run root: `build/M0-T420/S37/dbcs-r1`, generated through the existing
S17 fixture generator, MSVC x86 target oemuni-dbcs-fixture.exe. Observed:
actual_length=10 versus expected_length=11; file-part offset 5 versus 6;
the expected terminator byte remains sentinel 90. The diagnostic marker is
S37_DBCS_ORIGINAL_LENGTH_COPY_FILEPART_DEFECT_REPRODUCED. Zero fixture exit
means the defect was reproduced, explicitly NOT that the capability passes.

Pinned `O:/repos.external/OpenNT/base/mvdm/oemuni/file.c` contains the same
UnicodeLength-based copy/count and WCHAR-based file-part arithmetic. This is
an original-source SBCS assumption, not a local adapter or CCPU defect.
Selected text callers include dos/dem/demmisc.c and dos/command/cmdpif.c.
The current OEM437 success therefore cannot prove this interface correct on
a DBCS host. S37 remains open: review the related length/capacity families,
then make a minimal original-owner correction and replace the diagnostic
expectation with positive/short-buffer/optional-output/failure verification.
This delivery changes tests and evidence only; deployed products are unchanged.

## Source-shaped DBCS correction

Further source research found the same defect already corrected in original
OpenNT `base/win32/client/curdir.c::GetFullPathNameA`, under its DBCS sections
(lines 124--164 and 183--188): compute encoded path and prefix byte counts,
not WCHAR counts. This supplies the algorithmic authority; the OEMUNI file
in both pinned OpenNT and OpenNT-4.5 still retains the old calculation.

Recovery ladder: retain the already composed OEMUNI translation unit; use
the existing RtlUnicodeStringToOemSize interface for the original correction's
two size operations. The ANSI BaseClient body is not directly substituted:
its Basep8Bit policy can select ANSI whereas this API promises OEM. Full
BaseClient import is neither required nor admitted. No external intrusion,
new conversion algorithm, adapter body or overlay is necessary.

DIV-274 adds bounded OEM byte sizing before the existing capacity/copy logic
and sizes the Unicode prefix for lpFilePart. An unsuccessful or oversized
Unicode path is not consumed as initialized text. Mirror delta against the
preceding delivery is 12 added / 2 removed lines, including comments; the
existing control/conversion/allocation owner remains in file.c. No
opennt-host source or guest medium changes.

The CP932 test now asserts success rather than the earlier diagnostic failure:
length 11, prefix 6, terminator zero; capacity 11 returns required 12 without
writing output, capacity 12 copies exactly and preserves the next-byte canary.
Null optional output, null returned file-part, zero provider result and
oversized provider result also pass. All four x86 OEMUNI fixtures pass,
including existing SBCS file/environment and conversion-failure cases.
The CP932 result is still explicitly a mocked conversion-boundary proof,
not a real DBCS-host guest run.

Formal x86 product relink succeeds in the intentionally reused S36 graph.
Deployed worker SHA-256 is
`f6188e688d02c55d1e2f1d6e663c5e0aa821a53e465a05206c2c03ae2457e057`;
VDMREDIR.dll is
`a9cb67092e5d5f9fb7111c2b9ee96d682537d0de5c6f6abcbf4cac92c69623b9`.
Real OEM437 guest direct/nested file routes pass (s37-dbcs-guest-r1).
All 17 Console-text-gated product routes pass under s37-dbcs-product-r1,
including original expected nonzero COMMAND exits, native streams/EOF,
direct/nested MEM and EDIT. S37's remaining whole-family disposition is not
implied complete by this correction.

## Adjacent capacity-family sweep

The same x86 CP932 fixture now includes both actual OEMUNI translation units,
with fixed Unicode providers for current/search/system/Windows/temp paths.
All use the same 10-WCHAR / 11-OEM-byte sample and a caller-declared capacity
of 11 inside a larger sentinel-filled test array. These are test-only API
boundaries, not a change to the host code page or product provider.

Five additional defects reproduce: GetCurrentDirectoryOem, SearchPathOem,
GetSystemDirectoryOem, GetWindowsDirectoryOem and GetTempPathOem all write a
NUL at offset 11, outside declared capacity. Their original n+1 conversion
capacity causes this. SearchPathOem additionally returns 10 instead of 11
and publishes file-part offset 5 instead of 6. The current-directory mock
retains the original TEB buffer access; search/directory mocks retain the
Unicode API's character-count contract.

The markers ending DEFECTS_REPRODUCED_NOT_ACCEPTED deliberately distinguish
these baseline witnesses from the already repaired full-path PASS. The test
process exits zero only after proving every expected defect; it does not
certify product correctness. Build log: family-sweep-build.log under the
existing dbcs-r1 root. No additional product code or deployment changed.

The family must be corrected together using original owner byte-size rules,
actual caller capacity and required-size reporting. Remaining volume-name
output capacities also contain n+1 expressions and still require their own
boundary proof; no claim is made that the five reproductions exhaust the
package. S37 remains active and these findings are not deferred out of scope.

## Five-interface capacity correction

DIV-275 repairs the five reproduced conversion-capacity branches together.
The existing OEM RTL size operation determines required bytes including NUL;
the existing converter receives actual caller capacity, never n+1. Search
publishes converted length and prefix bytes and no longer interprets a
Unicode required-size equality as initialized text. Cleanup remains in the
original owner, including the short-output branch's Unicode-buffer release.

Original-source rationale: BaseClient curdir.c GetCurrentDirectoryA's DBCS
section already measures encoded bytes before comparing caller capacity.
The same original conversion/size contract applies to OEMUNI's directory
and search thunks. Both original translation units remain composed; existing
RTL APIs express the correction without full BaseClient import, new adapter,
overlay, external-code intrusion or a replacement conversion engine.

All five CP932 boundary cases now return required size 12 at capacity 11,
leave the output and next-byte sentinel untouched, and return length 11 with
exact expected bytes at capacity 12. Search also returns prefix offset 6.
All four x86 OEMUNI fixtures pass, including SBCS and failure-injection tests.
The two mirror bodies change by 40 added / 10 removed lines, including
registration comments; no opennt-host code or guest medium changes.

Formal x86 rebuild succeeds; deployed worker SHA-256 is
`520a82de28c1d3b2cf3f7cec9cf7c4c2138ceaf37f29f24cced327a3c61aa879`
and DLL is `5e20c087716f34404bb17c43d07e46b96c398f73116c4b0786c3787a1eaad3fa`.
Real OEM guest direct/nested tests pass under s37-capacity-guest-r1.
All 17 established Console-text-gated product routes also pass under
s37-capacity-product-r1, including EDIT and direct/nested COMMAND/MEM.

This is not full buffer-family closure: branches where the intermediate
Unicode query itself reports insufficient space still need encoded required-
size review, as do volume-name/filesystem-name capacities and wide-to-USHORT
narrowing. Existing tests do not prove these branches. Keep them in S37;
CP932 remains test-only boundary evidence rather than real-host acceptance.

## Volume dual-output capacity and cleanup

The same fixture now binds GetVolumeInformationW to a non-mutating test
provider returning U+8868 as each name. Its Unicode result fits two WCHARs,
but its CP932 representation needs three bytes including NUL. Before repair,
the original OEMUNI function with declared capacity 2 returned success and
changed the offset-2 sentinel to zero (fixture exit 18). Pinned OpenNT file.c
lines 898/900 contain the same erroneous n+1 capacities.

DIV-275 now also removes those two +1 expressions. No new algorithm or
adapter is needed: the already selected original RTL conversion reports
STATUS_BUFFER_OVERFLOW, original OEMUNI maps it to ERROR_MORE_DATA (234),
and its original finally block releases both Unicode buffers. This is direct
original-owner reuse with a minimal capacity correction, not a facade or
external import. Delta: file.c 4 added / 2 removed lines including comments.

Test separately makes each output too small: both return FALSE/error 234,
retain the boundary sentinel and finish with zero counted temporary heap
allocations. Exact capacity 3 returns both expected CP932 names; both omitted
outputs succeed; injected host ERROR_NOT_READY propagates with zero remaining
allocations. No host volume label, code page or guest media is modified.
All four x86 OEMUNI fixtures and formal x86 relink pass. Logs remain in the
existing dbcs-r1 build root. The CP932 tests remain mock-boundary evidence.

Deployed worker SHA-256:
`0dc5c24f089d5e05b1cefbe40bfc774357bf3d6c6780dfa0668e300fb7e54b39`;
VDMREDIR.dll:
`5b72e5d6e939185b663bdfe69bcbf0f1e796137d2ce184e143062bf1fe5ea567`.
Real OEM guest direct/nested file routes pass under s37-volume-guest-r1.
All 17 Console-text-gated product regression routes pass under
s37-volume-product-r1, including EDIT and original expected exit codes.
The outstanding S37 work remains intermediate-query sizing, narrowing and
complete selected-caller disposition; this fix is not whole-package closure.

## Selected-entry evidence reconciliation

At revision 1edae5841 the formal ntvdm.exe.map contains all 28 definitions
from original-mvdm-host-oemuni file.obj/process.obj. The original disabled
CreateProcessOem is not in that map. Call-shaped searches cover DOS, SoftPC,
WOW32, executable roots and opennt-host; a source-wide macro-alias search
finds no aliases bringing the three otherwise uncalled directory helpers
into selected call sites. Text matches establish source ownership, not
runtime hits or successful WOW execution.

Evidence labels below: G = actual S37 DOS probe plus independent host name/
content oracle; H = original-owner host fixture; M = test-only CP932 boundary
or failure injection. H/M do not substitute for G. Regressions from other
packets remain supporting evidence, not per-interface hits.

| Selected OEMUNI function | Original source consumer | Current direct evidence / remaining boundary |
| --- | --- | --- |
| CreateFileOem | DEM demfile/demfcb/demmisc; WOW wkfileio | G create/open/read/write/close and failed old-name open; H |
| SetFileAttributesOem | DEM demfile/demfcb; WOW wkfileio/wkman | G set/reset read-only; H |
| GetFileAttributesOem | DEM demfile/demdir; COMMAND cmdpif; WOW | G attribute query; H |
| DeleteFileOem | DEM demfile/demfcb; WOW wkman | G delete plus reopen error 2; H success/missing |
| MoveFileOem | DEM demfile/demfcb; WOW wkman | G rename; H |
| MoveFileExOem | WOW wkman delayed-delete workaround | H; no WOW workload acceptance |
| FindFirstFileOem | DEM demfcb FCB delete/rename | G FCB wildcard deletion of two files and repeated no-match; H |
| FindNextFileOem | DEM demfcb FCB delete/rename | G two-file loop and exhaustion; H end-of-enumeration |
| GetFullPathNameOem | DEM demmisc; COMMAND cmdpif; WOW wdos/wkman | H/M byte count, prefix, short/exact, zero/oversize result |
| GetCurrentDirectoryOem | WOW wdos | H/M; no WOW workload acceptance |
| SetCurrentDirectoryOem | DEM demdir/demgset; COMMAND cmdpif; WOW wdos | G non-ASCII switch and exact restoration; H |
| CreateDirectoryOem | DEM demdir | G non-ASCII directory create; H |
| RemoveDirectoryOem | DEM demdir | G private subdirectory removal; H |
| GetDriveTypeOem | DEM demioctl/demgset; COMMAND cmdmisc | G INT21 4408 removable/fixed result; H native comparison |
| GetDiskFreeSpaceOem | DEM demgset | G INT21 36 valid sectors/bytes/capacity; H |
| GetVolumeInformationOem | DEM demfile/demgset/demsrch; WOW wkfileio | H/M both capacities, optional outputs, host failure and cleanup |
| OutputDebugStringOem | DEM diagnostics including demmisc | H call; debug-only consumers do not establish ordinary guest behavior |
| GetComputerNameOem | DEM demgset | G INT21 5E00 name matched to host; H |
| RemoveFontResourceOem | DEM demfile create-failure fallback; WOW wkman | H absent-font failure; M original conversion and five original DEM retry scenarios; no real font installation claim |
| GetSystemDirectoryOem | No production textual consumer found | H/M complete-query size matrix and failure cleanup |
| GetWindowsDirectoryOem | No production textual consumer found | H/M complete-query size matrix and failure cleanup |
| SearchPathOem | COMMAND cmdpif | H/M byte count/prefix, complete-query size matrix and failure cleanup |
| GetTempPathOem | No production textual consumer found | H/M complete-query size matrix and failure cleanup |
| GetTempFileNameOem | WOW wkman | H create/delete; no WOW workload acceptance |
| GetEnvironmentVariableOem | COMMAND cmdenv/cmdmisc; WOW wdos | G explicit original COMMAND 54:04 returns host-matched non-ASCII directory; H/M encoding/capacity/failure; no WOW acceptance |
| SetEnvironmentVariableOem | DEM demdir; COMMAND cmdpif/cmdmisc; WOW wdos | G INT21 chdir publishes =X: and COMMAND 54:04 reads it back; H set/delete; no WOW acceptance |
| ExpandEnvironmentStringsOem | SoftPC host nt_pif | H/M non-ASCII, short buffer and two conversion failures |
| GetShortPathNameOem | COMMAND cmdpif | H valid file path and null-input failure |

The font fallback is not an unreachable-WOW exclusion: demfile.c's reached
create failure path examines TTF/FON/FOT extensions, attempts removal and
retries creation. It must receive a non-invasive original-call mock before
closure; do not load/remove a user's installed font merely to generate a hit.

Provider check: the formal map binds RtlUnicodeStringToOemString to the
existing opennt_support_rtl.obj implementation, and RtlUnicodeStringToOemSize
to NTDLL. The former measures WideCharToMultiByte(CP_OEMCP) output plus NUL
and rejects insufficient MaximumLength before writing. The CP932 mock
exercises this capacity rule but does not prove every native NLS behavior.
No production provider change is made by this reconciliation.

Closure work is therefore concrete, not an unspecified full sweep: complete
intermediate-buffer/narrowing cases; add targeted DOS FCB/directory/info
evidence; prove the font fallback contract with a test-only provider; and
reconcile each remaining host-only/WOW-owned row without claiming a WOW
workload. These remain part of active S37, not silent queue deferrals.

## Real DOS directory, deletion and disk information

The independent oem_file_contract.asm probe now also saves its DOS current
directory (INT21 47), changes into the non-ASCII test directory (3B), creates
and closes a disposable file (5B/3E), deletes it (41), then requires reopening
to fail with error 2. It creates/removes another non-ASCII subdirectory
(39/3A), queries current-drive free space (36: nonzero cluster/sector totals
and free <= total), and queries removable/fixed drive type (4408: 0 or 1).
Finally it restores the exact saved directory and compares the 64-byte DOS
directory result. Existing non-ASCII content/rename/attribute proof remains.

NASM output is `build/M0-T420/S37/guest-family-r1/O37.COM`. Both direct and
two-level COMMAND /c routes pass on the unchanged 1edae5841 formal product;
runtime log prefix s37-directory-disk-guest-r1. The harness now requires
S37_OEM_GUEST_DIRECTORY_DELETE_DISK_OK as well as the original file marker,
natural successful exit, and the independent Unicode-name/content oracle.
It confirms that only the intended final file remains before exact cleanup.
Duplicate code-page and directory-count checks in the harness were removed;
each required check remains once. No guest medium or product source changed.

These real calls cover the original DEM directory/file/disk-information
families; they do not prove FCB enumeration, computer-name service, WOW or
font retry. The coverage table above reflects only these new observations.

## Real FCB loop and computer-name service

Source inspection identifies demfcb.c::demDeleteFCB as the original
FindFirstFileOem/FindNextFileOem owner; ordinary DOS handle-find would not
prove those functions. The independent probe creates two private F-pound
DAT files inside its non-ASCII directory, closes them, then calls INT21 13
with a normal FCB wildcard. It requires AL=0, verifies both names can no
longer be opened (error 2), and requires a repeated wildcard delete to return
AL=FF. The original demDeleteFCB enumerates, deletes each match, advances
FindNext and closes the search handle. The harness's remaining-file oracle
independently confirms both disposable files are gone.

DOS macro.asm UserGet invokes SVC_DEMGETCOMPUTERNAME, implemented by demgset.c.
INT21 5E00 now requires success, nonzero CH and the 16-byte buffer's final
NUL, then emits the 15-character name. The harness compares S37_HOST against
the actual host COMPUTERNAME in addition to requiring the FCB/computer
marker and all earlier markers. This does not invent a network server or
use a mock. Only the current host's ASCII computer-name profile is covered.

NASM builds `build/M0-T420/S37/guest-fcb-r1/O37.COM`. Both direct and two-level
COMMAND /c routes pass, prefix s37-fcb-guest-r1, on unchanged formal product
1edae5841. Original guest media remain unchanged; no product rebuild or new
product implementation was needed. FCB rename's distinct policy and font
retry are not inferred from FCB delete. Remaining OEM buffer/caller review
still prevents S37 closure.

## Complete declared-capacity matrix

The CP932 fixture now tests current/system/Windows/temp/search helpers at
capacities 0, 1, 10, 11, 12 and 65536, with a real 65536-byte backing buffer.
Each row checks both result length and output bytes/sentinel; a matching
return number alone cannot pass. The sample needs 12 OEM bytes including
NUL. Thirty cases produce 13 passes and 17 failures on 1edae5841:

- Current directory passes all small capacities but fails 65536 with error
  234: its USHORT capacity wraps to zero.
- System/Windows/temp/search each fail 0/1/10: the intermediate Unicode
  query returns its character requirement (11), not OEM byte requirement 12.
- At 65536, system/Windows/temp return 11 without writing the expected path;
  search fails with error 234. These are narrowing defects, not successful
  results merely because some return values happen to equal expected length.

The diagnostic reports S37_SIZE_MATRIX_OPEN_DEFECTS=17 NOT_ACCEPTED. Its
zero exit confirms the expected open defects exist, NOT package acceptance.
Earlier repaired full-path, boundary-11/12 and volume checks still pass.
Build/run uses the existing dbcs-r1 target and size-matrix-build.log. No
product source or deployed artifact changes in this delivery.

Original recovery references are now located: BaseClient pathmisc.c contains
DBCS sections in SearchPathA (line 717 onward), GetTempPathA (line 1214
onward), and system/Windows directory routines. SearchPathA explicitly
reallocates and re-queries a complete Unicode result before computing encoded
required bytes. The temp-path historical retry itself retains suspicious
old-capacity use and is not approved for blind copying. The next correction
must preserve the original query/conversion/cleanup owner, validate the full
intermediate result and prevent narrowing; no fixed small-buffer workaround
or fabricated success is accepted. These 17 failures remain active S37 work.

## Complete-query recovery and capacity-matrix repair

DIV-275 now separates intermediate Unicode capacity from caller OEM capacity
in system/Windows/temp/search queries. The original query, allocation,
conversion and cleanup remain in their original functions. A zero-capacity
Unicode query obtains the required full size; the original heap allocation
uses that size and the second query is checked before any USHORT assignment
or conversion. This follows original BaseClient SearchPathA's complete-result
principle without copying its ANSI policy or importing the whole client.
The system-directory originals use an already complete Base global Unicode
string; standalone obtains the same complete input through their public W
query. The historical temp retry's stale capacity is not copied.

The original UNICODE_STRING byte carrier limits complete input to 32767
WCHARs including NUL. Larger requirements fail with the existing overflow
status, rather than wrapping. A second query that fails retains its error;
growth beyond the queried capacity fails with overflow instead of consuming
an uninitialized path. Existing allocations are always released. Output
MaximumLength uses min(caller capacity,65535), preserving the original
descriptor representation without turning a valid large output into zero.
No fixed path-sized workspace, retry loop, new adapter/provider, external
intrusion or new mirror file is introduced.

The previous 17 failures are now repaired: all 30 CP932 capacity cases pass,
including 0/1/10/11 required-size queries and 12/65536 successful output.
Sixteen additional cases cover four queried APIs with growth, second-query
ERROR_NOT_READY, unrepresentable Unicode requirement and allocation failure.
Each asserts zero result, exact error, untouched caller sentinel and zero
remaining temporary allocations. All four OEMUNI fixture executables pass;
these are boundary mocks, not a real CP932 system run.

Formal x86 product rebuild succeeds. Mirror change in this delivery is
file.c 1 added / 1 removed line and process.c 50 added / 26 removed lines;
no overlay or adapter implementation is added. Latest deployed worker hash:
`dd4d4c42b7e1fe3dca9391b8eb5ab2657cc2db767c78314de75de6418a4940fa`;
DLL hash `77d15a95a790fe3f1c7eb71ae0de9bc06f475c46fae8ce35268958f4be467708`.
Expanded real DOS file/directory/FCB/computer-name direct and nested routes
pass under s37-query-guest-r1. Remaining font/caller and other API boundary
review still belongs to S37; this is not full package closure.
All 17 Console-text-gated product regressions pass under
s37-query-product-r1, including EDIT and expected original COMMAND exits.

## Non-invasive original font-interface proof

oemuni_font_fixture includes the actual file.c translation unit and replaces
only RemoveFontResourceW with a recording provider and the OEM conversion
entry with a forwarding wrapper that can inject failures. Normal conversion
uses the formal RTL binding on actual OEM437. Input byte 82 in a FON name
must reach the GDI-shaped provider as U+00E9, not an ANSI-misinterpreted name.
No installed font is loaded or removed.

The x86 fixture verifies TRUE propagation, FALSE/ERROR_FILE_NOT_FOUND
propagation, STATUS_BUFFER_OVERFLOW -> ERROR_FILENAME_EXCED_RANGE, and
STATUS_NO_MEMORY -> ERROR_NOT_ENOUGH_MEMORY. Neither conversion failure may
invoke GDI. The thread-local Unicode buffer address/capacity must remain
unchanged and a subsequent success must still work. The original function
requests non-allocating conversion into its TEB buffer; it owns no temporary
font handle or heap allocation to release.

Build root `build/M0-T420/S37/font-r1`, target oemuni-font-fixture.exe via the
existing generator and formal RTL inputs. Exit zero and marker
S37_FONT_ORIGINAL_OEM_CONVERSION_RESULT_FAILURE_TEB_REUSE_OK pass. This is
explicitly mock-boundary evidence, not a real GDI font removal or a DEM
create-failure/retry chain. That higher-level chain remains pending; its
source reachability was established above. No production source, guest
media or deployed binary changes in this delivery.

## Original DEM font retry chain, test-only boundaries

The fixture generator extracts the current demfile.c demCreateCommon body
verbatim into its build root, records the source SHA-256, and registers
Ninja regeneration on the generator/source file. It is a source-shaped
function-slice fixture, not the full DEM translation unit or a guest run.
Unrelated DEM handlers are not reimplemented to make the fixture link.
The fixture composes that exact body with actual OEMUNI file.c and formal
RTL, supplying synthetic guest registers/path-copy input and controlled
CreateFileW, RemoveFontResourceW, GetFileSize and PostMessage endpoints.
The CD-ROM predicate is explicitly false for this font scenario; its separate
retry policy is not tested here. No installed font or actual broadcast occurs.

Five x86 cases pass:

- Original TTF removal succeeds; broadcast occurs once; second create succeeds.
- TTF removal fails, original same-basename FOT removal succeeds; broadcast
  once; second create succeeds. The Unicode FOT path is checked exactly.
- Removal succeeds but second create still fails: no repeated removal loop;
  original error endpoint receives ERROR_SHARING_VIOLATION.
- Both TTF/FOT removal attempts fail: no broadcast or create retry; original
  error endpoint is used.
- Non-font DAT create fails: no font call or broadcast; original error path.

The test validates non-ASCII OEM-to-Unicode filename delivery, original
read/write access and sharing flags, inheritability, CREATE_ALWAYS,
successful split-handle register publication and failure count/carry. No
successful handle is prematurely closed. It does not prove real GDI state,
DOS guest dispatch, CD-ROM fallback or GetFileSize failure handling.

Build root `build/M0-T420/S37/font-retry-r1`; target
oemuni-font-retry-fixture.exe exits zero with five S37_DEM_FONT_RETRY PASS
lines. Both initial/final builds pass. This closes the pending source-shaped
font retry mock requirement without adding product code; real font-medium
integration is not falsely reported as done. Product artifacts remain those
of c43364e54 and guest media remain untouched. Remaining whole-package
caller/buffer and mirror reconciliation still govern S37 closure.

## Final-review findings: short path and computer name

The CP932 fixture now supplies original GetShortPathNameOem with the same
10-WCHAR/11-byte path through a GetShortPathNameW-shaped test provider. With
caller capacity 11, it returns 10 and writes the terminator at offset 11.
The source advertises cchDst*sizeof(WCHAR) as OEM capacity, and returns the
Unicode rather than converted length. Its finally return also requires
review because it can override early conversion-error returns.

GetComputerNameOem is separately supplied a one-WCHAR U+8868 name with caller
capacity 2. Unicode retrieval succeeds but OEM conversion needs three bytes.
The function ignores the failed conversion status and returns TRUE with
length zero and unchanged output. The fixture records this false success,
not an accepted behavior. Both mechanisms are in the original source; no
new product change is made in this delivery. The previous 30-capacity and
16-query-failure cases still pass. Fixture log is remaining-boundaries-build.log
under dbcs-r1; final markers explicitly identify open/unaccepted defects.

A normalized original comparison at product revision c43364e54, using
git diff --no-index --ignore-space-at-eol --numstat, reports:

| Mirror body | Versus pinned OpenNT added/deleted | Versus S37 admission added/deleted |
| --- | --- | --- |
| oemuni/file.c | 28 / 9 | 21 / 7 |
| oemuni/process.c | 99 / 33 | 91 / 28 |

Admission comparison uses S36 closure 5ad0ac874. These are gross diff lines,
not executable-statement counts; they include registered correction comments.
No overlay or adapter production code has been added in S37. The increases
are proven original conversion defects, not autonomous duplicate providers;
they still require the final minimality review. Pinned source hashes are
file.c `a3184eb571d6822577b52849e2da3955689899ecc6dfd9df100ff3f3636be5df`
and process.c `38c9125f9f3c43f32f696616e565e63b8f056da7ec264d980a5e5278cc640468`.
Short-path/computer-name repairs and environment-boundary reconciliation
remain S37 work; no package closure is claimed.

## Short-path and computer-name correction

DIV-276 retains the two original OEMUNI owners and their Win32/RTL
dependencies. Short-path Unicode input/output conversion and finally cleanup
remain in process.c; the complete-query sizing principle already justified
by original BaseClient is reused, not a new path resolver. Original
BaseClient vdm.c's GetShortPathName contract explicitly returns copied length
without NUL, required capacity on a short output and zero on failure.
Computer-name Unicode storage uses the existing MAX_COMPUTERNAME_LENGTH
contract instead of the caller's unrelated OEM capacity. No full BaseClient
import, replacement conversion provider, adapter or overlay is necessary.

Short-path output now uses actual OEM capacity (saturated to the original
USHORT limit), encoded required/copied lengths, and zero ReturnValue on
conversion failure so the original finally return cannot mask it. In the
computer-name function, conversion failure returns FALSE, reports required
OEM bytes, releases temporary Unicode storage and maps insufficient space
to ERROR_BUFFER_OVERFLOW. Other conversion status failures retain their
Win32 mapping. Unicode retrieval failure still follows the original cleanup.

All six x86 OEMUNI/DEM fixtures pass. Added short-path tests cover the same
six capacities, NULL output size query and injected conversion failure with
zero live temporary allocations. Computer-name tests cover insufficient
output (required 3), success with declared capacity 65536 (two encoded bytes),
NULL/zero-capacity query, and conversion failure. Existing real host fixture
and injected font/DEM cases continue passing. These CP932 edge tests remain
mock-boundary evidence; no real DBCS-host acceptance is claimed.

Formal x86 product rebuild succeeds; file.c delta is 16 added / 4 removed
lines and process.c is 19 added / 13 removed lines, including comments.
Deployed worker SHA-256:
`c0f5c5ba1d7256fcddf39b5a26166159f81bf9e5763e7b15bf2f63708566e346`;
DLL `29dd39ccd5f0df842b7105cb0035d80f766056f0df0d2773c288155df70d575b`.
Expanded real DOS direct/nested tests pass under s37-short-computer-guest-r1,
including the host-matched DOS computer name. Guest media remain immutable.
Environment-boundary and final package reconciliation remain S37 work.
All 17 Console-text-gated product regressions pass under
s37-short-computer-product-r1, including EDIT and original COMMAND exit values.

## Environment descriptor and failure-order verification

The current-source x86 native fixture reproduced a 65536-byte caller buffer
wrapping to zero in GetEnvironmentVariableOem's USHORT descriptor: an existing
`abc` value returned zero/error 234 without copying. The failing fixture
exited 17 before repair (env-recheck-build.log under the S37 dbcs-r1 root).
Original BaseClient `base/win32/client/process.c::GetEnvironmentVariableA`
already clamps capacities before narrowing and returns converted byte length.
Its original MVDM wrapper lacks those safeguards. DIV-277 applies them locally
without importing BaseClient, replacing the environment provider or adding an
adapter. The original ANSI query, RTL conversions and finally cleanup remain.

The local correction also copies the successful empty terminator and stops
after ANSI-to-Unicode conversion failure, before an invalid second conversion.
All six x86 OEMUNI/DEM fixtures pass: the large buffer returns 3 and `abc\0`;
short-buffer sizing matches the native provider; empty/deleted/missing values
retain their tested results. Fault injection proves error 8 and zero return
at both conversions, no borrowed-output free, untouched output, and exactly
zero/one OEM conversion calls respectively. These are native/mock boundary
checks, not claims that a DOS caller requests a 64-KiB buffer.

Formal x86 product build passes (env-product-build.log). Expanded real DOS
direct/nested OEM probes pass under `s37-env-guest-r1`. Deployed worker SHA-256:
`27c878324c79fd111ee61585e848003e4165ba58abf8307d09b0bc574a15bafb`;
VDMREDIR DLL:
`d0f5dc3bea50aafade55c8512c0afd5dac8fd2743d993f297b4b07cc3f1ada42`.
This correction changes process.c by 13 added / 3 removed lines. The current
normalized totals versus pinned OpenNT are file.c 44 added / 13 removed and
process.c 131 added / 49 removed; no adapter or overlay is added. Guest media
are untouched. ANSI/OEM unequal-byte-count short-query behavior and final
whole-package reconciliation remain open; S37 is not closed by these tests.
All 17 Console-text/exit-code product regressions also pass under
`s37-env-product-r1`, including repeated/nested MEM and EDIT return. The
original COMMAND nonzero results remain unchanged; no test was skipped.

## Unequal environment encoding lengths

The controlled CP932 OEM endpoint, with unchanged real host ACP 1252,
reproduced three short-buffer faults for U+00A7: ANSI is one byte, OEM is two.
Capacities 0/1 returned 2 rather than required 3; capacity 2 returned zero
instead of required 3. Capacities 3/65536 already succeeded. This is an
explicit mixed-encoding boundary fixture, not a Japanese-host claim.

Original BaseClient process.c GetEnvironmentVariableA's DBCS insufficient-
buffer branch fetches the complete intermediate value before computing the
encoded required size. DIV-277 now follows that order in the original MVDM
wrapper: query its original ANSI provider, allocate the complete intermediate,
retain the existing ANSI-to-Unicode conversion, then size and copy OEM bytes.
It does not replace the provider with a new environment implementation, import
full BaseClient or add an adapter/overlay. A value growing between queries
fails with error 234 before conversion; disappearance retains error 203;
unrepresentable capacity fails 234; allocation failure returns zero/error 8.
Finally cleanup remains original. These four injected paths preserve output
and leave zero counted intermediate allocations. All five capacities now pass.

All six x86 fixtures pass (env-sizing-all-build.log), and formal product build
passes (env-sizing-product-build.log) in the existing S37 dbcs-r1 root. Real
DOS direct/nested probes pass under `s37-env-sizing-guest-r1`. Worker SHA-256:
`ac057e500139446953311d06fa9d9a07fb2f955a6d5bdc953cf3877151412ef5`;
VDMREDIR DLL:
`128849cd3f38e3193e7ae61d88923ad919a242552678571e58010ddbb34dde91`.
This step changes process.c by 25 added / 5 removed lines; guest media and
production adapter/overlay sources remain untouched. It closes this specific
environment short-query defect, not the remaining package/caller audit.
All 17 transcript-gated product routes pass under
`s37-env-sizing-product-r1`, including nested COMMAND/MEM and EDIT. The
documentation governance and whitespace gates pass as well.

## Real guest environment caller witness

The original `dos/v86/doskrnl/dos/macro.asm::GetCDSFromDrv` synchronizes a
drive through CMDSVC 54:04 with AL=drive and DS:SI=output. Its original
`dos/command/cmdmisc.c::cmdGetCurrentDir` reads the `=X:` variable through
GetEnvironmentVariableOem. Original `dos/dem/demdir.c::demSetCurrentDir`
sets that variable with SetEnvironmentVariableOem after the real chdir succeeds.
The expanded disposable probe now performs INT21 chdir to its non-ASCII
private directory, explicitly issues that same BOP contract, prints the
returned absolute path, and retains all prior OEM file/directory/FCB/disk tests.
This proves actual guest-to-COMMAND-to-OEMUNI readback, not an inferred trace
hit. It does not claim the DOS kernel's conditional CDS-cache branch was
necessarily selected on every call, nor prove the distinct WOW consumer.

First run `s37-env-caller-guest-r1` completed guest exit zero with all markers
but the harness failed the path comparison: its UTF-8 default decoded raw
OEM437 byte 9C as a replacement character. Original observer source uses
ReadConsoleOutputCharacterA and writes raw bytes; a hex inspection and the
independently created Unicode directory confirmed the exact pound-sign byte.
The harness now decodes its already-required OEM437 profile explicitly. Only
the exact verified test file and its empty directories were removed before
retesting; no runtime media or user file was changed.

Both direct and nested routes pass under `s37-env-caller-guest-r2`, including
`S37_OEM_GUEST_COMMAND_ENV_OK` and the host-matched path. Probe SHA-256:
`61dc69d0f54f37815b0c85484f73285b2954967c5fff6c51634da5ac5c308603`.
NASM builds `tests/observation/oem_file_contract.asm` with `-f bin` to
`build/M0-T420/S37/guest-env-r1/O37.COM`; only this independent probe is copied
below runtime tests. Product source/artifacts are unchanged from 08977bd17
and its formal x86/six-fixture/17-route verification. This step adds no mirror,
adapter or overlay diff. PIF-specific path/expansion and remaining capability
dispositions still require final review before S37 closure.

## PIF consumer encoding investigation (open)

Independent `oem_pif_contract.asm` reports the guest startup directory and
`S37_PIF_GUEST_OK`. The existing pure-DOS PIF builder now accepts optional
`--oem-paths`: its NT configuration names use `%S37_CONFIGROOT%`, and its
start directory uses that variable too. The default builder mode is unchanged.
MSVC x86 compiles the builder and NASM assembles the probe under
`build/M0-T420/S37/pif-guest-r1`. These are diagnostic inputs, not a passing
new capability or replacement guest media.

Observed results and their distinct meanings:

- `s37-pif-guest-r1`: direct PIF execution in an inherited Console returns
  193. Original BaseClient vdm.c BaseCheckVDM explicitly rejects this case
  without CREATE_NEW_CONSOLE. No policy bypass is authorized by this test.
- Associated `O37P.PIF` selects configuration while launching `O37P.COM`.
  The initial pure-DOS variants timed out; the unchanged-mode baseline later
  exited zero without the required marker. An independent S30 P30.COM ConPTY
  rerun also exited zero without its marker (`s37-pif-s30-control.raw`). None
  passes. The recorded low-DOS guest INIT/environment defect is relevant
  context, not a proven attribution of these particular executions.
- With byte-identical copies of the existing default CONFIG/AUTOEXEC media,
  literal ASCII configuration paths pass (`s37-pif-default-media-r1`), as do
  environment-expanded ASCII paths (`s37-pif-default-expanded-ascii-r1`).
  Both show `S37_PIF_CWD=WINNT` and `S37_PIF_GUEST_OK`. Original cmdpif.c
  skips start-directory/application policy for a non-PIF task with no new
  Console; this is not proof that the PIF start-directory branch ran.
- Moving only those identical configuration bytes to `tests/D` plus U+00A3,
  and changing the process-local S37_CONFIGROOT accordingly, times out
  (`s37-pif-default-expanded-oem-r1`). This is unresolved failure, not success.

CONFIG source/copy SHA-256 is
`6ae1e74e78de00c0fa03124e6f947d977c71f253855ad2af979554074aea189a`;
AUTOEXEC is
`c2a23a5fb96682c33d99c13a57e6f3c1e10dfd3d263d972775a8c42bfb6a9a9b`.
No original configuration or guest binary was modified. Copies stay below
runtime tests, logs below runtime logs; process-local variables are removed
after each run and only verified test-owned orphan processes are terminated.

Source review finds an inconsistent consumer contract: nt_pif.c's PIF override
calls ExpandEnvironmentStringsOem, whose repaired result is OEM; both
nt_msscs.c's LIM configuration reader and cmdconf.c's ExpandConfigFiles call
ANSI CreateFile on that result. The original default branch used ANSI
GetSystemDirectory, and the current default uses the existing package-path
binding. Therefore blindly changing both consumers to OEM is not a complete
fix. The next S37 step must establish one bounded output encoding for both
branches, preserve the original owners/cleanup, and rerun this exact A/B.
No production change or renewed product acceptance is claimed in this step.

## PIF encoding repair and corrected isolation

Deployment inspection found surviving workers from earlier ASCII PIF test
launchers 58216/45320. Original PIF task completion need not terminate a worker.
The preceding timeout A/B therefore cannot prove an isolated encoding cause;
pure-DOS controls also require fresh-worker revalidation. Only the exact test
processes were stopped. The new verifier refuses pre-existing package processes
and terminates only children of each recorded test launcher after observation.

Without an old worker, unmodified 08977bd17 still failed the non-ASCII path:
`s37-pif-oem-clean-before-r1` exited zero but emitted no guest marker. After
repair, identical media/PIF/environment produced `S37_PIF_CWD=WINNT` and
`S37_PIF_GUEST_OK` in `s37-pif-oem-clean-after-r1`. Exit zero alone is not a pass.

Recovery ladder: original GetPIFConfigFiles, OEMUNI expansion, LIM and COMMAND
readers remain compiled. Their original ANSI consumers/default path cannot
directly consume repaired OEM expansion. The smallest same-shaped handoff is
one hook to the existing worker media-path binding: bounded public OEM-to-
Unicode-to-ANSI conversion, not a second environment expander/file provider.
The unbounded OemToChar API cannot enforce caller capacity when encoding grows.
No third-party intrusion, new mirror file or autonomous environment/file policy
is needed. The finite capacity-aware handoff is registered under existing
ADAPTER-SOFTPC-024/025 and DIV-278. Original read/failure/cleanup policy remains.

The helper rejects absent/zero/oversized/unterminated input, uses bounded stack
Unicode storage, and opens/allocates nothing. The original caller clears its
path on failure. LIM's output buffer now matches GetPIFConfigFiles' documented
MAX_PATH+12 contract; COMMAND already supplied that capacity. The default ANSI
path and both original CreateFile consumers remain unchanged.

`Verify-T420S37PifPaths.ps1` passes fresh-worker ASCII/OEM rows with identical
default media at `s37-pif-clean-matrix-r1`. It gates on guest text and exit,
checks inherited-Console cwd policy, restores process environment and removes
its exact test objects. The seventh x86 fixture links the actual helper;
unused session functions are fail-fast sentinels, never called. Non-ASCII,
ASCII, empty, absent, zero/oversized-capacity and unterminated tests pass.
All six prior fixtures pass; Ninja includes the new fixture in the same set.

Formal x86 build passes (S37/pif-guest-r1/product-build.log). Deployed worker:
`d7427a755f5327b835bd4abc22726d963d91b5292fd2a3f55bb907e7e529637d`;
VDMREDIR DLL:
`c4c28147423b96c0197c917a9e7ddd6952b8c0818c44f465950a376daa18c5c7`.
Expanded real DOS direct/nested probes pass at `s37-pif-fixed-oem-r1`.
Original guest/configuration media are untouched. Whole-package reconciliation
and isolated pure-DOS follow-up remain open; S37 is not closed.
All 17 transcript-gated product routes pass at `s37-pif-fixed-product-r1`.
Production footprint for this repair is nt_pif.c +3/-1, nt_msscs.c +2/-1,
and the existing worker boundary +16 implementation lines/+4 declarations;
there is no overlay. Tracked nt_msscs.c LF is restored before delivery,
avoiding a format-only whole-file diff. Both governance gates pass.

## Fresh pure-DOS comparison and current-source environment witness

Question: does the failed pure-DOS control implicate the S37 OEM/PIF repair,
or does this exact input reach the previously accepted immutable-guest defect?
The comparison uses the same classic observer from S34/nested-console-observer-r1,
`run16.exe tests\P30.COM`, a 15000 ms observation limit, ordinary inherited
environment, and no pre-existing package processes. Each historical comparison
replaces all five package artifacts together; the current five are retained in
S37/pure-baseline-compare-r1 and restored afterward. Only children of the exact
recorded test launcher are terminated. No guest input or environment policy is
changed between package selections.

Input SHA-256:

- P30.COM: `706b0e5297624e5bc5d3ab3f4789b2d7f6fed3838ed6ddceebd799fe2cc00682`.
- P30.PIF: `1e2ab254d79d548316ce29ff995b20d93a09c8daf9010a5bc9cf6ecea9be10af`.
- P30.NT (`files=20`): `f9a6eb29dbde8837465d934508bc654704a8e989b391947faa8bbb30ed6a87dc`.
- P30AUTO.NT (`@echo off`): `c134b2f85415ba5cfce3e3fe4745688335745a9bb22152ac8f5c77f190d8aee3`.
- Original COMMAND.COM, deployed and mirrored:
  `908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43`.

| Package | Runtime log prefix | Observation |
| --- | --- | --- |
| S30/formal-x86-r2, worker `8148dde31b9b4a5296f77da358df0ae3329e840a9200796738667a4877e6fee8` | s37-pure-s30-package-r1 | Exit 0 and S30_PURE_DOS_OK. |
| S35/formal-native-wait-r2, worker `568292e2640d82cd8c5a542b489bc291e0decefe70a063be3aa3e0651f5853e2` | s37-pure-s35-package-r1 | Timeout; blank guest transcript. |
| Current formal worker `d7427a75...9637d` | s37-pure-current-console-r1 | Timeout; blank guest transcript. |

The current ConPTY control `s37-pure-clean-r1.raw` also lacks the required
marker. The S35 classic failure establishes that this symptom predates S37;
it does not by itself explain why the S30 package passes. A report-reader typo
after the S35 run omitted `.txt` from the console filename; the actual retained
`s37-pure-s35-package-r1.txt.console.txt` contains only buffer geometry.
The finally block still restored the current package and verified its hash.

The existing diagnostic includes the unchanged current cmdenv.c and surrounds
only cmdGetInitEnvironment with read-only leases. Its new compile-time log-path
option keeps this run separate from historical S35 logs without injecting any
guest or host environment variable. The build script restricts that path to
the runtime logs directory. Default historical behavior is unchanged; the
formal product graph does not select this wrapper.

Command: `Build-T420S35EnvironmentTrace.ps1 -FormalRoot
build/M0-T420/S36/original-callback-product-r1 -BuildRoot
build/M0-T420/S37/pure-environment-trace-r1 -EnvironmentTraceLog
O:/winnt/logs/s37-pure-envtrace-r1.events.txt`. Its reused-inputs.json hashes
every reused formal input. The x86 diagnostic links successfully, SHA-256
`3541ac41cd36eee16c98d6522e53769b7015c18597814393cfc95254855d093b`.

The identical P30 workload records:

| Boundary | DS | ES | BX | EnvSiz | MCB owner / paragraphs | Contains EnvSiz |
| --- | --- | --- | --- | --- | --- | --- |
| First entry | 0BF4 | 0D22 | 0010 | 0010 | 0BF4 / 0010 | No |
| First return | 0BF4 | 0D22 | 0119 | 0010 | 0BF4 / 0010 | No |
| Second entry | 0BF4 | 0D22 | 0119 | 0119 | 0BF4 / 0119 | Yes |
| Second return | 0BF4 | 0D22 | 0119 | 3B48 | 0BF4 / 0119 | Yes |

The environment allocation `[0D220,0E3B0)` contains the discarded INIT
variable at `DS:203C = 0DF7C`. The original in-bounds copy overwrites EnvSiz;
the next original comparison of BX with EnvSiz must mismatch. Original
command.map and rdata.asm::EndInit identify that offset and the subsequent
branch to INIT-resident Alloc_error. This is direct evidence that this exact
current pure-DOS workload reaches the already documented guest lifetime bug,
not merely a similar timeout. The diagnostic exits zero without the required
guest marker: still a failed workload, never a successful acceptance result.
No environment contents are logged, and no guest byte is patched.

Current formal artifacts are restored afterward. This delivery adds no product
or mirror diff, no overlay and no workaround. The historical S30 success is
not evidence that the guest defect is absent: its different execution/layout
still needs attribution before claiming a complete historical explanation.
S37 remains open for that reconciliation and the remaining caller ledger.
The restored formal package passes all 17 transcript-gated COMMAND/MEM/EDIT
regressions under `s37-pure-post-restore-r1`. Those default-profile passes do
not turn the pure-DOS failure into a pass.

## Historical pure-DOS false positive resolved

The historical difference is now attributed, superseding the open question
above. Commit 4ff95fa9d repaired GetPIFConfigFiles' one-read ownership: original
config.c invokes LIM configuration first, nt_msscs.c consumes the PIF CONFIG
path, then cmdconf.c::ExpandConfigFiles requests it again for DOS boot. The
S30 implementation frees and clears that path on the first read, so DOS uses
the default CONFIG containing DOS=HIGH and HIMEM. The independent AUTOEXEC
path still selects P30AUTO.NT and therefore excludes DOSX. Absence of DPMI
alone could not distinguish this mixed profile from the requested pure profile.

The diagnostic command used above was repeated with FormalRoot set to the
retained S30/formal-x86-r2 graph, BuildRoot S37/pure-s30-environment-trace-r1
and EnvironmentTraceLog s37-pure-s30-envtrace-r1.events.txt below runtime logs.
cmdenv.c has no source diff between 91a5be61a and current HEAD. This is a
diagnostic relink using archived formal inputs, not a source-exact rebuild of
the whole historical product; reused-inputs.json records those input hashes.
The diagnostic worker SHA-256 is
`9a8b07b14bd7752b4fbf1114f010e9719b114a2f86f20d5433a1116d9f87a12d`.
The same original P30 workload emits S30_PURE_DOS_OK and exits zero. Its trace
has DS=0710, ES=07BB, BX growing from 0010 to 0119; after the second call
EnvSiz remains 0119. The allocation `[07BB0,08D40)` does not contain
`DS:203C = 0913C`, unlike the current low-DOS allocation. This corroborates
the different layout but is not alone proof of the DOS placement policy.

An independent real guest check provides that proof without instrumentation:
original doskrnl/dos/msdisp.asm's INT21 AX=3306 returns DosHasHMA in DH;
dos/v86/inc/versiona.inc defines DOSINHMA as bit 4. The strengthened authored
pure_dos_capability.asm checks that bit after the original DPMI query and
rejects unexpected HMA residency. NASM emits S37/pure-profile-contract-r1/
P37L.COM, SHA-256
`ee2ff94a749b0bbaaaa5f14ef678cb45bea3c6b9caa7ad4798f97b7a3f1d0fa2`.
It is deployed only as tests/P37L.COM with a byte-identical associated copy
of P30.PIF; original P30 inputs and guest media remain untouched.

On the uninstrumented complete S30 package, `s37-pure-s30-hma-r1.txt` returns
1 and its guest transcript contains `S30_PURE_DOS_UNEXPECTED_HMA`. This is a
passing negative test of the strengthened assertion, not pure-DOS acceptance.
The former S30 success therefore proved only the no-DOSX half of the profile.
Its full CONFIG/pure-DOS claim is withdrawn in the linked S30 record. The
corrected configuration path exposes the already owner-accepted original
COMMAND INIT/environment lifetime defect; restoring the consumed-path bug or
forcing DOS=HIGH would hide that defect, not repair it.

Only test-owned processes are terminated; all five current formal artifacts
are restored, including worker d7427a75...9637d. This step changes only a
disposable probe and evidence, with zero product, mirror or overlay diff.
The preceding 17-route formal regression remains applicable to those unchanged
artifacts. S37's historical pure-DOS attribution is complete; remaining
OEMUNI caller/whole-package reconciliation still prevents S37 closure.

## Real volume-query consumer and original DEM padding repair

The remaining GetVolumeInformationOem consumer is now exercised by the real
DOS probe: INT21/6900, original misc.asm drive normalization, demGSetMediaID,
GetMediaId and OEMUNI. The probe checks its trailing canary and prints all
25 bytes of the packed result. The verifier obtains independent Unicode host
volume information without changing the label, converts expected fixed-width
fields to OEM437, and compares serial, label and filesystem bytes exactly.

Before repair, `s37-volume-before-r1-direct.txt.console.txt` shows:
`00002FE93528636F6465202020202020204E54465300000000`.
The host-derived expected record ends in `4E54465320202020`. The guest query
itself succeeded, but the strict oracle correctly rejected the NUL padding.
Source review of both the current and pinned OpenNT demgset.c identifies the
same typo: the filesystem NUL loop writes VolumeID instead of FileSystemType.
On a longer volume label this also destroys label characters at those indices.

Recovery ladder: the complete original DEM translation unit and OEMUNI remain
selected. Direct original composition reproduced incorrect bytes. A binding
cannot correct the private result assignment without duplicating owner policy.
DIV-279 changes that one field reference in the original GetMediaId owner;
the original query, structures, iteration, failure order and cleanup remain.
No new interface, adapter, overlay, external-code import or guest change is
needed. The mirror delta is +2/-1 including its one-line rationale, not a
new implementation. Tracked LF is preserved to avoid format-only churn.

The strengthened authored probe is S37/guest-volume-r1/O37.COM, SHA-256
`dd643a9d94acc08e8d1548fee4b933e8f3747fcdad20bf9466cad75b14a87f78`.
After the formal x86 six-edge build, direct and two-level nested COMMAND routes
pass under `s37-volume-after-r1`, including all preceding file/FCB/directory/
environment assertions. The actual record now matches the host oracle. The
failed run's exact created payload and empty directories were checked and
removed before rerunning; original guest media were not touched.

The eighth native fixture extracts the actual GetMediaId body and packed
VOLINFO declaration, with source hash and generated dependencies, rather than
copying its algorithm. Only the OEM query endpoint is mocked. Four cases cover
an eleven-character label with short filesystem (would expose label overwrite),
short names, empty names, and truncated label with exact-width filesystem.
All preserve info level and surrounding canaries. The fifth query-failure case
returns ERROR_NOT_READY and preserves the complete output. It reports
S37_DEM_MEDIA_PADDING_LABEL_CANARY_FAILURE_OK. This is mock owner-contract
evidence, separate from the real DOS tests. All seven existing OEMUNI fixtures
also pass from S37/media-fixtures-r1 using current formal RTL inputs.

Deployed formal worker SHA-256:
`c7150def0ac9d2a4cb6b280bfbbbe5185245c6bef2c165ec6460f092cfd38d26`;
VDMREDIR DLL:
`01cf9060393c1b3a4e3856c8da10251ca11158ef4e6b1939cf552dec4608f75b`.
All 17 text-gated product routes pass under `s37-volume-product-r2`.
The attempted r1 prefix was rejected because it belonged to an earlier S37
run; its older results were not used to validate this build. Process inspection
confirmed no active test before selecting the fresh r2 prefix. S37 remains
open for final caller and residual-diff reconciliation, not another volume fix.

## Formal PIF caller boundary verification

cmdpif.c::cmdCheckForPIF skips startup policy for a non-PIF application with
DosSessionId zero. Its new-console/explicit-PIF branches own full/search/short
OEM path queries; BaseCheckVDM rejects explicit PIF execution on an inherited
Console. Associated-PIF CONFIG tests therefore do not prove those startup
branches. DEM SignalSegmentNotice's full-path call is behind IsDebuggee and
belongs to debugger workload acceptance, not ordinary COMMAND/MEM evidence.

The new oemuni_pif_caller_fixture links the actual formal cmdpif.obj, SHA-256
`443b8f29207fe2e6e3a825f219526548748844e46d7ab353f4b99d8058e3942f`,
current OEMUNI objects and formal RTL inputs. It uses original VDMINFO,
PIF_DATA and COMMAND declarations with formal owner compile flags. Task/PIF
inputs, literal-only expansion, error presentation and the final guest scalar
sink are controlled boundaries. Unexpected PIF loading terminates the fixture;
there is no fake parser. OEM full/search/short paths, attributes, directories
and environment APIs are real. Native Unicode APIs independently check the
selected non-ASCII name and cwd. This is host/caller evidence, not a guest
launch, parser test or real new-console acceptance.

Five scenarios pass in both pif-caller-capacity-1.txt and
pif-caller-capacity-2.txt under S37/pif-caller-r1:

| Input | Original observed result |
| --- | --- |
| Explicit PIF, first/new-console task, valid non-ASCII directory/file | No error; one scalar write; correct short path, AppLen, command tail and cwd. |
| Associated application, inherited Console | Original early skip; no scalar/cwd/command change; PIF strings cleared. |
| Missing startup directory | Error 19 (EG_PIF_STARTDIR_ERR); no scalar write; strings cleared. |
| Missing application | Both original searches fail; error 20 (EG_PIF_STARTFILE_ERR); no scalar write; strings cleared. |
| Existing unsupported extension | Error 28 (EG_DOS_PROG_EXTENSION); no scalar write; strings cleared. |

Each case restores cwd; successful completion removes both empty files and
private directories. Both executions emit
S37_ORIGINAL_PIF_CALLER_OEM_PATH_ERRORS_CLEANUP_OK.

The first fixture incorrectly used strdup for expandable fields. One run
failed silently, one happened to pass, and a repeat failed with heap corruption
(C0000374). All are discarded as malformed-input evidence: original nt_pif.c
allocates MAX_PATH+1 for StartDir, StartFile and CmdLine. Correcting those
fixture capacities yields the two clean results above, with no product change.
Failed-run files were verified empty before exact nonrecursive cleanup.

This step adds no product/mirror/overlay changes; the previous formal package
and 17 s37-volume-product-r2 passes remain unchanged. Remaining S37 work is
explicit: real no-console/new-console PIF entry coverage or justified original-
owner disposition, final WOW/debugger consumer handoff, and byte/normalized
diff accounting. A host fixture alone does not close real entry coverage.

## Detached explicit-PIF entry: failed integration observation

The test-only pif_detached_observer.c starts the installed run16 with
DETACHED_PROCESS in a private kill-on-close job, then attempts to observe a
descendant ntvdm Console. It does not change guest media or product options.
The x86 observer built without warnings in S37/pif-detached-r1. With the
existing O37P.COM probe and generated P37DET/P30.PIF, using byte-identical
default CONFIG/AUTOEXEC copies, logs/s37-pif-detached-r1.txt records launcher
40680, wait=0, exit=6. No worker Console was observed. This is a failure,
not PIF or OEMUNI acceptance. A subsequent package process query found no
remaining run16, basesrv or ntvdm processes.

Source review finds a concrete standalone binding gap. A fresh service
connection starts with a null console. OpenNtBaseServiceCheck assigns an
identity for an existing-Console request, or for its explicitly separated
resident-DOS case, but not for an initially null DOS Console. After original
CheckDOS publishes a new record, OpenNtBaseServiceCreateReservation rejects
that null console with ERROR_INVALID_HANDLE. The observed exit is consistent
with this path; it is not yet a traced call-site attribution.

Separately, run16 compares the complete binary value with BINARY_TYPE_DOS
when supplying the DOS session id and selecting CREATE_NEW_CONSOLE. An
explicit PIF also carries BINARY_TYPE_DOS_PIF; original BaseCheckVDM masks
the subtype before its DOS policy. This is another integration candidate,
not evidence that the guest or OEM conversion failed.

No production repair is made in this observation. Repairing broker Console
identity/reservation and launcher subtype handling expands the current
OEMUNI surface and must be explicitly admitted under the active packet's
original-owner boundary stop condition. S37 remains open; neither the five
host cmdpif scenarios nor the previous 17 regressions cover this failed entry.

## Current footprint and cross-package consumer reconciliation

At 4ca7671b5, re-running git diff --no-index --numstat against pinned OpenNT
and repeating with --ignore-space-at-eol gives identical counts for both
OEMUNI units: file.c +44/-13, process.c +151/-49. These are diff-line counts,
not runtime capability counts. Current SHA-256 values are respectively
83cdb96808ad79df98c608be95cdd16f7229b2e4946b86e6a473edae8f2407ce and
2337d69ca349f29ba4790620cf10080e7ff6aa19b28b4299007970cb8572f5a2.

| Production mirror | Current normalized diff versus pinned OpenNT | S37 change versus admission 5ad0ac874 |
| --- | --- | --- |
| oemuni/file.c | +44/-13 | +37/-11 |
| oemuni/process.c | +151/-49 | +143/-44 |
| dos/dem/demgset.c | +37/-14 | +2/-1 |
| softpc.new/host/src/nt_pif.c | +21/-7 | +3/-1 |
| softpc.new/host/src/nt_msscs.c | +50/-16 | +2/-1 |
| Total for these five files only | +303/-99 | +187/-58 |

The total is not a repository-wide mirror inventory. S37 additionally adds
16 implementation and four declaration lines to the existing worker-local
firmware binding for OEM-to-ANSI CONFIG paths. There is no new overlay or
adapter component. README registration and all tests/tools/evidence are
excluded from production code counts. This packet increases corrective code;
it must not be reported as net diff reduction.

Source reconciliation confirms these outstanding workload dependencies:

| Original caller | Contract still requiring its real consumer | Acceptance owner |
| --- | --- | --- |
| wow32/wdos.c, DIR_NT_TO_DOS | GetCurrentDirectoryOem followed by guest default-drive and current-directory publication; failure must not publish a false directory | S40 WOW32 and S41 actual WOW16 workload |
| wow32/wkman.c, WK32WowDelFile | Delete succeeds but attributes still exist: full path, temporary name, replace-existing rename, delete, rollback and temporary-file cleanup; font-removal fallback is a separate branch | S40 WOW32/S41; host API tests alone do not establish guest reachability |
| dos/dem/demmisc.c, SignalSegmentNotice | IsDebuggee gates full-path expansion and the original module notification | S42 debugger workload |

For WK32WowDelFile, an ordinary successful delete does not exercise the
GetTempFileNameOem/MoveFileExOem branch. Modern filesystem reachability must
be measured by that package, not assumed from a linked symbol; an original-
body injected boundary test can supplement, but cannot masquerade as, the
real guest result. Existing OEMUNI host fixtures establish API contracts only.
This table records dependencies, not an exclusion or a reduced S37 exit bar.
The proposal's real Win16 wording and the explicit-PIF failure remain open;
S37 is not closed and no WOW/debugger success is claimed.

## Owner-approved detached PIF binding repair

The owner approved extending S37 to the identified broker/launcher boundary.
The unchanged formal x86 graph is intentionally reused; build and fixture
logs are under S37/pif-binding-r1 and S37/pif-environment-fixtures-r1.

The reservation fixture now submits a genuinely null-Console DOS request.
Before repair, original CheckDOS returns task 1 successfully, immediately
followed by reservation-error=6. After repair, two successive cases return
tasks 1 and 2 with reservation-error=0; disconnect/rundown leaves the service
empty, and the existing full Check/Update/Get/Exit/stream/WOW fixture passes.
This resolves the previous source-only attribution of the error-6 boundary.

Recovery ladder: original BaseCheckVDM, CheckDOS, BaseGetVdmConfigInfo and
UpdateVDMEntry remain compiled owners. Private NT4 Console handles cannot be
transported as modern broker identities; the already-admitted finite Console
binding is reused, not replaced. Its existing allocation now covers both
initially detached DOS and explicitly separated busy-Console DOS. The source
request stays null until original PIF acquisition publishes the identity.
run16 clears BINARY_SUBTYPE_MASK after successful new-record CheckVDM;
configuration, new-Console creation and update then consume DOS, not DOS|PIF.
No new wire protocol, record policy, mirror algorithm or external intrusion
is introduced. A new standalone implementation rung is unnecessary.

### Newly reached missing guest-environment input

The first repaired run creates ntvdm.exe -f -i1. Existing RPC trace proves
PIF and first-command Get requests succeed, but the observer records the
original Invalid startup directory dialog. Comparing cmdmisc.c with pinned
OpenNT proves this is a project regression, not an original-source bug:
the original first call binds cmdVDMEnvBlk.lpszzEnv to the completed guest
environment at EnvSeg:0; the existing DIV-194 lease conversion replaced it
with NULL. Original cmdExpandEnvironmentStrings consequently has no variables.

DIV-194 now wraps only the original PIF call with a bounded copy of that
first-command input. The existing guest-state component acquires a read lease,
copies EnvSize bytes, releases before any host/UI call, checks double-NUL
termination, temporarily binds the copy, calls unchanged cmdCheckForPIF, and
clears/frees it. Existing merged environments and non-first calls stay on the
original direct path. Unavailable/unterminated input and allocation failure
return explicit errors. No guest write, replacement expansion algorithm,
durable alias, new mirror file or overlay is used.

### Real execution and controls

Logs below are under O:/winnt/logs. All PIFs and probes are independently
authored inputs; default CONFIG/AUTOEXEC copies and original guest binaries
remain unchanged.

| Observation | Result and interpretation |
| --- | --- |
| s37-pif-detached-fixed-r3.txt | Before environment-binding repair: original invalid-startup-directory UI, not silent success. |
| s37-pif-literal-r1.txt | Correct cwd and S37_PIF_GUEST_OK; no close-on-exit bit intentionally leaves original Inactive worker. Timeout is not a normal-exit pass. |
| s37-pif-close-r1.txt | Literal startup with original close-on-exit bit: cwd WINNT/TESTS/P37END, success marker, launcher exit 0. |
| s37-pif-env-fixed-ascii-r1.txt | Malformed test expectation used host SystemRoot for a probe actually under O:/winnt; startup-directory expansion succeeds but program-name error follows. Discarded as a positive test. |
| s37-pif-env-fixed-ascii-r2.txt | Explicit test-variable expansion, cwd WINNT/TESTS/P37ENV2, guest marker and exit 0. |
| s37-pif-env-fixed-oem-r2.txt | Same with OEM437 pound-sign directory P37E£: exact cwd, guest marker and exit 0. |

The retained builder uses S37_CONFIGROOT for both startup program and directory;
the probe is copied into that private directory. This corrects the test, not
the product environment. Literal mode remains a separate control. The observer
reports only its job-owned worker's windows and Console output and closes its
private job on completion/timeout.

All ten native OEMUNI/PIF fixtures pass. The new fixture compiles the actual
guest-state source, asserts copying rather than aliasing and release before
the original-consumer call, and injects allocation/acquire/release failures,
missing double-NUL and undersized input. It checks no residual allocation or
published pointer and preserves an existing environment. Unrelated same-TU
functions have fail-fast stubs; they must not execute. Native results live in
S37/pif-env-r1. An initial overly long fixture cwd correctly failed original
PIF's 64-character startup-directory limit; only the short-root rerun is the
positive result. Earlier compile failures (fixture printf import and an
incorrectly named copy helper) were corrected before runtime publication.

Final formal product build: pif-binding-r1/environment-build-r3.log.
All 17 established routes pass in s37-pif-binding-product-r2-summary.json;
r1 is the intermediate broker-only product and is not final-product evidence.
Final real OEM direct/nested probes also pass under s37-pif-final-oem-r1,
including the volume record's independent host oracle.

| Deployed artifact | SHA-256 |
| --- | --- |
| run16.exe | ce626741201ab987b5e979a838369c898c59797582f2c28ae801d883cbae1b71 |
| basesrv.exe | 0f25523c5e44cbf2fd464a51227386da05bde5fd9e9905f7c9985eca0afa98c2 |
| ntvdm.exe | ee095a1f46c1b67b53913d4c0a073c6a80b54456f004be798ed995b8381b8039 |
| VDMREDIR.dll | 66e3edc583e54e97f35cf487ce811e6d7c85f6ed7f1ec156f2d7f00c11bbc452 |

This delivery adds +5/-3 mirror lines in cmdmisc.c, +40/-0 existing worker
binding/header lines, and +12/-3 launcher/broker lines (comments included;
tests/docs excluded). No overlay grows. S37's mirror change since admission
is now +192/-61 across six files; this is corrective growth, not diff removal.
Real explicit PIF entry is now evidenced, but the previously recorded WOW/
debugger workload dependencies still prevent claiming complete S37 acceptance.

## S37 closure with explicitly accepted receiver obligations

After delivery 6b80471d9 the owner approved closing the verified OEMUNI/DOS
scope and transferring real WOW/debugger consumer acceptance, requiring that
the receiving S briefs carry explicit additional criteria. This section
supersedes the preceding provisional statements that those dependencies keep
S37 open; it does not turn any unrun consumer workload into a pass.

The proposal now binds four named acceptance items:

- S40: OEM-WOW-DIR, original WOW directory/drive/environment synchronization,
  including non-ASCII input, failed publication and task cleanup.
- S40: OEM-WOW-DELETE, ordinary and retained-file delete branches, temporary
  names, replace-existing rename, rollback, cleanup and separate font fallback.
- S41: OEM-WIN16-INTEGRATION, real guest task/callback/return/teardown validation
  of those S40 contracts, retaining mandatory WRITE and subsequent DOS usability.
- S42: OEM-DBG-PATH, actual debuggee non-ASCII module-path conversion and
  notification, failed-path behavior and cleanup, or the package's strictly
  source-proven complete-profile-exclusion alternative.

Each receiver must copy its named item into admission and test/closure evidence.
API fixtures, linked symbols and ordinary DOS regressions cannot discharge
these obligations. Branches lacking a real trigger retain boundary injection
plus an explicit reachability disposition; they cannot silently disappear.
Implementation stays with the original consumer owner; S41 revalidates rather
than duplicates S40's implementation.

The 28-function ledger is reconciled with later evidence: full/search/short
path and expansion now have real detached-PIF proof; volume information has
real DOS data checked against the host oracle. Host-only uncalled helpers,
CP932 mock limits and the demonstrated immutable guest pure-profile defect
remain classified as recorded above. Neither full DBCS-host nor Win16 success
is asserted. The original disabled CreateProcessOem remains outside the 28.

Closure rechecks confirm all 17 expected results match and all four deployed
artifact hashes match the final repair record. No source/build input changed
in this documentation-only delivery, so the final x86, ten fixture and OEM
direct/nested/PIF evidence applies without another rebuild. S37 closes;
T420 remains open, with S38 as the next sequential package.

Final S37 change against 5ad0ac874: six mirror files +192/-61; non-mirror
production code/header +72/-3; no new overlay. Documentation/tests are excluded.
This is corrective growth, not autonomous-code reduction. The receiving
packages cannot use this closure to waive their new mandatory tests.
The six changed mirrors were also re-compared against pinned OpenNT with
raw and end-of-line-normalized numstat: both give identical counts per file.
cmdmisc.c is +201/-35; the other five retain the preceding table's +303/-99
aggregate. These upstream totals include pre-S37 differences and must not be
reported as changes introduced by S37.
