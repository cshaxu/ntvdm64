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
| GetEnvironmentVariableOem | COMMAND cmdenv/cmdmisc; WOW wdos | H non-ASCII/missing; caller-specific guest witness pending |
| SetEnvironmentVariableOem | DEM demdir; COMMAND cmdpif/cmdmisc; WOW wdos | H set/delete; caller-specific guest witness pending |
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
