# T202 S3: DEM file/handle/FCB/search source and ABI map

## Question

What is the complete non-overlapping DEM file/handle/FCB/search work set after
T202 S2, and which historical host surfaces can be used by the modern CLI
without importing CCPU/SAS, VDMREDIR, or raw host handles into guest state?

## Inputs and procedure

- Original dispatch order: `base/mvdm/dos/dem/demdisp.c:apfnSVC`.
- Original owners: `demfile.c`, `demhndl.c`, `demfcb.c`, `demsrch.c`, and
  `demdir.c`.
- Current routes: `boot_namespace_{provider,plane}`, `readonly_namespace`,
  `dem_fcb_provider_v1`, `dem_*_search_service_v1`, and the temporary DEM
  fallback/facade routes.
- The T200 S31 all-BOP ledger and source policy.

The table below deliberately lists all 29 identities admitted to this S. It is
an admission map, not evidence that a present bx-vdm recognizer is a provider.

## Identity and owner map

| BOP | Original handler / primary source | Guest ABI / state owner | Host surface and modern verdict | Direct / readonly / overlay / virtual disposition | Existing workaround action |
| --- | --- | --- | --- | --- | --- |
| `50:00` | `demChgFilePtr`, `demhndl.c` | AX:BP opaque file handle; CX:DX offset; BL origin | `SetFilePointer`; ordinary API, but host `HANDLE` must become a private adapter token | direct opaque-token seek; readonly allowed; overlay session handle; virtual deferred | migrate boot seek |
| `50:01` | `demChMod`, `demfile.c` | DS:DX DOS path; AL get/set; CX attrs | `Get/SetFileAttributes`; ordinary API | direct metadata mutation; readonly source-shaped refusal; overlay metadata backend; virtual deferred | replace readonly leaf |
| `50:02` | `demClose`, `demhndl.c` | AX:BP token; optional CX:DX final seek | `CloseHandle`; ordinary API | all supported profiles release their own opaque token; redirector pipe cleanup deferred | migrate boot close |
| `50:03,22` | `demCreate`, `demCreateNew`, `demfile.c` | DS:SI path; CX attrs; returns AX:BP handle | `CreateFile`; ordinary API | direct create/create-new; readonly refusal; overlay namespace+handle creation; virtual deferred | replace readonly leaf |
| `50:04,06` | `demCreateDir`, `demDeleteDir`, `demdir.c` | DS:DX DOS path | `CreateDirectory` / `RemoveDirectory`; ordinary APIs | direct namespace mutation; readonly refusal; overlay namespace backend; virtual deferred | replace readonly leaf |
| `50:05` | `demDelete`, `demfile.c` | DS:DX DOS path | `DeleteFile`; ordinary API | direct deletion; readonly refusal; overlay tombstone/backend; virtual deferred | replace readonly leaf |
| `50:07,20` | `demDeleteFCB`, `demRenameFCB`, `demfcb.c` | FCB paths and DTA/FCB records in checked guest ranges | `FindFirst/Next`, attributes, delete/move; ordinary APIs | direct FCB namespace semantics; readonly refusal; overlay backend; virtual deferred | replace FCB failure helper |
| `50:08` | `demFileTimes`, `demhndl.c` | AX:BP token; BL get/set/device; CX:DX DOS date/time | `Get/SetFileTime`, conversion APIs; ordinary APIs | direct metadata mutation for set; readonly allows get/refuses set; overlay metadata backend; virtual deferred | replace readonly helper |
| `50:09,0B` | `demFindFirst`, `demFindNext`, `demsrch.c` | DTA registration plus DS:DX path / DTA continuation | historical directory query; adapter-private `NtQueryDirectoryFile` seam already exists | direct selected-root enumeration; readonly identical read; overlay merged enumeration; virtual deferred | migrate profile search snapshot |
| `50:0A,0C` | `demFindFirstFCB`, `demFindNextFCB`, `demsrch.c` | ES:DI FCB path and DS:SI SRCHBUF/DTA continuation | same directory enumeration + copied FCB records | direct selected-root FCB search; readonly identical read; overlay merged enumeration; virtual deferred | migrate FCB search fixture |
| `50:12` | `demOpen`, `demfile.c` | DS:SI path; BL access/share; returns AX:BP token, CX:BX size, DX pipe flag | `CreateFile`; ordinary API; named-pipe branch requires absent `VDMREDIR` | direct local-file token; readonly grants only compatible read token; overlay token/backend; virtual deferred; pipes remain deferred | replace boot readonly open |
| `50:16` | `demRead`, `demhndl.c` | AX:BP token; CX bytes; DS:DX checked output range; BX:SI position/ZF | `ReadFile`; ordinary API | direct/readonly read; overlay resolves overlay/base; virtual deferred | replace boot readonly read |
| `50:17` | `demRename`, `demfile.c` | DS:DX source; ES:DI target | `MoveFile`; ordinary API; source preserves cross-drive AX=11 | direct same-drive rename; readonly refusal; overlay rename backend; virtual deferred | replace readonly leaf |
| `50:1E` | `demWrite`, `demhndl.c` | AX:BP token; CX bytes; DS:DX checked input range; BX:SI position/ZF | `WriteFile`, `SetEndOfFile`; ordinary APIs | direct write/truncate; readonly refusal; overlay writes; virtual deferred | replace generic failure |
| `50:27` | `demCommit`, `demhndl.c` | AX:BP token | `FlushFileBuffers`; ordinary API | direct/readonly/overlay preserve CF-clear even flush failure; virtual deferred | retain only as token-aware behavior |
| `50:2C,2D,2E,2F,30,31` | FCB create/open/close/I/O/date/file-info, `demfcb.c` | FCB/SRCHBUF/DTA plus guest buffer ranges and per-process FCB state | `CreateFile`, pointer, read/write, time/attribute APIs; ordinary APIs | direct opaque FCB/token state; readonly read-only variants; overlay backend; virtual deferred | replace `dem_fcb_provider_v1` fence |
| `50:44` | `demCheckPath`, `demfile.c` | DS:SI path and DL drive; checked result | `CreateFile` path probe in source | direct selected-root path validation; readonly same read; overlay path view; virtual deferred | migrate special device-path helper |
| `50:47,48` | `demPipeFileDataEOF`, `demPipeFileEOF`, `demhndl.c` | AX:BP handle and pipe state | historical `VDMREDIR` named-pipe table | explicit deferred/unavailable until Redirector package supplies pipe-token ownership | retain source-labelled unavailable fence |

