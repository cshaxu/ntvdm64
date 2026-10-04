# T427 S1 root and search audit

## Status and inputs

S1 is in progress. This first audit delivery contains source conclusions and
an actual selected-image reproducer, not production repair or S1 closure.
Baseline: 4f5aae2e3, accepted T426 S3/r001/runtime. No product file or O:/winnt
is modified. The probe uses the unchanged eight-file package copied below build.

## Actual application-shadowing reproduction

Test sources: [identity target](../../../tests/observation/application_search_identity_probe.c)
and [baseline runner](../../../tests/observation/verify-application-search-baseline.ps1).
The target is windowless GUI code compiled x86 /MT /W4 /WX; it writes its
actual GetModuleFileNameW path and exits 37. Real run16 --wait submits it
through the production native route. This is not a model of SearchPath.

Build: VsDevCmd.bat -arch=x86 -host_arch=x64, then cl /nologo /W4 /WX /MT
/DUNICODE /D_UNICODE tests/observation/application_search_identity_probe.c,
explicit /Fo and /Fe below build/M0-T427/S1/r001, /SUBSYSTEM:WINDOWS.

Run: tests/observation/verify-application-search-baseline.ps1 -PackageRoot
build/M0-T426/S3/r001/runtime -Probe build/M0-T427/S1/r001/search-identity.exe
-RunRoot build/M0-T427/S1/r002. Results: r002/results.json. Both ordered PATH
directories contain the same target; the package directory is absent from PATH
and is not CWD. All four cases returned the actual target result 37:

| Case | Actual selected image below r002 | Interpretation |
| --- | --- | --- |
| Package and CWD both contain target | runtime/search-identity.exe | Defect: package shadows CWD. |
| Explicit CWD image path | cwd/search-identity.exe | Explicit path works in this case. |
| CWD target removed; both PATH directories contain target | runtime/search-identity.exe | Defect: package shadows PATH. |
| Package target removed | path-first/search-identity.exe | First PATH directory wins once package probe cannot win. |

These assertions intentionally document the baseline defect, not desired-policy
acceptance. S3 must add independent desired-policy assertions, including suffix
conflicts, missing explicit paths and direct/nested execution. The current
fixture does not prove interactive COMMAND or host EDIT selection.
Cleanup uses the existing exact-path/creation-identity isolated scope; no name
kill, Z: mapping, interactive desktop, production protocol or helper is added.

## Source-backed caller ledger, first pass

Locations below name original versus project-owned roles; a similar filename
does not imply equivalent semantics. Tests listed are required follow-ups,
not passes unless explicitly recorded above.

