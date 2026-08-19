# T229 S2 — file/handle/FCB/search conformance ledger

## Purpose

This ledger is the semantic-completion checklist for all 29 DEM identities in
the `file-handle-fcb-search` owner group. It supplements the S1 source/ABI
ledger: a typed route is not counted as semantic completion.

## Evidence classes

- **P**: source-shaped Direct/Readonly behavior has a local whole-provider
  conformance test.
- **R**: route/ABI audit exists, but no meaningful input-state conformance
  proof yet.
- **D**: explicitly deferred to another owner package; no local filesystem
  substitute is permitted.

| BOP | OpenNT handler | Present semantic evidence | Next package work |
| --- | --- | --- | --- |
| `50:00` | `demChgFilePtr` | P — opaque handle seek, result ABI and invalid-token error | add nonzero/origin matrix |
| `50:01` | `demChMod` | P — CX query, Direct set/Readonly refusal | source error variants |
| `50:02` | `demClose` | P — opaque token release | final-location and null-token variants |
| `50:03` | `demCreate` | P — whole-provider create + owner/PDB ABI | attribute/error variants |
| `50:04` | `demCreateDir` | P — Direct create, Readonly refusal | source error variants |
| `50:05` | `demDelete` | P — whole-provider Direct delete/Readonly refusal | source error variants |
| `50:06` | `demDeleteDir` | P — Direct remove | source error variants |
| `50:07` | `demDeleteFCB` | P — Direct wildcard delete, Readonly refusal | extended-FCB/attribute variants |
| `50:08` | `demFileTimes` | P — Direct set, pre-host Readonly refusal | get/device-time variants |
| `50:09` | `demFindFirst` | P — Direct/Readonly DTA first/next and no-match terminal | stale-DTA/attribute variants |
| `50:0A` | `demFindFirstFCB` | P — checked FCB search transaction | host enumeration/extended-FCB matrix |
| `50:0B` | `demFindNext` | P — Direct/Readonly DTA continuation after FindFirst | stale-DTA/attribute variants |
| `50:0C` | `demFindNextFCB` | P — continuation/no-more-files | host enumeration variant |
| `50:12` | `demOpen` | P — whole-provider token/size/pipe ABI | share/access and pipe-token variants |
| `50:16` | `demRead` | P — checked output, ZF cursor continuation and EOF | host-read error variants |
| `50:17` | `demRename` | P — whole-provider Direct rename/Readonly refusal | cross-drive/same-path variants |
| `50:1E` | `demWrite` | P — Direct write, pre-host Readonly refusal | zero-length/disk-full variants |
| `50:20` | `demRenameFCB` | P — two-path Direct rename + pre-enumeration Readonly refusal | wildcard/template/error variants |
| `50:22` | `demCreateNew` | P — whole-provider collision/refusal ABI | success/attribute variants |
| `50:27` | `demCommit` | P — token-aware Direct/Readonly CF-clear | invalid-handle variant |
| `50:2C` | `demCreateFCB` | P — Direct create/result layout + pre-host Readonly refusal | source attribute/error variants |
| `50:2D` | `demOpenFCB` | P — OEM path, `AX:BP` token | share/access matrix |
| `50:2E` | `demCloseFCB` | P — `AX:SI` token release | null-token variant |
| `50:2F` | `demFCBIO` | P — DTA read; Direct write/Readonly refusal | partial/disk-full variants |
| `50:30` | `demDate16` | R | local-clock result layout |
| `50:31` | `demGetFileInfo` | P — `BX:DI` size | attributes/error variants |
| `50:44` | `demCheckPath` | R | ordinary/`\\DEV\\`/missing path matrix |
| `50:47` | `demPipeFileDataEOF` | D — Redirector | only Redirector may supply pipe-token owner |
| `50:48` | `demPipeFileEOF` | D — Redirector | only Redirector may supply pipe-token owner |

## Current P evidence

- P1: whole-provider `demOpenFCB`, `demGetFileInfo`, `demFCBIO`,
  `demCloseFCB`, `demCreateDir`, `demDeleteDir`.
- P2: `demFileTimes` metadata and `demWrite` owner-level Readonly gates.
- P3: `demDeleteFCB` wildcard mutation.
- P4: `demOpen` whole-provider PDB/path transaction and `AX:BP`, `BX:CX`, `DX` success ABI.
- P5: `demCreate` Direct creation/close and pre-host Readonly refusal.
- P6: `demCreateNew` Direct collision and pre-host Readonly refusal.
- P7: `demDelete` Direct deletion and pre-host Readonly refusal.
- P8: `demRename` Direct two-path rename and pre-host Readonly refusal.
- P15: `demFindFirst`/`demFindNext` Direct/Readonly DTA continuation and no-match terminal.

## Completion rule

This row set becomes complete only when every P/R row has a source-backed
Direct and Readonly outcome with its relevant ABI/failure matrix, and the two
D rows remain explicitly deferred under the later Redirector package. Default
73-route fixture reachability never upgrades an R row to P.