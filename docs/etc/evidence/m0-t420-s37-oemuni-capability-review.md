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
