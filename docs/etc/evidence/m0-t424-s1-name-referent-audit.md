# T424 S1 — Component name referent audit

S2 normalizes project-worker/record identity labels in this historical record.
Exact pre-migration names, command arguments and path inventory remain in Git
at the S1 delivery `3c8e6b7d1` and sealed build/M0-T424/S1/r001 output. Hashes,
dates, observed counts and outcomes below are unchanged. Normalized file labels
do not claim those basenames existed in the pre-migration published package;
reproduce that package audit with its recorded revision, not new source names.

## Question and inputs

Rename the project Win32 text worker and its project-owned task record without
renaming real Windows Console APIs or original OpenNT Console source identities.
Inputs are main `90a82e154`, the owner-accepted T423 S40 `f64559086`, the
admitted naming proposal, the complete Git file list and current untracked files.
This is an audit/documentation delivery, not runtime capability implementation.

## Reproduction and coverage

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/audit/Get-ComponentRenameInventory.ps1 -OutputRoot build/M0-T424/S1/r001 -WorkerName ntw32 -FrontendName ntkvm -RecordName win32record
```

The script uses Git's case-insensitive binary-excluding tracked text search,
then reads all untracked non-ignored text files; it also scans tracked and
untracked path names. It emits per-occurrence path, line, column, spelling,
context and suggested classification in `occurrences.csv`, path inventory in
`paths.csv`, version/coverage in `summary.json`, and actual published identities
in `published-hashes.json`, all below that build root. No source, guest or
published product is rewritten by the tool. Classification suggestions are
review inputs, not permission to blindly substitute an entire matching line.

Initial reviewed run: 11,511 tracked files, one untracked audit script,
3,553 raw occurrences in 305 files, 106 matching path/token rows. Classes:
569 substring non-identities; 828 original Console source references; 682
frontend occurrences; 1,358 worker/reserved-name candidates; 116 project-record
occurrences. Counts include this audit tool and change as documentation evolves.
The naming proposal contains reserved future frontend names within the worker
candidate bucket; those are not old-worker references and require prose edits
at the intermediate-stage gate, not substitution into a worker name.

## Reviewed migration map

| Origin and current location | Disposition and target | Boundary/invariant |
| --- | --- | --- |
| Former project worker: 14 files (literal paths pinned in S1 Git/inventory) | S2 pure move to `src/ntw32-exe/`; local types/functions use `ntw32_*`, include guards/diagnostic variables use `NTW32_*` | Console capture, input, completion and residency algorithms unchanged. |
| Project NTSRV record in `src/ntsrv-exe/opennt/source/base_service.c` | S2 rename the former native record type/helpers/members to `OPENNT_BASE_WIN32RECORD` and the `win32record` family | Project code despite directory name; preserve fields/order, receipt, error and exit-code source. No original DOSRECORD/WOWRECORD extraction or edit. |
| `run16-exe/frontend_scope.c` native worker image selection | S2 executable basename to `ntw32.exe` | Keep existing native command delivery and launcher argument semantics. |
| `interface/native_launch.h`, service/frame/client comments; worker-base and consumer READMEs | S2 worker identity and guard spelling | Wire fields, sizes, enum values and RPC operations unchanged; real Console names unchanged. |
| `tools/build/New-T310OriginalSoftpcNinja.ps1` | S2 worker source/object/target/library consumer paths | Regenerate MIDL/Ninja; no stale generated graph used as proof. |
| 17 worker-named observation source/script paths plus cross-component tests/observer/product matrices | S2 paths, includes, assertions, diagnostic names and process selection | Existing assertions remain; no dropping legacy-name cases instead of migrating them. |
| `docs/` current design/rules/state, proposals, history and evidence | S2 old-worker referents and associated documentary filename/links | Preserve hash values, dates, outcomes; Git retains literal pre-migration evidence. S3 reserved frontend names must not be mistaken for worker names. |
| Pre-2026-09-10 documentation archive under `artifacts/` | Preserve its original Console-source references | This archive predates the project native worker; paths such as `windows/core/ntcon/client` mean original OpenNT, not the product worker. Do not corrupt provenance ledgers. |
| `ntkvm-exe/window_keyboard.c`, `ntvdm-exe/win32/console_{graphics,compat,client,bitmap}.c`, host-compat README | Preserve original `ntcon/server`, `ntcon/client`, `HandleKeyEvent`, bitmap/private service citations | Refers to OpenNT Windows Console implementation; not project worker. |
| Original `mvdm/softpc.new/host/src/nt_event.c` comment | Preserve `ntcon\client\iostubs.c` | Original source identity. `mvdm/README.md` separately has two project worker references to rename. |
| `GetCurrentConsoleFontEx`, `PrintContext`, `CurrentControlSet`, `NTCONFIGFILE`, `BaseClientConnectRoutine`, client-connect names | Preserve exactly | Substring matches are not identities; genuine Console/configuration/connection API symbols. |
| Old frontend `src/ntkvm-exe/`, executable, tests and product-owned symbols | S3 only, after verified worker-name migration | No simultaneous directory collision; generic imported `kvm-*` library identities unchanged. |
| Existing generated graphs/caches and sealed test logs under `build/` | New graph/current artifacts regenerated; sealed predecessor evidence remains immutable | Not runtime inputs unless explicit matching-input reuse is proved. Historical hash/log names are not silently rewritten. |

## Gate interpretation under the owner's Console boundary

The proposal's initial literal zero-substring gate conflicts with its own requirement
to preserve original identities and with the owner's explicit prohibition on
renaming genuine Console symbols. The required zero is therefore **zero old
project-worker referents**, not mutilation of `GetCurrentConsoleFontEx` or
OpenNT Console source paths. S2 must scan the entire tree, report every surviving
raw hit, and prove each retained occurrence belongs to the reviewed original
Console or substring classes. These are not product aliases. A newly discovered
unclassified occurrence fails the gate. No file/root may be hidden from scanning.

## ABI and mixed-package decision

Current `APP_VERSION` is `0.0.423`; application protocol and MIDL interface
major are both 28, UUID `7d3e78bd-0472-4736-8797-e64e966d692e`. S2 advances
the application version to `0.0.424` per version.h's existing T rule; the
existing Connect/management comparisons reject old application peers. C naming
and executable paths do not alter copied record layout, so protocol/RPC 28 and
UUID remain stable. Endpoint names, access control, Console handles, lifetime,
launch syntax and completion authority remain unchanged. No externally fixed
project-worker identifier was found; original Console source citations above
are outside the requested rename, not new API exceptions.

## Published baseline

Actual O:/winnt SHA-256 values compared exactly with S40's manifest, eight of
eight matching. No deployment occurred during S1.

| File | SHA-256 |
| --- | --- |
| run16.exe | 299A58AB889BB53EC54315791FE4B5CC08B450CDEC2231FCE91C92AB8A1F556E |
| ntsrv.exe | 5566B5C4B5FC86D97A54E6B0F66300AFEB33B6E7973D29DDFD67BA70D3BB76E5 |
| ntvdm.exe | 60155D9B1E8DF83F17AC407B682EFF80033A3A9584CA0CE2314DAFCEE783C0C5 |
| ntw32.exe | 4002CB09C8B732DECA4CC7BA0901359411B368BF36B8CBA4C29DA14294901CB4 |
| ntkvm.exe | 886E2ECC9D8937F17E21A63E634018AC3F8087766632D9BB582B30F731EE1F94 |
| ntmon.exe | 3D86477B95A673B602540952805790DDFE7C4541496819030ADF16DDBD4FBA53 |
| wow32.dll | 0D2AE60264B03A8040D98AA86BCF80455127064084E2217D318D5E13F90FA94A |
| VDMREDIR.dll | 1A2418FE667348EF3C6764A85C375A53B40C00D3ECAF2FBC8F74B2D1DF4D881F |

## Confidence, verification and next action

Source/path/ABI identities are directly checked; runtime behavior has not been
retested or changed in S1. Documentation governance and `git diff --check` are
the delivery gate. S2 receives the complete migration map above, then owns
the rename, semantic diff review, x86 build, focused compatibility/lifecycle
tests, native EDIT return, Console17/Window17, retained WOW frontiers and the
coherent intermediate eight-file publication. S1 does not claim those passes.
