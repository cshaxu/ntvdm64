# T453 S1 files, directories, environment and OEM owner matrix

## Result

S1 closes the source-contract stage for Queue package W04.  The selected
original code is already present in the production NTVDM closure; this review
found no autonomous file, current-directory, environment or OEM service to
remove.  The next stage is therefore a finite behaviour/cleanup verification
and repair stage, not a replacement implementation exercise.

This is static source evidence.  It does not claim that every W04 operation is
runtime accepted.

## Build and source denominator

`src/mvdm/oemuni/sources` selects exactly `file.c` and `process.c` for the
original OEM library.  `src/mvdm/dos/dem/sources` selects the DEM consumers,
including `demdir.c`, `demfile.c` and `demfcb.c`; `src/mvdm/wow32/sources`
selects `wdos.c`, `wkfileio.c`, `wkman.c`, `wkernel.c`, `wudlg.c`, `wkgthunk.c`
and `wcommdlg.c`.  The stable formal producer graph builds
`original-mvdm-host-oemuni.lib`, `original-mvdm-dem.lib` and
`original-mvdm-command.lib`, and links all three into `ntvdm.exe` through
`tools/build/New-T310OriginalSoftpcNinja.ps1`.

The comparison baseline is pinned `O:/repos.external/OpenNT`.  Current
normalized differences are deliberately narrow:

| Source | Difference | Disposition |
| --- | --- | --- |
| `oemuni/file.c` | OEM byte-capacity, required-size and initialized-output corrections (DIV-269/274/275/276). | Retain: the original wrappers remain owner; corrections prevent capacity wrap, uninitialized pointer publication and false success. |
| `oemuni/process.c` | Complete Unicode query before OEM sizing and bounded OEM/ANSI environment conversion (DIV-269/273/275/276/277). | Retain: no parallel environment provider; prior focused/guest evidence covers the corrected conversion paths. |
| `wow32/wdos.c` | One shared `demClientErrorEx` declaration include (DIV-294). | Retain: ABI declaration only. |
| `wow32/wkfileio.c` | Default-off failure observation (DIV-340). | Retain pending S2 trace review; it observes the selected original failure and does not choose an open result. |
| `wow32/wcommdlg.c` | Existing scoped callback/message-lifetime guards (DIV-335/349). | Retain: callback lifetime boundary, not a directory implementation. |
| `demfile.c`, `demfcb.c`, command files | Bounded guest-address leases/copies where CCPU40 cannot expose a durable host alias. | Retain: the original body remains the operation owner; the adapter owns only a call-scoped guest mapping. |

`demdir.c` has no semantic source delta (its raw diff is line-ending-only).

## Authority matrix