| Caller/source | Provenance and role | Disposition and failure contract | Required assertion |
| --- | --- | --- | --- |
| run16/main.c sibling_path, connect_broker | Project adapter; product ntsrv.exe | Shared own-image root plus declared sibling; missing sibling fails, never PATH fallback. | Missing sibling, wrong-package live service, inherited false root. |
| run16/main.c resolve_image_path | Project adapter; user target | Remove package-first and default SearchPath(NULL); directory-first search, explicit path authoritative. | Actual selected-image matrix; r002 proves current shadowing. |
| run16/main.c begin_worker_win16_directory | Project adapter; guest environment | Replace duplicate own-image derivation; WIN16DIR projection is not native root authority. Audit ANSI guest block before guest SYSTEMROOT projection. | Native host root unchanged; guest Windows/system/system32 independently correct. |
| NTSRV frontend_registry.c create frontend; worker_registry.c worker spawn | Project adapters; internal executable paths | Shared own-image root for ntcon/ntvdm/ntvwm. Preserve existing creation/reservation/rollback owners. | Relocation, missing executable, mismatched participant rejection. |
| NTVDM package_layout.c/session roots | Project adapter/state; guest media | Shared self-root, retain original declared layout and 64-byte COMMAND configuration limits. Do not fabricate a short path. | Relocated roots and unrepresentable short-root explicit failure. |
| NTVDM command_process_compat.c create launcher | Project adapter; internal run16 plus user tail | Sibling run16 explicit; generated COMMAND.COM /c must use explicit internal interpreter, unlike user command. | Package excluded from PATH; shell tail and direct/nested targets still work. |
| NTVDM mvdm_shadow_registry.c | Project adapter; NTVDM.REG | Shared self-root replaces duplicate image derivation; retain optional-file/state semantics. | Relocation and no CWD fallback; preserve user registry state. |
| mvdm_softpc_firmware.c directory facades | Project adapter; guest root/system32 | Keep facade return/encoding/failure contract; replace only root source. | Required system32 absent fails; DLL module directory does not redefine EXE root. |
| Original cmdconf.c WriteExpanded and startup shell | Original logic with existing guest-directory binding; product config | Existing bounded %SystemRoot% expansion uses achSysRoot; do not globally replace native environment variables. Explicit config paths stay explicit. | CONFIG/AUTOEXEC/media selected correctly and immutable bytes unchanged. |
| Original cmdenv.c cmdCreateVDMEnvironment | Original DOS-to-native environment transform | It copies guest variables except guest COMSPEC, preserving native COMSPEC. A new guest SYSTEMROOT projection would leak without a narrow native-boundary restoration. | DOS -> native child sees real host SYSTEMROOT; no guest patch. |
| Original Win16 ldboot.asm get_windir, userpro.asm, ldopen.asm | Original guest loader/API; guest Windows/system and module system32 | WIN16DIR overrides Windows-directory source; SYSTEMROOT separately populates real-Windows base; kernel location remains module root. Do not merge these paths or patch KRNL386. | Actual guest environment and all three paths, relocated package. |
| Original cmdredir.c temp fallback / cmdpif.c | Original host/user semantics with directory binding | TMP/TEMP/GetTempPath remain host/user. Product Windows-directory temp fallback needs separate disposition; PIF current-dir/config paths are not package search. | Configured temp and PIF respected; no unintended product-root scratch. |
| WOW32 public USER font facade | Project adapter calling real GetWindowsDirectoryW; real host | Keep host fonts path, not guest system root. | Native Windows directory unchanged. |
| Original XACTSRV apiwksta.c | Original real-host System directory caller | Do not redirect by spelling; formal reachability still to be completed. | Selected graph/caller proof or explicit nonselected disposition. |
| NTCON, NTVWM, NTMON main paths | Project EXEs; shared contract consumers | Root must be available without inventing media needs; full resource/loader sweep remains open. | Own actual EXE root and package join validation for each consumer. |

## Original DOS ordering

Original [path1.asm](../../../src/mvdm/dos/v86/cmd/command/path1.asm)
Path_Search first calls search on the specified path/current directory;
explicit path failure stops search. Only then does path_loop visit user PATH.
Its documented early success applies even if that directory supplies EXE/BAT.
Original [path2.asm](../../../src/mvdm/dos/v86/cmd/command/path2.asm)
Search ranks .COM=8, .EXE=4, .BAT=2 within that directory; an explicit extension
is retained. This supports directory-first, not extension-first global lookup.
The current run16 .PIF slot is a separate compatibility concern to resolve
against cmdpif/source before implementation, not silently assume DOS ranks it.
Drive-relative paths and empty PATH segments likewise need explicit cases.

## Embedded ROM versus file resources

Original nt_rez.c has a project binding which intercepts ROMS_REZ_ID and calls
mvdm_softpc_firmware_read_embedded_rom; BIOS1/BIOS4/V7VGA use EXE RCDATA.
That proves ROM startup does not require root/softpc. It does not prove every
host_find_file caller is dead. Original cmos.c read_cmos/write_cmos use
CMOS_REZ_ID, with cmos_pickup and cmos_terminate callers compiled under NTVDM.
Determine whether selected composition reaches pickup/terminate before deleting
firmware_root. No external ROM directory or new CMOS persistence is added.

## Next audit obligations and implementation boundary

Finish full selected caller/resource inventory, actual Win16 environment
construction, CMOS/non-ROM reachability, PIF extension policy, module loads,
host directory callers and same-package join authority. Current RPC identity
checks authenticate local process/logon/session, but the reviewed security
helper does not itself check package root; do not equate authentication with
same-package validation.

Smallest proposed shared owner is the existing common library, usable by all
six native EXEs. Worker-base stays worker-only. One checked own-EXE root
mechanism should replace local derivations; original DOS/WOW execution,
scheduler, environment transform order and DLL loader policy stay at owners.
S1 cannot close until the pending ledger/assertions are resolved.
