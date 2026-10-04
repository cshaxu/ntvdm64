# T427 S1 root and search audit

## Status and inputs

S1 source/contract audit is complete after the follow-up below. This delivery
contains source conclusions and an actual selected-image reproducer, not a
production repair or a claim that guest directory isolation already works.
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
The following source/graph follow-up resolves the audit obligations; runtime
assertions remain mandatory implementation gates for S2/S3, not audit passes.

## Completed reachability and contract follow-up

The formal T426 S2/r001/build.ninja (retained S3 production dependency cache)
selects obj/system/cmosnt.obj and lists it in original-softpc-system.lib.
The original base/system/sources selects the same translation unit. Its
cmos_pickup at line 1080 explicitly uses static initialization/post instead
of external files. The cmos.c read/write rows above describe an unselected
alternative, not reached production. All selected host_read_resource callers
reduce to ROM loading; no selected non-ROM host_write_resource caller remains.
Keep the mirror bodies; remove the project's unused firmware-root dependency
without requiring or manufacturing root/softpc. Embedded resources stay intact.

The actual parent construction is run16 launch_vdm -> independent MULTI_SZ
from begin_worker_win16_directory -> original BaseCreateVDMEnvironment ->
ANSI CheckVDM record plus Unicode worker delivery. Original vdm.c normalizes
SYSTEMROOT/WINDIR/PATH values but does not choose another directory authority.
KRNL386 consumes the ANSI DOS environment through topPDB/PDB_environ, not the
native worker process block. Therefore a WOW-only guest SYSTEMROOT projection
can be applied to the ANSI record AFTER original conversion, leaving the
Unicode native-worker environment and native SYSTEMROOT unchanged. DOS task
environment need not acquire that WOW-only projection. Native child tests
must verify this distinction and user-provided host SYSTEMROOT remains host.

Additional selected caller dispositions:

| Caller | Role and original/source basis | Implementation/failure test |
| --- | --- | --- |
| cmd.c cmdInit root; cmdkeyb.c KB16.COM | Guest system32, existing directory facade | Root-derived declared media; missing/oversize retains original failure/skip. |
| nt_pif.c default _default.pif | Optional product default; explicit PIF remains user | Root default only when no supplied path; optional absence keeps original defaults. |
| nt_fulsc.c old font/hardware path | Original historical full-screen system directory | Preserve compiled-out hardware boundary; no external VGA/font requirement invented. |
| nt_bop.c SafeLoadLibrary WOW32/VDMREDIR | Known internal native DLLs, original floating-point save/restore | Narrow known-module binding to declared package paths; missing fails without CWD/PATH replacement. |
| nt_bop.c arbitrary VDD; nt_msscs.c configured VDD | Explicit configured/native module contract | Preserve supplied name/path and existing optional error behavior, not general package-first module search. |
| WOW32 wkfileio.c VDMREDIR; wow32.c initialization load | Internal product module | Use existing module boundary, not module directory as process root; test relocated redirector/WOW startup. |
| WOW32 native printing/multimedia/generic thunk; VDMREDIR DLCAPI | Real-host native modules or explicit requested module | Keep Windows loader/API contract; do not reroute host DLLs through guest system32. |
| XACTSRV apiwksta.c workstation lanroot | Selected original-opennt-xactsrv library, native RAP workstation response | Keep real GetSystemDirectoryW; source proof is not a new API runtime pass. |
| NTCON/NTVWM/NTMON native resource accesses | Native EXE resources, shared control client, no independent media root discovery found | All gain common self-root availability and join checks without fictional media reads. |

Original COMMAND suffix ranking is COM/EXE/BAT; PIF is not in that guest
ranking. cmdpif.c handles an explicitly requested PIF and restricts its StartFile
to COM/EXE/BAT. Preserve that original parser/configuration path. For run16's
existing bare-name PIF compatibility, retain it after the three DOS executable
suffixes within EACH directory rather than allowing it ahead of a BAT in CWD.
Explicit suffixes remain exact; no application-name exemptions are allowed.
Drive-qualified X:foo is an explicit drive-relative path, not ordinary PATH
lookup. Empty/unset PATH adds nothing beyond CWD; empty entries repeat CWD.

Temporary-file fallback in cmdredir.c is currently a guest-directory facade
used in a host scratch role. Use a narrow host temp/directory binding for that
fallback, preserving original retry/error order; do not make product root the
scratch fallback. Existing system.ini shadow already uses GetTempPath and
delete-on-close, and retains immutable source configuration.

Cross-package isolation belongs at the existing authentication/join boundary:
server authenticates the actual passed process then compares its actual EXE
directory to its own; the BaseClient checks the authenticated broker process
capability before publishing the connection. Compare directory object identity
so a subst alias does not create a false mismatch. No new protocol, environment
authority, reconnect, helper, scheduler or task registry is necessary.

S2 implements shared checked own-image/root/resource mechanics and authenticated
join validation; S3 implements directory-first user search and explicit internal
COMMAND. Guest-only WOW projection, known product DLL binding and host-temp
correction stay bounded S2 caller adaptations, with immutable guest hashes and
retained WOW frontiers. Each changed mirror expression needs the existing
provenance register and minimum same-shaped hook; no original execution moves.