## Boundary findings

1. `GetVDMAddr`, `Sim32GetVDMPointer`, `Sim32FlushVDMPointer`, and
   `Sim32FreeVDMPointer` are historical CCPU/SAS mechanisms, not usable modern
   APIs. Every pathname, FCB/DTA/SRCHBUF record, input buffer and output buffer
   must therefore travel through bounded gather/read/write transactions.
2. Host `HANDLE` values cannot be placed in AX:BP or guest memory. The next
   package implementation needs one session-owned opaque file-token table;
   this is a provider state boundary, not a replacement DOS handle table.
3. Direct host file APIs are allowed under ordinary caller permissions. A
   denied share/ACL/path operation feeds the source `demClientError` mapping;
   it is not a reason to silently use readonly behavior.
4. Named-pipe/VDMREDIR paths are not local filesystem support. They remain
   deferred with Redirector; no `HANDLE`, callback, or overlapped structure
   may cross the bx-vdm boundary.
5. Overlay must be a complete namespace/handle view, not individual write
   traps. Virtual boot volume remains separately deferred.

## Current-workaround disposition

`boot_namespace_provider`, `readonly_namespace`, `profile_search_snapshot`,
the FCB provider and readonly namespace failure provider are retained only as
fixture/profile evidence pending migration or replacement by the complete
S3 provider. They are not direct-host defaults. `demCommit`'s CF-clear rule
is a retained source behavior, but must acquire an opaque token before it is
considered implemented.

## Exact ABI, continuation and failure work set

The following is the required implementation contract for the later whole
package provider.  It is intentionally more precise than a dispatcher table:
it identifies the bounded input/output transactions and the historical result
form that the provider must preserve.  It does **not** authorize implementing
one row before the whole package is admitted.

