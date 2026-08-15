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

## Follow-up

The next S3 evidence pass will add the exact DOS structure/range layouts,
`demClientError` branch conditions, and per-row direct/readonly/overlay
failure disposition. Only then may one subsequent S implement the entire
file/handle/FCB/search package and its family regression.