| Family and original owner | Entry/callers | Authority and representation | Normal and failure/retirement contract | Existing executable evidence / S2 obligation |
| --- | --- | --- | --- | --- |
| OEM file wrappers, `oemuni/file.c` | `CreateFileOem`, attribute, delete, move, full/current directory and enumeration wrappers; consumers include DEM, FCB, WOW KERNEL and COMMAND. | Wrapper owns OEM-to-Unicode conversion and last-error shape; Windows owns files/handles/current native directory.  A caller owns guest registers, DOS error packaging and its path buffer. | Exact `GENERIC_READ` gets a `FILE_WRITE_ATTRIBUTES` attempt then an identical-argument read retry; no broader access augmentation.  Conversion overflow is `ERROR_FILENAME_EXCED_RANGE`; wrapper never owns a caller's handle. | `tests/mvdm/oemuni/*` fixtures and `tools/audit/Verify-T420S37OemGuest.ps1` cover bounded wrapper/guest families.  S2 adds/recovers only missing production delete/rollback witnesses. |
| OEM environment/process wrappers, `oemuni/process.c` | OEM get/set environment, search, temporary path/file and PIF-facing path calls. | Wrapper owns OEM→Unicode→ANSI conversion for the executed environment API.  Windows owns the process environment. `=X:` is a special name, not an ordinary-variable projection. | Allocation/conversion failure preserves wrapper error/cleanup; caller decides whether a failed publication changes guest state.  No new environment cache or retry layer. | `oemuni_family`, failure, DBCS, expansion and PIF fixtures; guest command environment marker in `Verify-T420S37OemGuest.ps1`. S2 verifies retained child/PIF carrier only where reached. |
| DOS per-drive synchronization, `wow32/wdos.c` | `GetCurrentDir`, `SetCurrentDir`, `QueryCurrentDir`, `DosWowSetCurrentDirectory`, `UpdateDosCurrentDirectory`; native-call users in KERNEL/GDI/USER/common-dialog paths. | Four separate facts: host cwd, DOS CDS/`CurDirs`, current TDB saved directory/owner, and `=X:` environment. `wdos.c` owns their original sequential publication; no adapter may cache another current directory. | `SetCurrentDir` first changes host cwd, then `=X:`; later allocation can fail after that. `DosWowSetCurrentDirectory` then updates CDS/long path/TDB.  This is original partial publication, not a transaction to "improve". Directional returns are source-shaped and not aggregate success guarantees. | `Verify-T420S37OemGuest.ps1`, `verify-dem-directory-reset.ps1`, and `verify-win16-directory-probe.ps1` are existing entrypoints. S2 must explicitly exercise DOS→NT and NT→DOS with short/long paths and the native dialog direction. |
| DOS DEM directory, `demdir.c` and `demgset.c` | INT21 create/remove/query/chdir and default drive. | DEM owns DOS register/`demClientError` result; OEM wrapper owns native conversion; CDS and `=X:` ownership remains `wdos`/DOS. | Query invalid path resets CDS/env to root; set-directory publishes cwd then `=X:` without invented rollback. | Existing OEM guest test exercises directory create/delete and COMMAND readback. S2 isolates failure/reset ordering rather than replacing it. |
| DOS handles/files, `demfile.c` | INT21 open/delete/chmod/create and VDD handle association. | Original DEM owns DOS share/open mode, CD-ROM retry, JFT/SFT and DOS errors. Call-scoped mapping adapter owns only temporary pathname/guest-structure access. Windows owns backing handles. | Handle close/release follows original JFT/SFT order.  Guest mapping failure returns the original handler's error path before an open; it must not create a shadow SFT/JFT service. | Existing VDD/DEM lifecycle evidence and OEM guest create/rename/attribute read witness. S2 targets failure cleanup only if a real route exposes a gap. |
| FCB delete/open, `demfcb.c` | FCB delete, get-info and open/create. | FCB owner controls read-only clearing, find continuation and metadata registers; OEM wrapper only performs the host operation. | Delete can clear read-only only for the extended-FCB condition; it must continue/error as the original body specifies.  DTA mapping lease is released once on every normal/failure branch. | OEM guest FCB marker and prior DTA lifetime fixtures. S2 adds no generic delete policy. |
| WOW file thunks, `wkfileio.c` | `_lopen`, `_lcreat`, attributes and `NormalizeDosPath`. | Original KERNEL thunk owns Win16 frame, DOS access/share conversion, trailing-separator and `FFFFxxxx` return packaging. `NormalizeDosPath` owns its borrowed/static/pipe result distinction. | Preserve drive-relative versus rooted C-drive spelling, 8.3 final-component truncation and pipe allocation cleanup. Callers free only the named-pipe result. | Existing formal source fixture; S2 gives the selected path/attribute branches a reachable Win16 witness before changing code. |
| WOW delete workaround, `wkman.c::WK32WowDelFile` | Win16 `WowDelFile` only. | Original KERNEL code owns success probe, temp rename/delete rollback and font-removal fallback. OEM wrappers own each host pathname operation. | Direct delete success is not enough: visible post-delete file triggers rename→temp delete; failed cleanup deletes temporary file, then tries font release and final delete. | No current real guest witness for the retained-file and font fallback branches. This is a mandatory S2/S3 bounded acceptance gap, not grounds for a new delete service. |
| Native directory boundaries | `wudlg.c::WU32DlgDirList`, `wcommdlg.c` common dialog messages/calls, `wkgthunk.c::WK32LoadLibraryEx32`, selected GDI/KERNEL callers. | Original caller owns whether to import a native cwd change. `wdos.c` owns directional sync; Windows owns the process-wide ambient cwd. | Dialog paths perform DOS→NT then NT→DOS and intentionally retain a changed native directory. LoadLibraryEx does DOS→NT only.  A blanket save/restore would change original observable behavior. | S2 needs one native dialog/changing-directory witness and one non-dialog load path; it must not use a global cwd lock or task-private host cwd fiction. |
| COMMAND/PIF | `cmdmisc.c`, `cmdenv.c`, `cmdpif.c`. | Original COMMAND owns its `=X:` lookup and PIF validation/policy. `oemuni` owns conversion; firmware's bounded OEM→ANSI step exists only at the original ANSI `cmdconf/nt_msscs` boundary. | PIF start directory validates/shortens then sequentially updates `=X:`, host cwd and current drive; no fake PIF/environment service. | Existing command environment and PIF fixtures. S2 retains known external PIF console limitations as non-pass, but verifies reachable ordinary environment behavior. |

## Decisions and finite S2 boundary

1. No source body is replaced, and no existing autonomous implementation is
   eligible for deletion: every examined host adaptation is either a
   call-scoped guest mapping or a numbered capacity correction within the
   selected original owner.
2. S2 may change only a proved consumer/binding defect exposed by the following
   finite matrix: both `UpdateDosCurrentDirectory` directions; direct and
   retained-file `WK32WowDelFile` branches; DEM and FCB delete branches;
   ordinary COMMAND environment/PIF publication; Win16 file open/create/
   attributes with normal, conversion/buffer, and cleanup results.
3. S2 must not change `wdos.c`'s original partial-publication order or turn its
   non-transactional Boolean results into a new transaction.  It must not
   introduce a global cwd lock, per-task host cwd cache, new OEM conversion
   engine, generic file service, or guest change.
4. If a stage proves no connected defect in a selected row, it records the
   witness and leaves the original code unchanged.  S3 owns only non-ASCII and
   retained-font fallback consumers; S4 owns the package-wide repeated/failure
   acceptance pass.

## Static limitations

This review deliberately does not claim runtime correctness for PIF console
identity, a filesystem's retained-delete behavior, all modern codepages,
native-dialog reentry, or allocation failures after partial current-directory
publication.  These are named S2/S3 test conditions, not implicit defects or
permission for speculative repairs.