| Identity group | Checked guest transactions and session state | Source-shaped success and failure result |
| --- | --- | --- |
| `50:00,02,08,16,1E,27` handle operations | Decode AX:BP as a session-owned opaque file token, never as `HANDLE`.  `00` copies CX:DX/BL; `02` copies optional CX:DX final position; `08` copies BL/CX:DX; `16` gathers a CX-byte output range at DS:DX; `1E` gathers a CX-byte input range at DS:DX; `27` has no guest buffer.  `16`/`1E` copy BX:SI only when the original ZF condition requests a seek. | `00` returns the 32-bit position in DX:AX.  `02` preserves the null-token success branch and otherwise closes the owned token.  `08` returns DOS time/date in CX:DX for get and applies the source's device-time branch.  `16` returns the transferred byte count in AX and preserves broken-pipe EOF only for the later Redirector token kind.  `1E` treats CX=0 as truncate/extend at the selected position and preserves its own disk-full `AX=0,CF=0` branch.  The distinct FCB I/O disk-full form is `50:2F` (`CX=partial, AX=1, CF=1`).  `27` preserves its CF-clear completion form even where `FlushFileBuffers` reports failure. |
| `50:01,03,04,05,06,12,17,22,44` pathname operations | Bounded OEM pathname gather: DS:DX for `01/04/05/06/17` source, ES:DI for `17` destination; DS:SI for `03/12/22/44`.  Paths resolve only through the selected session root/CWD context.  Open/create results allocate an opaque token and copy it to AX:BP; no host handle reaches registers. | Ordinary failures take the source `demClientError` form (AX DOS/Win32 error, CF=1).  `17` retains source same-path `AX=5,CF=1` and cross-drive `AX=11,CF=1`.  `12` returns CX:BX size and DX pipe flag; local files set DX=0, while named-pipe discovery is deferred with Redirector.  `44` retains `\\DEV\\` success and its source-shaped path probe result, but must not create a host file as a by-product of validation. |
| `50:09,0B` path search | `09` gathers DS:DX wildcard and reads/writes the current DTA range identified by the existing DTA state.  `0B` reads/writes that same DTA range.  A session-owned search token associates the DTA location with the enumeration; it cannot be a host pointer or a value trusted from guest reserved bytes. | `09` clears continuation state before a new search, preserves volume-label-first ordering, maps file-not-found to no-more-files and bad-path/directory to path-not-found.  `0B` with absent, altered, or stale continuation returns `ERROR_NO_MORE_FILES` and clears its continuation.  Both copy `attr,time,date,size,8.3-name` into the DOS DTA before CF clear. |
| `50:0A,0C` FCB search | `0A` gathers ES:DI wildcard and reads/writes the DS:SI `SRCHBUF`; `0C` reads/writes that `SRCHBUF`.  The continuation is session-owned and keyed to the checked record location plus a non-pointer opaque id. | Both retain extended-FCB attribute selection and volume-label branches.  `0C` missing/mismatched continuation or a volume next request returns `AX=ERROR_NO_MORE_FILES,CF=1`; it clears continuation state.  Successful records preserve blank-padded 8.3 fields, attributes, DOS date/time and 32-bit size. |
| `50:07,20,2C,2D,2E,2F,30,31` FCB file operations | `07` gathers ES:DI wildcard plus AL/DL attributes; `20` gathers DS:SI source and ES:DI template; `2C/2D/31` gather DS:SI pathname; `2E/2F` decode AX:SI or AX:BP opaque token as specified by the original owner.  `2F` reads the DTA location and performs a bounded read/write there, never a borrowed SAS pointer. | `07` preserves wildcard and extended-FCB attribute rules, including no-match `AX=ERROR_FILE_NOT_FOUND,CF=1`.  `20` preserves wildcard destination substitution and same-path `AX=5,CF=1`.  `2C/2D` return token, time/date and size in their historical register form; `2E` preserves null-token CF-clear.  `2F` returns CX transfer and AX:BX size, including the disk-full partial-write branch.  `30` is local-clock DOS date/time; `31` returns attribute/time/date/size. |
| `50:47,48` pipe EOF | Decode AX:BP only as a typed token.  There is no local-filesystem fallback or host pointer/callback transaction. | Explicitly deferred: only a future Redirector package may supply pipe-token state and the original COMMAND/VDMREDIR EOF semantics.  Until then each call uses the documented unavailable disposition, not a fake local-file success. |

### Guest record layouts and token rule

`dosdef.h` defines packed historical records.  The next provider must copy the
guest-visible non-pointer fields at their original byte positions:

- `SRCHDTA`: 21-byte reserved prefix, then attributes, DOS write time/date,
  low/high size and a 13-byte name.
- `SRCHBUF`: drive byte, 8-byte name, 3-byte extension, block/record-size and
  size, followed by `DIRENT` with 8.3 name, attributes, reserved continuation
  area, DOS time/date and size.

The historical `pFFindEntry` fields inside those reserved areas were host
pointers on 32-bit NT.  A 64-bit CLI provider must never serialize either a
pointer or a native `HANDLE` there.  It may retain the byte layout and write a
non-address opaque identifier only if the session-owned continuation table
also verifies the checked DTA/SRCHBUF location and generation before a next
operation.  A copied record is therefore data, never authority to dereference
host state.

### Error classifier boundary

`demerror.c:demClientErrorEx` has two source-distinct outcomes.  Ordinary
errors become `AX=(USHORT)GetLastError(), CF=1` (with access denied if the
last error is zero).  The range `ERROR_WRITE_PROTECT..ERROR_GEN_FAILURE` and
`ERROR_WRONG_DISK` instead arm the historical INT 24 hard-error packet and
set CF; they do not use the ordinary AX assignment.  The new provider must
express that distinction through its typed result record and the available
guest hard-error owner; it may not silently collapse a hard error into a
readonly policy refusal.  If that owner is not admitted when implementation
starts, the package needs an explicit source-derived unavailable result and a
separate owner admission -- not a hidden adapter-side INT 24 implementation.

### Current route reconciliation

The following mechanical cross-check makes the prior broad workaround note
actionable.  Every S3 identity appears once; these are **current** routes, not
claims of direct-host implementation.  `replace` means replace the leaf with
the later single package provider; `migrate` means retain only as fixture
input while moving its contract into that provider; `defer` means no local
filesystem substitute is permitted.

| BOP | Current bx-vdm route | Required action |
| --- | --- | --- |
| `50:00` | `boot_namespace_provider_v1_seek` | migrate |
| `50:01` | readonly failure leaf | replace |
| `50:02` | `boot_namespace_provider_v1_close` | migrate |
| `50:03` | readonly failure leaf | replace |
| `50:04` | readonly failure leaf | replace |
| `50:05` | readonly failure leaf | replace |
| `50:06` | readonly failure leaf | replace |
| `50:07` | `dem_fcb_provider_v1` access-denied fence | replace |
| `50:08` | readonly failure leaf | replace |
| `50:09` | `profile_search_snapshot` transaction | migrate |
| `50:0A` | FCB search fixture in boot namespace plane | migrate |
| `50:0B` | `profile_search_snapshot` transaction | migrate |
| `50:0C` | FCB search fixture in boot namespace plane | migrate |
| `50:12` | boot readonly open transaction | migrate |
| `50:16` | boot readonly read transaction | migrate |
| `50:17` | readonly failure leaf | replace |
| `50:1E` | DEM CLI unavailable terminal | replace |
| `50:20` | `dem_fcb_provider_v1` access-denied fence | replace |
| `50:22` | readonly failure leaf | replace |
| `50:27` | readonly CF-clear leaf | replace with token-aware commit |
| `50:2C` | `dem_fcb_provider_v1` access-denied fence | replace |
| `50:2D` | `dem_fcb_provider_v1` access-denied fence | replace |
| `50:2E` | `dem_fcb_provider_v1` null-token special case/fence | replace |
| `50:2F` | `dem_fcb_provider_v1` access-denied fence | replace |
| `50:30` | `dem_fcb_provider_v1` local-clock special case | migrate |
| `50:31` | `dem_fcb_provider_v1` access-denied fence | replace |
| `50:44` | boot namespace device-path helper | migrate |
| `50:47` | readonly invalid-handle leaf | defer to Redirector package |
| `50:48` | readonly invalid-handle leaf | defer to Redirector package |

This check deliberately excludes `50:32/33`: they are DEM error/lock owner
work, not a file/handle/FCB/search identity, and therefore cannot be smuggled
into S3 merely because hard-error classification is shared evidence.

## Follow-up

The remaining S3 pass must reconcile each current bx-vdm workaround with this
contract and demonstrate mechanically that all 29 identities have one
non-overlapping row.  Only then may one subsequent S implement the entire
file/handle/FCB/search package and its family regression.
