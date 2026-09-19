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
