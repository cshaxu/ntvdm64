# M0 T261 S2 — OpenNT host function-recovery ledger

## Question

For every non-identical production source/header below `src/opennt-host`, what
historical OpenNT caller or provider actually defines its contract, which
historical dependency prevents direct composition, and what final component and
path must own the result?

## Method

- Read the live declaration and implementation surface of all 47 files from
  the T261/S1 inventory.
- Locate the corresponding historical body and call sites under `refs/opennt`.
- Compare the live seam with the source's argument/register layout, ordering
  and failure/return path.  Do not infer direct reuse merely from a matching
  service number or function spelling.
- Classify the first usable recovery rung: `DIRECT`, `FACADE`, `COMPOSITION`,
  or `EXCEPTION`. `FACADE` means an OpenNT-shaped dependency facade, not a
  replacement provider. `EXCEPTION` means that no historical body can be
  independently composed and the row needs an individual README/register entry.

## Binding ledger

Each row covers every listed C source and header. `Final target` is binding for
T261 layout work; it can change only with a successor ledger entry that shows
why the identified source route is wrong.

| Live file(s) | Original evidence and retained contract | Historical dependency / required failure behavior | First usable rung and final target |
| --- | --- | --- | --- |
| `command/command_opennt_pif_parser.c` | `softpc.new/host/src/nt_pif.c`: `GetPIFData`, `GetPIFConfigFiles`, `SetPifDefaults`; `dos/command/cmdpif.c` is the provider caller. | The original body itself is already available unchanged; this wrapper only supplies composition symbols and must not replace it. Original parser failure remains its `BOOL`/dialog path. | `COMPOSITION`: move to a distinct PIF composition name under `app` or `opennt-bop`; retain original `nt_pif.c` separately at its re-rooted path. |
| `command/command_pif_shim.c`, `command/command_pif_shim.h` | `nt_pif.c` uses OEM path, environment, title and allocation helpers with the shown Win32-shaped signatures. | OEM conversion/public path APIs are available; private UI/resource behavior is not. Preserve `GetPIFData`'s `BOOL` and source error ordering; no parser reimplementation. | `FACADE`: split same-shaped OEM/host calls to `adapter-win32`; import the original body in `opennt-host/softpc.new/host/src/nt_pif.c` with only registered includes/divergences. |
| `command/command_config_shim.c` | `nt_pif.c:GetPIFConfigFiles` and `dos/command/cmdconf.c` consume CONFIG/AUTOEXEC path selection in source order. | Historical process/global configuration composition is absent. Preserve empty/not-found source result rather than synthesizing boot files. | `FACADE`: OpenNT-shaped config query facade in `opennt-host/softpc.new/host/...` if source proof reaches it; otherwise individual host exception with the exact source caller. |
| `command/command_binary_shim.c` | `dos/command/cmdexec.c` and `softpc.new/host/src/nt_error.c` map `NTSTATUS` through `RtlNtStatusToDosError`. | Direct old import is not a stable app contract; modern `ntdll` export is usable behind an adapter. Preserve exact DOS-error conversion, never collapse all errors. | `FACADE`: move the same-shaped `RtlNtStatusToDosError` entry to `adapter-win32`; leave source callers in imported COMMAND/host bodies. |
| `command/command_environment_shim.c` | `dos/command/cmdenv.c:cmdXformEnvironment` and `cmdCreateVDMEnvironment` own environment filtering, sorting and multisz layout. | NT4 process-environment assumption cannot be reused as a global product shell. Preserve session-owned multisz input, source ordering and allocation/failure outcome. | `COMPOSITION`: provider source stays in `opennt-bop/dos/command`; session storage belongs to `app` or a narrowly named BOP composition seam, not `opennt-host`. |
| `command/command_misc_shim.c`, `command/command_misc_shim.h` | `dos/command/cmdmisc.c`, `cmdexec.c`, `cmdkeyb.c`, `cmddisp.c` retain the 17-slot dispatch order, register/guest spans, command lifecycle and console notification ordering. | `GetVDMAddr`/SAS must use the one bounded adapter-softpc mapping lease; BaseSrv/CSR, LPT and VDD product services are unavailable. Their source failure/no-op dispositions must remain explicit. | Split: `FACADE` entries for same-shaped Win32/SoftPC calls go to adapters; `COMPOSITION` session/worker/continuation logic goes to `opennt-bop` or `app`; no generic host `command_misc_shim` remains. |
| `command/command_native_session_shim.c`, `command/command_native_session_shim.h` | `cmdexec.c:cmdCreateProcess`, `cmdExec32`; `cmdmisc.c:cmdGetNextCmd`; `cmdexit.c` define the lifecycle call order. | Historical CCPU worker, CSR/BaseSrv broker and process-global standard-handle replacement are not directly composable. Preserve single-session pending/completion, child failure and cleanup semantics only. | `COMPOSITION`: move to a named session scheduler under `app`/`opennt-bop`; public child-process primitives, if needed, are `adapter-win32`. |
| `config/config_complete_shim.c` | `softpc.new/host/src/config.c` and `nt_bop.c` configuration-complete path; `UMBNotify` is a machine/configuration notification, not a BOP provider. | The full historical configuration/SAS shell is unavailable; false completion is prohibited. | `COMPOSITION`: route/notification moves to `opennt-bop` or `app`; a finite machine notification uses `adapter-softpc`. |
| `dem/bx_ntvdm_host_drive_policy.c`, `dem/bx_ntvdm_host_drive_policy.h` | `demgset.c` uses current drive/current directory and DPB facts; historical policy is spread over DOS and host startup rather than one reusable host TU. | This project-specific CLI admission policy has no direct host source body. Preserve explicit excluded-drive failure, never create an implicit replacement drive. | `EXCEPTION`: retain as named host capability only if S3/S4 confirms no original host provider; register source caller, session input and failure behavior in `opennt-host/README.md`. |
| `dem/bx_ntvdm_host_namespace.c`, `dem/bx_ntvdm_host_namespace.h` | `demfile.c`, `demdir.c`, `demsrch.c`, `demfcb.c` own file/directory/search behavior and OEM argument ordering. | Modern public filesystem APIs can supply host capability, but do not replace original DEM algorithms or error mapping. | `EXCEPTION` pending direct-source proof: retain only a narrowly named host filesystem capability; imported DEM callers remain in `opennt-bop`, and Win32 compatibility primitives move to `adapter-win32`. |
| `dem/dem_common_shim.h`, `dem/dem_drive_policy_shim.h` | `dos/dem/dem.h`, `demdisp.c` and related declarations are the original declaration closure. | Wrapper typedefs/macros must not become a competing ABI; drive policy is project admission, not a DEM source definition. | `COMPOSITION`: delete/replace with exact imported declarations and a clearly named app/BOP policy interface; no `*_shim.h` final path. |
| `dem/dem_direct_host_session.c`, `dem/dem_direct_host_session.h` | `demdisp.c` selects original providers; `demhndl.c` and `demfile.c` consume handle and guest-memory contracts. | Direct session wiring is not an OpenNT host TU. The shared handle manager and mapping leases must retain close/release failure behavior. | `COMPOSITION`: move to `opennt-bop` session ingress or `app`; host handles remain opaque and adapter mechanics remain outside `opennt-host`. |
| `dem/dem_native_session_shim.c`, `dem/dem_native_session_shim.h` | `softpc.new/host/src/nt_bop.c` performs BOP fetch/decode; `demdisp.c` dispatches the DEM service. | A generic-UD event/copy-outcome is project mechanical composition, not historical host capability. No selector meaning may enter adapter-softpc. | `COMPOSITION`: move ingress to `opennt-bop`; typed CPU event/result transport remains `adapter-softpc`. |
| `dem/demdasd_ioctl_shim.c`, `dem/demdasd_ioctl_shim.h` | `dos/dem/demdasd.c` and `demioctl.c` use register/DS:BX structures, preserve BIOS/DOS error and CF paths; `spckbd.asm`/SoftPC are related machine callers. | `GetVDMAddr`, SAS/IVT and `host_simulate` cannot use raw host pointers or recursive Bochs execution. Existing source-proven `ERROR_CALL_NOT_IMPLEMENTED`/CF branch remains for unavailable recursion. | Split: imported DEM bodies stay `opennt-bop/dos/dem`; finite copied state, RAM/IVT and same-shaped SoftPC calls move to `adapter-softpc`; do not retain host shim. |
| `dem/demdir_shim.c`, `dem/demdir_shim.h` | `dos/dem/demdir.c:demCreateDir`, `demDeleteDir`, current-directory functions; OEM path pointer comes from `GetVDMAddr`. | Public directory APIs are usable, but guest pointer lifetime must be mapping-manager bounded. Preserve source `SetLastError`/CF ordering. | `FACADE`: OEM directory APIs go to `adapter-win32`; original `demdir.c` remains the provider in `opennt-bop`; BOP/session mapping comes from `adapter-softpc`. |
| `dem/demdisp_shim.c`, `dem/demdisp_shim.h` | `dos/dem/demdisp.c` is the original 50:xx dispatcher and owner map. | No host capability exists here; a hand-written dispatcher risks reordered service dispatch. | `COMPOSITION`: move/remove wrapper into `opennt-bop` ingress and call imported `demdisp.c`; no host file survives. |
| `dem/demerror_lock_shim.c`, `dem/demerror_lock_shim.h` | `demerror.c:demSetHardErrorInfo`, `demRetry`; `demlock.c:demLockOper`; structures are guest-addressed. | Unicode/device-link `ntdll` calls need same-shaped adapter-win32 facades; guest structure access needs adapter-softpc bounded mapping. | Split: source bodies remain `opennt-bop/dos/dem`; Win32 facade moves `adapter-win32`; memory/register facade moves `adapter-softpc`. Preserve source error/CF retry order. |
| `dem/demfile_shim.c`, `dem/demfile_shim.h` | `dos/dem/demfile.c` owns create/delete/rename/attributes and VDD DOS-handle helper ordering. | OEM file functions map through public Win32; Redirector/VDD protocol is unavailable or separately owned. Preserve source error mapping and do not invent remote success. | `FACADE`: OEM file APIs to `adapter-win32`; shared handle manager through `adapter-softpc`/app service; original `demfile.c` remains `opennt-bop`. |
| `dem/demgset_shim.c`, `dem/demgset_shim.h` | `dos/dem/demgset.c` reads CDS/DPB/DTA/PDB/SFT guest structures and invokes `SetCurrentDirectoryOem`. | All guest spans need mapping leases; host current-directory API is public. Preserve source layout and failed-directory result. | Split: source body in `opennt-bop`; mapping calls in `adapter-softpc`; OEM directory facade in `adapter-win32`; project drive admission remains the named host exception. |
| `dem/demhndl_shim.c`, `dem/demhndl_shim.h` | `dos/dem/demhndl.c:demClose`, `demRead`, `demWrite`, `demChgFilePtr`, pipe EOF helpers use DOS register ABI, guest buffers and host HANDLEs. | Raw 32-bit host handles and raw guest pointers are invalid on modern x64. Preserve source sequence by using the one session handle manager and bounded guest-memory lease. | `FACADE`: original body remains `opennt-bop`; same-shaped handle/memory facade is split between `adapter-softpc` and the shared host-handle service; no second table and no generic host shim. |
| `dem/demmisc_shim.c`, `dem/demmisc_shim.h` | `dos/dem/demmisc.c:demEntryDosApp`, loader/stack helpers and direct host notifications; original calls include VDD hooks, debugger, floppy/Fdisk paths. | Normal public file/path pieces can recover; VDD broker/debugger/FDC product mechanisms cannot. Preserve explicit no-op or failure per source branch, never synthetic device completion. | Split: original DEM provider is `opennt-bop`; public APIs are `adapter-win32`; finite machine calls are `adapter-softpc`; individually retain unavailable VDD/debug/FDC exceptions. |
| `dem/demsrch_fcb_shim.c`, `dem/demsrch_fcb_shim.h` | `dos/dem/demsrch.c:demFindFirst/demFindNext`, `demfcb.c` preserve DTA/FCB layout and search/error rules. | OEM find API can be public; `NtVdmControl(VdmQueryDir)` and VDD user hook require unavailable historical product services. | `FACADE` for OEM find + mapping; original bodies stay `opennt-bop`; explicit unavailable VDD/QueryDir branch remains source-shaped, not a hidden host fallback. |
| `redir/redir_session_shim.c`, `redir/redir_session_shim.h` | Historical `VDMREDIR` loader, pipe/mailslot completion and `cmdredir.c`/DEM callers form one Redirector package. | DLL/broker composition is absent; local named-pipe primitives alone are not VDMREDIR. Preserve unavailable/declined remote behavior until Redirector package is admitted. | `COMPOSITION`: move to Redirector BOP/package or `app` session scheduler; public pipe helpers, if retained, are `adapter-win32`; remove from host. |
| `top_level/top_level_nosupport_shim.c`, `top_level/top_level_nosupport_shim.h` | `softpc.new/host/src/nt_error.c:host_direct_access_error` and `DisplayErrorTerm`; BOP 59 caller preserves AX reason then abort/ignore behavior. | NT4 resource/WOW/CSRSS shell is unavailable; public dialog is sufficient for non-WOW one-session prompt. Terminate must stop only the VDM session; Ignore resumes source branch. | `FACADE`: recover `nt_error.c` under `opennt-host/softpc.new/host/src` with registered UI/session divergences; typed stop/resume stays adapter/app composition. |
| `xms/xms_shim.c`, `xms/xms_shim.h` | `xms.486/{xms.c,xmsdisp.c,xmsa20.c,xmsmisc.c,xmsumb.c}` retain XMS dispatch table/order, A20, allocator and UMB failure behavior. | SAS/GetVDMAddr, A20, RAM, INT 15 and UMB physical mapping are mechanical; no DOS/XMS semantic may move into mantle/core. | `FACADE`: original XMS provider stays `opennt-bop/xms`; all same-shaped CPU/RAM/A20/IVT mechanics move to `adapter-softpc`; UMB requires a separately admitted opaque physical-span seam. |
| `xms/xms_native_session_shim.c`, `xms/xms_native_session_shim.h` | `nt_bop.c` fetches XMS service; `xmsdisp.c:XMSDispatch` executes it. | Generic-UD transport is project composition, not XMS host capability. | `COMPOSITION`: move to `opennt-bop` ingress; typed event/copy mechanics stay `adapter-softpc`; remove from host. |

## Cross-row dependency order

1. Recover final adapter-win32 and adapter-softpc headers before moving an
   imported body, so the import never retains a generic shim include.
2. Move BOP ingress/session composition out of `opennt-host` before moving
   DEM, COMMAND or XMS providers; it prevents a host library from owning
   selector or continuation meaning.
3. Re-root the three direct PIF imports, then compose their named adapter
   calls; this is the first content-preserving layout move.
4. Recover COMMAND and DEM providers by their original `dos/...` paths under
   `opennt-bop`, while retaining only host capability bodies under an original
   `softpc.new/host/...` path or an individual exception.
5. Move adapter-bound mechanics and delete the superseded shim only after all
   current consumers use the named facade. XMS and Redirector are last because
   their source bodies share the machine/worker seams.

## Result and confidence

All 47 non-identical files now have a source-family, source function/caller,
historical dependency, retained failure rule and final-owner decision. The
strongest evidence is the source bodies already imported below `opennt-bop` and
the exact paths listed above. The two remaining exception candidates are the
project drive policy and namespace capability: they have source callers but no
single independently composable historical host translation unit. S3/S4 may
move code only in this ordering and must register every imported-body edit as a
statement-level `DIVERGENCE(<ID>)` in the target component README.
